#include "BrainStdAfx.h"
#include "SRF01.h"
#include "physics/constants.h"
#include "utils/Timed_Scope.h"

#include "sz_math.hpp"
#include "sz_SRF01.hpp"

namespace silk
{
namespace node
{


constexpr uint8_t ADDR = 0x70;

//Registers
constexpr uint8_t SW_REV_CMD        = 0x0;
constexpr uint8_t UNUSED            = 0x1;
constexpr uint8_t RANGE_H           = 0x2;
constexpr uint8_t RANGE_L           = 0x3;
constexpr uint8_t AUTOTUNE_MIN_H	= 0x4;
constexpr uint8_t AUTOTUNE_MIN_L	= 0x5;

//Commands
constexpr uint8_t REAL_RAGING_MODE_IN       = 0x50;
constexpr uint8_t REAL_RAGING_MODE_CM       = 0x51;
constexpr uint8_t REAL_RAGING_MODE_US       = 0x52;
constexpr uint8_t FAKE_RAGING_MODE_IN       = 0x56;
constexpr uint8_t FAKE_RAGING_MODE_CM       = 0x57;
constexpr uint8_t FAKE_RAGING_MODE_US       = 0x58;
constexpr uint8_t BURST                     = 0x5C;
constexpr uint8_t FORCE_AUTOTUNE_RESTART    = 0x60;


constexpr std::chrono::milliseconds MAX_MEASUREMENT_DURATION(100);


SRF01::SRF01(HAL& hal)
    : m_hal(hal)
    , m_init_params(new sz::SRF01::Init_Params())
    , m_config(new sz::SRF01::Config())
{
    m_config->direction = math::vec3f(0, 0, -1); //pointing down
}

auto SRF01::get_outputs() const -> std::vector<Output>
{
    std::vector<Output> outputs(1);
    outputs[0].type = stream::IDistance::TYPE;
    outputs[0].name = "Distance";
    outputs[0].stream = m_output_stream;
    return outputs;
}
auto SRF01::init(rapidjson::Value const& init_params) -> bool
{
    QLOG_TOPIC("SRF01::init");

    sz::SRF01::Init_Params sz;
    autojsoncxx::error::ErrorStack result;
    if (!autojsoncxx::from_value(sz, init_params, result))
    {
        std::ostringstream ss;
        ss << result;
        QLOGE("Cannot deserialize SRF01 data: {}", ss.str());
        return false;
    }
    *m_init_params = sz;
    return init();
}

auto SRF01::init() -> bool
{
    m_bus = m_hal.get_buses().find_by_name<bus::IUART>(m_init_params->bus);
    auto bus = m_bus.lock();
    if (!bus)
    {
        QLOGE("No bus configured");
        return false;
    }

    bus->lock();
    At_Exit at_exit([this, &bus]()
    {
        bus->unlock();
    });

    m_output_stream = std::make_shared<Output_Stream>();

    m_init_params->rate = math::clamp<size_t>(m_init_params->rate, 1, 10);

    QLOGI("Probing SRF01 on {}...", m_init_params->bus);

    uint32_t tries = 0;
    constexpr uint32_t max_tries = 10;
    while (++tries <= max_tries)
    {
        uint8_t rev = 0, test = 0;
        auto ret = bus->read_register_u8(ADDR, SW_REV_CMD, rev);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        ret &= bus->read_register_u8(ADDR, UNUSED, test);
        if (ret && rev != 0 && rev != 255 && test != 0 && test != 255)
        {
            QLOGI("Found SRF01 rev {} after {} tries", rev, tries);//rev is 6 so far
            break;
        }
        QLOGW("\tFailed {} try to initialize SRF01: bus {}, rev {}, test {}", tries, ret, rev, test);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    if (tries > max_tries)
    {
        QLOGE("Failed to initialize SRF01");
        return false;
    }

    trigger(*bus);

    m_output_stream->set_rate(m_init_params->rate);
    m_output_stream->set_tp(q::Clock::now());

    return true;
}

void SRF01::trigger(bus::IUART& bus)
{
    m_last_trigger_tp = q::Clock::now();
    bus.write_register_u8(ADDR, SW_REV_CMD, REAL_RAGING_MODE_CM);
}

void SRF01::process()
{
    QLOG_TOPIC("SRF01::process");

    m_output_stream->clear();

    //wait for echo
    auto now = q::Clock::now();
    if (now - m_last_trigger_tp < MAX_MEASUREMENT_DURATION ||
        now - m_last_trigger_tp < m_output_stream->get_dt())
    {
        return;
    }

    auto bus = m_bus.lock();
    if (!bus)
    {
        return;
    }

    bus->lock();
    At_Exit at_exit([this, &bus]()
    {
        bus->unlock();
    });

    std::array<uint8_t, 4> buf;
    bool res = bus->read_register(ADDR, RANGE_H, buf.data(), buf.size());

    //trigger immediately
    trigger(*bus);

    if (res)
    {
        int d = (unsigned int)(buf[0] << 8) | buf[1];
        int min_d = (unsigned int)(buf[2] << 8) | buf[3];

        //QLOGI("d = {}, min_d = {}", d, min_d);

        float distance = static_cast<float>(d) / 100.f; //meters

        float min_distance = math::max(m_config->min_distance, static_cast<float>(min_d) / 100.f); //meters
        float max_distance = m_config->max_distance;
        auto value = m_config->direction * math::clamp(distance, min_distance, max_distance);
        auto is_healthy = distance >= min_distance && distance <= max_distance;

        m_output_stream->clear();
        auto samples_needed = m_output_stream->compute_samples_needed();
        while (samples_needed > 0)
        {
            m_output_stream->push_sample(value, is_healthy);
            samples_needed--;
        }
    }
}

auto SRF01::set_config(rapidjson::Value const& json) -> bool
{
    QLOG_TOPIC("SRF01::set_config");

    sz::SRF01::Config sz;
    autojsoncxx::error::ErrorStack result;
    if (!autojsoncxx::from_value(sz, json, result))
    {
        std::ostringstream ss;
        ss << result;
        QLOGE("Cannot deserialize SRF01 config data: {}", ss.str());
        return false;
    }

    *m_config = sz;
    m_config->min_distance = math::max(m_config->min_distance, 0.1f);
    m_config->max_distance = math::min(m_config->max_distance, 12.f);
    if (math::is_zero(math::length(m_config->direction), math::epsilon<float>()))
    {
        m_config->direction = math::vec3f(0, 0, -1); //pointing down
    }
    m_config->direction.normalize<math::safe>();

    return true;
}
auto SRF01::get_config() const -> rapidjson::Document
{
    rapidjson::Document json;
    autojsoncxx::to_document(*m_config, json);
    return std::move(json);
}

auto SRF01::get_init_params() const -> rapidjson::Document
{
    rapidjson::Document json;
    autojsoncxx::to_document(*m_init_params, json);
    return std::move(json);
}

auto SRF01::send_message(rapidjson::Value const& /*json*/) -> rapidjson::Document
{
    return rapidjson::Document();
}

}
}