#include "BrainStdAfx.h"
#include "RC_Comms.h"
#include "utils/Timed_Scope.h"

#include "utils/comms/RC.h"

//#include "hal.def.h"

//#include <boost/asio.hpp>

using namespace silk;

struct RC_Comms::Channels
{
    Channels()
        : rc(false)
    {}

    util::comms::RC rc;
};

RC_Comms::RC_Comms(HAL& hal)
    : m_hal(hal)
    , m_channels(new Channels())
{
}

auto RC_Comms::start(std::string const& interface, uint8_t id) -> bool
{
    try
    {
//        m_rcp.reset(new util::comms::RCP);

//        {
//            auto s = new util::comms::RFMON_Socket(interface, id);
//            m_video_socket.reset(s);
//            if (!s->start())
//            {
//                throw std::exception();
//            }

//            util::RCP::Socket_Handle handle = m_rcp->add_socket(m_video_socket.get());
//            if (handle >= 0)
//            {
//                m_rcp->set_socket_handle(VIDEO_CHANNEL, handle);
//                m_rcp->set_socket_handle(TELEMETRY_CHANNEL, handle);
//            }
//        }

//        {
//            auto s = new util::RCP_RF4463F30_Socket("/dev/spidev1.0", 16000000, false);
//            m_rc_socket.reset(s);
//            if (!s->start())
//            {
//                throw std::exception();
//            }

//            util::RCP::Socket_Handle handle = m_rcp->add_socket(m_rc_socket.get());
//            if (handle >= 0)
//            {
//                m_rcp->set_internal_socket_handle(handle);
//                m_rcp->set_socket_handle(PILOT_CHANNEL, handle);
//            }
//        }

        m_is_connected = m_channels->rc.init();
    }
    catch(std::exception e)
    {
        m_is_connected = false;
    }

    if (!m_is_connected)
    {
        QLOGW("Cannot start comms on interface {}", interface);
        return false;
    }

    QLOGI("Started sending on interface {}", interface);

    return true;
}

auto RC_Comms::is_connected() const -> bool
{
    return m_is_connected;
}

//auto Comms::send_video_stream(Stream_Telemetry_Data& ts, stream::IStream const& _stream) -> bool
//{
//    if (_stream.get_type() != stream::IVideo::TYPE)
//    {
//        return false;
//    }

//    auto const& stream = static_cast<stream::IVideo const&>(_stream);
//    auto const& samples = stream.get_samples();

//    for (auto const& s: samples)
//    {
//        m_channels->video.begin_pack(comms::Video_Message::FRAME_DATA);
//        m_channels->video.pack_param(ts.stream_name);
//        m_channels->video.pack_param(s);
//        m_channels->video.end_pack();
//        m_channels->video.try_sending(*m_rcp);
//    }
//    return true;
//}

auto RC_Comms::get_multirotor_commands_values() const -> std::vector<stream::IMultirotor_Commands::Value> const&
{
    return m_multirotor_commands_values;
}
void RC_Comms::add_multirotor_state_sample(stream::IMultirotor_State::Sample const& sample)
{
//    m_channels->pilot.pack_all(silk::rc_comms::Pilot_Message::MULTIROTOR_STATE, sample);
//    m_channels->pilot.send(*m_rcp);
}
void RC_Comms::add_video_sample(stream::IVideo::Sample const& sample)
{
//    if (m_rcp)
//    {
//        m_channels->video.pack_all(silk::rc_comms::Video_Message::FRAME_DATA, sample);
//        m_channels->video.send(*m_rcp);
//    }
}

void RC_Comms::handle_multirotor_commands()
{
//    auto& channel = m_channels->pilot;

//    stream::IMultirotor_Commands::Value value;
//    if (!channel.unpack_param(value))
//    {
//        QLOGE("Error in unpacking multirotor commands");
//        return;
//    }

//    m_multirotor_commands_values.push_back(value);
}


void RC_Comms::process()
{
    if (!is_connected())
    {
        return;
    }

    static q::Clock::time_point s_tp = q::Clock::now();
    if (q::Clock::now() - s_tp > std::chrono::milliseconds(100))
    {
        s_tp = q::Clock::now();

        struct Data
        {
            int8_t throttle = 0;
            int8_t yaw = 0;
            int8_t pitch = 0;
            int8_t roll = 0;
        };

        static Data tx_data;
        tx_data.yaw++;

        m_channels->rc.set_tx_data(&tx_data, sizeof(tx_data));

        Data rx_data;
        util::comms::RC::Data rc_rx_data;
        m_channels->rc.get_rx_data(rc_rx_data);
        if (rc_rx_data.rx_data.size() == sizeof(Data))
        {
            rx_data = *reinterpret_cast<Data const*>(rc_rx_data.rx_data.data());
        }

        QLOGI("^{}dBm, v{}dBm, throttle: {}", rc_rx_data.tx_dBm, rc_rx_data.tx_dBm, rx_data.throttle);
    }

    m_multirotor_commands_values.clear();

//    while (m_channels->pilot.get_next_message(*m_rcp))
//    {
//        rc_comms::Pilot_Message msg;
//        if (!m_channels->pilot.begin_unpack() || !m_channels->pilot.unpack_param(msg))
//        {
//            QASSERT(0);
//            m_channels->pilot.end_unpack();
//            break;
//        }

//        switch (msg)
//        {
//        case rc_comms::Pilot_Message::MULTIROTOR_COMMANDS: handle_multirotor_commands(); break;
//        default: QLOGE("Received unrecognised pilot message: {}", static_cast<int>(msg)); break;
//        }
//    }


//    if (m_video_socket)
//    {
//        auto result = m_video_socket->process();
//        if (result != util::comms::ISocket::Result::OK)
//        {
////            m_rcp->reconnect();
//        }
//    }

//    if (m_rc_socket)
//    {
//        auto result = m_rc_socket->process();
//        if (result != util::comms::ISocket::Result::OK)
//        {
//            m_rcp->reconnect();
//        }
//    }

//    m_rcp->process();

//    auto now = q::Clock::now();
//    if (now - m_last_rcp_tp >= RCP_PERIOD)
//    {
//        m_last_rcp_tp = now;

//        //m_channels->setup.send(*m_rcp);
//        m_channels->telemetry.try_sending(*m_rcp);
//    }
}

