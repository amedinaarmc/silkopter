#include "BrainStdAfx.h"
#include "RC5T619.h"
#include "physics/constants.h"
#include "utils/Timed_Scope.h"

#include "sz_math.hpp"
#include "sz_RC5T619.hpp"

#if defined RASPBERRY_PI

extern "C"
{
    #include "hw/pigpio.h"
}

#endif

namespace silk
{
namespace node
{



struct Mode_Guard
{
    Mode_Guard()
    {
#if defined RASPBERRY_PI
        gpioSetMode(28, PI_INPUT);
        gpioSetMode(29, PI_INPUT);
        gpioSetMode(0, PI_ALT0);
        gpioSetMode(1, PI_ALT0);
#endif
    }
    ~Mode_Guard()
    {
#if defined RASPBERRY_PI
        gpioSetMode(28, PI_ALT0);
        gpioSetMode(29, PI_ALT0);
        gpioSetMode(0, PI_INPUT);
        gpioSetMode(1, PI_INPUT);
#endif
    }
};


constexpr uint8_t ADDR = 0x32;

constexpr uint8_t RC5T619_PWR_WD			= 0x0B;
constexpr uint8_t RC5T619_PWR_WD_COUNT		= 0x0C;
constexpr uint8_t RC5T619_PWR_FUNC          = 0x0D;
constexpr uint8_t RC5T619_PWR_SLP_CNT		= 0x0E;
constexpr uint8_t RC5T619_PWR_REP_CNT		= 0x0F;
constexpr uint8_t RC5T619_PWR_ON_TIMSET		= 0x10;
constexpr uint8_t RC5T619_PWR_NOE_TIMSET	= 0x11;
constexpr uint8_t RC5T619_PWR_IRSEL         = 0x15;

/* Interrupt enable register */
constexpr uint8_t RC5T619_INT_EN_SYS		= 0x12;
constexpr uint8_t RC5T619_INT_EN_DCDC		= 0x40;
constexpr uint8_t RC5T619_INT_EN_RTC		= 0xAE;
constexpr uint8_t RC5T619_INT_EN_ADC1		= 0x88;
constexpr uint8_t RC5T619_INT_EN_ADC2		= 0x89;
constexpr uint8_t RC5T619_INT_EN_ADC3		= 0x8A;
constexpr uint8_t RC5T619_INT_EN_GPIO		= 0x94;
constexpr uint8_t RC5T619_INT_EN_GPIO2		= 0x94; // dummy
constexpr uint8_t RC5T619_INT_MSK_CHGCTR	= 0xBE;
constexpr uint8_t RC5T619_INT_MSK_CHGSTS1	= 0xBF;
constexpr uint8_t RC5T619_INT_MSK_CHGSTS2	= 0xC0;
constexpr uint8_t RC5T619_INT_MSK_CHGERR	= 0xC1;
constexpr uint8_t RC5T619_INT_MSK_CHGEXTIF	= 0xD1;

/* interrupt status registers (monitor regs)*/
constexpr uint8_t RC5T619_INTC_INTPOL		 = 0x9C;
constexpr uint8_t RC5T619_INTC_INTEN		 = 0x9D;
constexpr uint8_t RC5T619_INTC_INTMON		 = 0x9E;

constexpr uint8_t RC5T619_INT_MON_SYS		 = 0x14;
constexpr uint8_t RC5T619_INT_MON_DCDC		 = 0x42;
constexpr uint8_t RC5T619_INT_MON_RTC		 = 0xAF;

constexpr uint8_t RC5T619_INT_MON_CHGCTR	 = 0xC6;
constexpr uint8_t RC5T619_INT_MON_CHGSTS1	 = 0xC7;
constexpr uint8_t RC5T619_INT_MON_CHGSTS2	 = 0xC8;
constexpr uint8_t RC5T619_INT_MON_CHGERR	 = 0xC9;
constexpr uint8_t RC5T619_INT_MON_CHGEXTIF	 = 0xD3;


// GPIO register base address
constexpr uint8_t RC5T619_GPIO_IOSEL	 = 0x90;
constexpr uint8_t RC5T619_GPIO_IOOUT	 = 0x91;
constexpr uint8_t RC5T619_GPIO_GPEDGE1	 = 0x92;
constexpr uint8_t RC5T619_GPIO_GPEDGE2	 = 0x93;
//constexpr uint8_t RC5T619_GPIO_EN_GPIR	 = 0x94;
//constexpr uint8_t RC5T619_GPIO_IR_GPR	 = 0x95;
//constexpr uint8_t RC5T619_GPIO_IR_GPF	 = 0x96;
constexpr uint8_t RC5T619_GPIO_MON_IOIN	 = 0x97;
constexpr uint8_t RC5T619_GPIO_LED_FUNC	 = 0x98;

constexpr uint8_t RC5T619_REG_BANKSEL	 = 0xFF;

// Charger Control register
constexpr uint8_t RC5T619_CHG_CTL1	 = 0xB3;
constexpr uint8_t TIMSET_REG		 = 0xB9;

// ADC Control register
constexpr uint8_t RC5T619_ADC_CNT1	 = 0x64;
constexpr uint8_t RC5T619_ADC_CNT2	 = 0x65;
constexpr uint8_t RC5T619_ADC_CNT3	 = 0x66;
constexpr uint8_t RC5T619_ADC_VADP_THL	 = 0x7C;
constexpr uint8_t RC5T619_ADC_VSYS_THL	 = 0x80;

constexpr uint8_t RC5T619_FG_CTRL	 = 0xE0;
constexpr uint8_t RC5T619_PSWR		 = 0x07;

constexpr uint8_t RC5T619_AIN1_DATAH	 = 0x74;
constexpr uint8_t RC5T619_AIN1_DATAL	 = 0x75;
constexpr uint8_t RC5T619_AIN0_DATAH	 = 0x76;
constexpr uint8_t RC5T619_AIN0_DATAL	 = 0x77;


constexpr uint8_t CONVERT_ADC0           = 0x17;
constexpr uint8_t CONVERT_ADC1           = 0x16;

RC5T619::RC5T619(UAV& uav)
    : m_uav(uav)
    , m_init_params(new sz::RC5T619::Init_Params())
    , m_config(new sz::RC5T619::Config())
{
    m_adc[0] = std::make_shared<Stream>();
    m_adc[1] = std::make_shared<Stream>();
}

auto RC5T619::get_outputs() const -> std::vector<Output>
{
    std::vector<Output> outputs(2);
    outputs[0].name = "ADC0";
    outputs[0].stream = m_adc[0];
    outputs[1].name = "ADC1";
    outputs[1].stream = m_adc[1];
    return outputs;
}
auto RC5T619::init(rapidjson::Value const& init_params) -> bool
{
    QLOG_TOPIC("rc5t619::init");

    sz::RC5T619::Init_Params sz;
    autojsoncxx::error::ErrorStack result;
    if (!autojsoncxx::from_value(sz, init_params, result))
    {
        std::ostringstream ss;
        ss << result;
        QLOGE("Cannot deserialize RC5T619 data: {}", ss.str());
        return false;
    }
    *m_init_params = sz;
    return init();
}

auto RC5T619::init() -> bool
{
    m_i2c = m_uav.get_buses().find_by_name<bus::II2C>(m_init_params->bus);

    auto i2c = m_i2c.lock();
    if (!i2c)
    {
        QLOGE("No bus configured");
        return false;
    }

    m_init_params->adc0_rate = math::clamp<size_t>(m_init_params->adc0_rate, 1, 50);
    m_init_params->adc1_rate_ratio = math::clamp<size_t>(m_init_params->adc1_rate_ratio, 1, 100);

    m_adc[0]->rate = m_init_params->adc0_rate;
    m_adc[1]->rate = m_init_params->adc0_rate / m_init_params->adc1_rate_ratio;

    m_dt = std::chrono::milliseconds(1000 / m_init_params->adc0_rate);

    Mode_Guard mg;

    uint8_t control;
    auto res = i2c->read_register_u8(ADDR, 0x36, control);
    if (!res || control == 0xff)
    {
        QLOGE("rc5t619 not found");
#ifdef RASPBERRY_PI
        return false;
#endif
    }
    QLOGI("rc5t619 found: {}", control);

    // Set ADRQ=00 to stop ADC
    res &= i2c->write_register_u8(ADDR, RC5T619_ADC_CNT3, 0x0);

    // Set ADC auto conversion interval 250ms
    res &= i2c->write_register_u8(ADDR, RC5T619_ADC_CNT2, 0x0);

    // Enable AIN0, AIN1 pin conversion in auto-ADC
    res &= i2c->write_register_u8(ADDR, RC5T619_ADC_CNT1, 0xC0);

    // Start auto-mode & average 4-time conversion mode for ADC
    res &= i2c->write_register_u8(ADDR, RC5T619_ADC_CNT3, 0x17);

    //start by converting voltage first
    res &= i2c->write_register_u8(ADDR, RC5T619_ADC_CNT3, CONVERT_ADC1);
    if (!res)
    {
        QLOGI("Failed to init rc5t619");
#ifdef RASPBERRY_PI
        return false;
#endif
    }

    return true;
}

auto RC5T619::start(q::Clock::time_point tp) -> bool
{
    m_last_tp = tp;
    return true;
}

void RC5T619::process()
{
    QLOG_TOPIC("rc5t619::process");

    m_adc[0]->samples.clear();
    m_adc[1]->samples.clear();

    auto i2c = m_i2c.lock();
    if (!i2c)
    {
        return;
    }

    auto now = q::Clock::now();
    if (now - m_last_tp < m_dt)
    {
        return;
    }

    m_last_tp = now;

//    LOG_INFO("ADC{} : {}:{} -> {}", idx, buf[0], buf[1], result);

    // Stop AD conversion */
    //i2c.write_u8(ADDR, RC5T619_ADC_CNT3, uint8_t(0x00));

    Mode_Guard mg;

    std::array<uint8_t, 2> buf;
    if (m_stage == 0)
    {
        //read coltage

        if (i2c->read_register(ADDR, RC5T619_AIN1_DATAH, buf.data(), buf.size()))
        {
            int r = (unsigned int)(buf[0] << 4) | (buf[1]&0xf);
            auto result =  math::clamp(static_cast<float>(r) / 4095.f, 0.f, 1.f);

            Stream::Sample& sample = m_adc[1]->last_sample;
            sample.value = result;
            m_adc[1]->samples.push_back(sample);

            m_adc[1]->last_tp = now;
        }

        //next
        if (i2c->write_register_u8(ADDR, RC5T619_ADC_CNT3, CONVERT_ADC0)) //read current next
        {
            m_stage++;
        }
    }
    else
    {
        //read current

        if (i2c->read_register(ADDR, RC5T619_AIN0_DATAH, buf.data(), buf.size()))
        {
            int r = (unsigned int)(buf[0] << 4) | (buf[1]&0xf);
            auto result =  math::clamp(static_cast<float>(r) / 4095.f, 0.f, 1.f);

            Stream::Sample& sample = m_adc[0]->last_sample;
            sample.value = result;
            m_adc[0]->samples.push_back(sample);

            m_adc[0]->last_tp = now;
        }

        //next
        if (m_stage >= m_init_params->adc1_rate_ratio)
        {
            if (i2c->write_register_u8(ADDR, RC5T619_ADC_CNT3, CONVERT_ADC1)) //read voltage next
            {
                m_stage = 0;
            }
        }
        else if (i2c->write_register_u8(ADDR, RC5T619_ADC_CNT3, CONVERT_ADC0)) //read current next
        {
            m_stage++;
        }
    }
}

auto RC5T619::set_config(rapidjson::Value const& json) -> bool
{
    QLOG_TOPIC("rc5t619::set_config");

    sz::RC5T619::Config sz;
    autojsoncxx::error::ErrorStack result;
    if (!autojsoncxx::from_value(sz, json, result))
    {
        std::ostringstream ss;
        ss << result;
        QLOGE("Cannot deserialize RC5T619 config data: {}", ss.str());
        return false;
    }

    *m_config = sz;
    return true;
}
auto RC5T619::get_config() const -> rapidjson::Document
{
    rapidjson::Document json;
    autojsoncxx::to_document(*m_config, json);
    return std::move(json);
}

auto RC5T619::get_init_params() const -> rapidjson::Document
{
    rapidjson::Document json;
    autojsoncxx::to_document(*m_init_params, json);
    return std::move(json);
}

auto RC5T619::send_message(rapidjson::Value const& /*json*/) -> rapidjson::Document
{
    return rapidjson::Document();
}

}
}
