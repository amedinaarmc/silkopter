#pragma once

#include "common/Comm_Data.h"
#include "HAL.h"

#include "common/Manual_Clock.h"
#include "common/node/ISource.h"
#include "common/stream/IMultirotor_Commands.h"
#include "common/stream/IMultirotor_State.h"
#include "common/stream/IVideo.h"

namespace util
{
namespace comms
{
class RCP;
class ISocket;
}
}

namespace silk
{

class RC_Comms : q::util::Noncopyable
{
public:
    RC_Comms(HAL& hal);

    auto start(std::string const& interface, uint8_t id) -> bool;

    auto is_connected() const -> bool;

    void process();

    auto get_multirotor_commands_values() const -> std::vector<stream::IMultirotor_Commands::Value> const&;
    void add_multirotor_state_sample(stream::IMultirotor_State::Sample const& sample);
    void add_video_sample(stream::IVideo::Sample const& sample);

    struct Channels; //this needs to be public...
private:
    void handle_multirotor_commands();
    //void handle_accept(boost::system::error_code const& error);

//    auto send_video_stream(Stream_Telemetry_Data& ts, stream::IStream const& _stream) -> bool;

    HAL& m_hal;
    q::Clock::time_point m_uav_sent_tp = q::Clock::now();

    std::vector<stream::IMultirotor_Commands::Value> m_multirotor_commands_values;

    std::shared_ptr<Channels> m_channels;

    bool m_is_connected = false;

};


}
