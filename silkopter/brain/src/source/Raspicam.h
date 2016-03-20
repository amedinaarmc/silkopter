#pragma once

#include "UAV.h"
#include "common/node/ISource.h"
#include "common/Comm_Data.h"
#include "common/stream/IVideo.h"


namespace sz
{
namespace Raspicam
{
struct Init_Params;
struct Config;
}
}



namespace silk
{
namespace node
{

class Raspicam : public ISource
{
public:
    Raspicam(UAV& uav);
    ~Raspicam();

    auto init(rapidjson::Value const& init_params) -> bool;
    auto get_init_params() const -> rapidjson::Document;

    auto set_config(rapidjson::Value const& json) -> bool;
    auto get_config() const -> rapidjson::Document;

    auto send_message(rapidjson::Value const& json) -> rapidjson::Document;

    auto start(q::Clock::time_point tp) -> bool override;

    void shutdown();

    auto get_outputs() const -> std::vector<Output>;

    //----------------------------------------------------------------------

    auto start_recording() -> bool;
    void stop_recording();

    //----------------------------------------------------------------------
    struct Impl;
    void process();

    typedef std::function<void(uint8_t const* data, size_t size, math::vec2u32 const& resolution, bool is_keyframe)> Data_Available_Callback;

private:
    UAV& m_uav;

    auto init() -> bool;

    std::shared_ptr<sz::Raspicam::Init_Params> m_init_params;
    std::shared_ptr<sz::Raspicam::Config> m_config;

    void activate_streams();

    std::shared_ptr<Impl> m_impl;

    void streaming_callback(uint8_t const* data, size_t size, math::vec2u32 const& resolution, bool is_keyframe);
    void recording_callback(uint8_t const* data, size_t size, math::vec2u32 const& resolution, bool is_keyframe);
    void create_file_sink();

    auto create_components() -> bool;

    struct Stream : public stream::IVideo
    {
        auto get_samples() const -> std::vector<Sample> const& { return samples; }
        auto get_rate() const -> uint32_t { return rate; }

        uint32_t rate = 0;
        std::vector<Sample> samples;
    };
    mutable std::shared_ptr<Stream> m_stream;

    struct Sample_Queue
    {
        std::mutex mutex;
        std::deque<Stream::Sample> samples;
        size_t count = 0;
    } m_sample_queue;


    struct Recording_Data
    {
        std::mutex mutex;
        std::thread thread;
        std::atomic_bool should_stop = {false};
        std::vector<uint8_t> data_in;
        std::vector<uint8_t> data_out;
        std::shared_ptr<q::data::File_Sink> file_sink;
    } m_recording_data;

};


}
}
