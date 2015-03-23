#pragma once

#include "HAL.h"
#include "common/node/stream/IAngular_Velocity.h"
#include "common/node/stream/IAcceleration.h"
#include "common/node/stream/IMagnetic_Field.h"
#include "common/node/stream/IPressure.h"
#include "common/node/stream/ITemperature.h"
#include "common/node/stream/IDistance.h"
#include "common/node/stream/ILocation.h"
#include "common/node/stream/IPWM.h"
#include "common/node/IProcessor.h"

#include "World.h"


namespace sz
{
namespace Simulator
{
struct Init_Params;
struct Config;
}
}

namespace silk
{
namespace node
{

class Simulator : public IProcessor
{
public:
    Simulator(HAL& hal);

    auto init(rapidjson::Value const& init_params) -> bool;
    auto get_init_params() const -> rapidjson::Document const&;

    auto set_config(rapidjson::Value const& json) -> bool;
    auto get_config() const -> rapidjson::Document;

    auto get_inputs() const -> std::vector<Input>;
    auto get_outputs() const -> std::vector<Output>;

    void process();

private:
    auto init() -> bool;

    HAL& m_hal;

    rapidjson::Document m_init_paramsj;
    std::shared_ptr<sz::Simulator::Init_Params> m_init_params;
    std::shared_ptr<sz::Simulator::Config> m_config;

    q::Clock::time_point m_last_tp;

    struct Angular_Velocity : public stream::IAngular_Velocity
    {
        auto get_samples() const -> std::vector<Sample> const& { return samples; }
        auto get_rate() const -> uint32_t { return rate; }
        uint32_t rate = 0;
        q::Clock::duration accumulated_dt = q::Clock::duration{0};
        std::vector<Sample> samples;
        Sample last_sample;
    };
    struct Acceleration : public stream::IAcceleration
    {
        auto get_samples() const -> std::vector<Sample> const& { return samples; }
        auto get_rate() const -> uint32_t { return rate; }
        uint32_t rate = 0;
        q::Clock::duration accumulated_dt = q::Clock::duration{0};
        std::vector<Sample> samples;
        Sample last_sample;
    };
    struct Magnetic_Field : public stream::IMagnetic_Field
    {
        auto get_samples() const -> std::vector<Sample> const& { return samples; }
        auto get_rate() const -> uint32_t { return rate; }
        uint32_t rate = 0;
        q::Clock::duration accumulated_dt = q::Clock::duration{0};
        std::vector<Sample> samples;
        Sample last_sample;
    };
    struct Pressure : public stream::IPressure
    {
        auto get_samples() const -> std::vector<Sample> const& { return samples; }
        auto get_rate() const -> uint32_t { return rate; }
        uint32_t rate = 0;
        q::Clock::duration accumulated_dt = q::Clock::duration{0};
        std::vector<Sample> samples;
        Sample last_sample;
    };
    struct Temperature : public stream::ITemperature
    {
        auto get_samples() const -> std::vector<Sample> const& { return samples; }
        auto get_rate() const -> uint32_t { return rate; }
        uint32_t rate = 0;
        q::Clock::duration accumulated_dt = q::Clock::duration{0};
        std::vector<Sample> samples;
        Sample last_sample;
    };
    struct Distance : public stream::IDistance
    {
        auto get_samples() const -> std::vector<Sample> const& { return samples; }
        auto get_rate() const -> uint32_t { return rate; }
        uint32_t rate = 0;
        q::Clock::duration accumulated_dt = q::Clock::duration{0};
        std::vector<Sample> samples;
        Sample last_sample;
    };
    struct ECEF_Location : public stream::IECEF_Location
    {
        auto get_samples() const -> std::vector<Sample> const& { return samples; }
        auto get_rate() const -> uint32_t { return rate; }
        uint32_t rate = 0;
        q::Clock::duration accumulated_dt = q::Clock::duration{0};
        std::vector<Sample> samples;
        Sample last_sample;
    };

    mutable std::shared_ptr<Angular_Velocity> m_angular_velocity_stream;
    mutable std::shared_ptr<Acceleration> m_acceleration_stream;
    mutable std::shared_ptr<Magnetic_Field> m_magnetic_field_stream;
    mutable std::shared_ptr<Pressure> m_pressure_stream;
    mutable std::shared_ptr<Temperature> m_temperature_stream;
    mutable std::shared_ptr<Distance> m_distance_stream;
    mutable std::shared_ptr<ECEF_Location> m_ecef_location_stream;

    std::vector<stream::IPWM_wptr> m_input_pwm_streams;

    World m_world;
};


}
}
