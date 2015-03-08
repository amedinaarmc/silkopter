﻿#include "BrainStdAfx.h"

#include "HAL.h"
#include "utils/Json_Helpers.h"
#include "utils/Timed_Scope.h"

#include "rapidjson/document.h"     // rapidjson's DOM-style API
#include "rapidjson/prettywriter.h" // for stringify JSON
#include "rapidjson/stringbuffer.h"



/////////////////////////////////////////////////////////////////////////////////////

#include "bus/I2C_Linux.h"
#include "bus/SPI_Linux.h"
#include "bus/UART_Linux.h"

#include "source/Raspicam.h"
#include "source/MPU9250.h"
#include "source/MS5611.h"
#include "source/RC5T619.h"
#include "source/SRF02.h"
#include "source/UBLOX.h"
#include "sink/PIGPIO.h"

#include "processor/ADC_Voltmeter.h"
#include "processor/ADC_Ammeter.h"
#include "processor/LPF.h"
#include "processor/Resampler.h"
#include "processor/LiPo_Battery.h"
#include "processor/Inertial.h"
#include "processor/Comp_AHRS.h"
#include "processor/Comp_Location.h"

//#include "common/node/IAHRS.h"

#include "autojsoncxx/boost_types.hpp"


namespace silk
{

static const q::Path k_settings_path("settings.json");

using namespace boost::asio;

//wrapper to keep all nodes in the same container
struct INode_Wrapper : q::util::Noncopyable
{
    virtual void process() = 0;
};
template<class T> struct Node_Wrapper : public INode_Wrapper
{
    template<class... Args>
    Node_Wrapper(Args&&... args) : node(new T(std::forward<Args>(args)...)) {}
    void process() { node->process(); }
    std::unique_ptr<T> node;
};


///////////////////////////////////////////////////////////////

HAL::HAL()
    : m_bus_factory(*this)
    , m_node_factory(*this)
{
    using namespace node;

    QLOG_TOPIC("hal");

    m_bus_factory.register_node<bus::UART_Linux>("UART_Linux");
    m_bus_factory.register_node<bus::I2C_Linux>("I2C_Linux");
    m_bus_factory.register_node<bus::SPI_Linux>("SPI_Linux");

    m_node_factory.register_node<MPU9250>("MPU9250");
    m_node_factory.register_node<MS5611>("MS5611");
    m_node_factory.register_node<SRF02>("SRF02");
    m_node_factory.register_node<Raspicam>("Raspicam");
    m_node_factory.register_node<RC5T619>("RC5T619");
    m_node_factory.register_node<UBLOX>("UBLOX");

    m_node_factory.register_node<PIGPIO>("PIGPIO");

    m_node_factory.register_node<ADC_Ammeter>("ADC_Ammeter");
    m_node_factory.register_node<ADC_Voltmeter>("ADC_Voltmeter");
    m_node_factory.register_node<Comp_AHRS>("Comp_AHRS");
    m_node_factory.register_node<Comp_Location>("Comp_Location");
    m_node_factory.register_node<Inertial>("Inertial");
    m_node_factory.register_node<LiPo_Battery>("LiPo_Battery");
    m_node_factory.register_node<LPF<stream::IAcceleration>>("Acceleration_LPF");
    m_node_factory.register_node<LPF<stream::ILinear_Acceleration>>("Linear_Acceleration_LPF");
    m_node_factory.register_node<LPF<stream::IAngular_Velocity>>("Angular_Velocity_LPF");
    m_node_factory.register_node<LPF<stream::IADC_Value>>("ADC_Value_LPF");
    m_node_factory.register_node<LPF<stream::ICurrent>>("Current_LPF");
    m_node_factory.register_node<LPF<stream::IVoltage>>("Voltage_LPF");
    m_node_factory.register_node<LPF<stream::ILocation>>("Location_LPF");
    m_node_factory.register_node<LPF<stream::IDistance>>("Distance_LPF");
    m_node_factory.register_node<LPF<stream::IMagnetic_Field>>("Magnetic_Field_LPF");
    m_node_factory.register_node<LPF<stream::IPressure>>("Pressure_LPF");
    m_node_factory.register_node<LPF<stream::ITemperature>>("Temperature_LPF");
    m_node_factory.register_node<LPF<stream::IReference_Frame>>("Reference_Frame_LPF");
    m_node_factory.register_node<LPF<stream::IPWM_Value>>("PWM_Value_LPF");
    m_node_factory.register_node<Resampler<stream::IAcceleration>>("Acceleration_Resampler");
    m_node_factory.register_node<Resampler<stream::ILinear_Acceleration>>("Linear_Acceleration_Resampler");
    m_node_factory.register_node<Resampler<stream::IAngular_Velocity>>("Angular_Velocity_Resampler");
    m_node_factory.register_node<Resampler<stream::IADC_Value>>("ADC_Value_Resampler");
    m_node_factory.register_node<Resampler<stream::ICurrent>>("Current_Resampler");
    m_node_factory.register_node<Resampler<stream::IVoltage>>("Voltage_Resampler");
    m_node_factory.register_node<Resampler<stream::ILocation>>("Location_Resampler");
    m_node_factory.register_node<Resampler<stream::IDistance>>("Distance_Resampler");
    m_node_factory.register_node<Resampler<stream::IMagnetic_Field>>("Magnetic_Field_Resampler");
    m_node_factory.register_node<Resampler<stream::IPressure>>("Pressure_Resampler");
    m_node_factory.register_node<Resampler<stream::ITemperature>>("Temperature_Resampler");
    m_node_factory.register_node<Resampler<stream::IReference_Frame>>("Reference_Frame_Resampler");
    m_node_factory.register_node<Resampler<stream::IPWM_Value>>("PWM_Value_Resampler");
}

HAL::~HAL()
{
}

auto HAL::load_settings() -> bool
{
    TIMED_FUNCTION();

    q::data::File_Source fs(k_settings_path);
    if (!fs.is_open())
    {
        QLOGE("Failed to load '{}'", k_settings_path);
        return false;
    }

    auto data = q::data::read_whole_source_as_string<std::string>(fs);
    if (m_settings.Parse(data.c_str()).HasParseError())
    {
        QLOGE("Failed to load '{}': {}:{}", k_settings_path, m_settings.GetParseError(), m_settings.GetErrorOffset());
        return false;
    }

    return true;
}
void HAL::save_settings()
{
    TIMED_FUNCTION();

    if (!m_settings.IsObject())
    {
        m_settings.SetObject();
        get_settings(q::Path("hal/buses"));
        get_settings(q::Path("hal/nodes"));
    }

    auto copy = std::make_shared<rapidjson::Document>();
    jsonutil::clone_value(*copy, m_settings, copy->GetAllocator());

    silk::async([=]()
    {
        TIMED_FUNCTION();

        rapidjson::StringBuffer s;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(s);
        copy->Accept(writer);    // Accept() traverses the DOM and generates Handler events.
        q::data::File_Sink fs(k_settings_path);
        if (fs.is_open())
        {
            fs.write(reinterpret_cast<uint8_t const*>(s.GetString()), s.GetSize());
        }
        else
        {
            QLOGE("Cannot open '{}' to save settings.", k_settings_path);
        }
    });

    //autojsoncxx::to_pretty_json_file("sensors_pi.cfg", config);
}

auto HAL::get_settings(q::Path const& path) -> rapidjson::Value&
{
    auto v = jsonutil::get_or_add_value(m_settings, path, rapidjson::kObjectType, m_settings.GetAllocator());
    if (v == nullptr)
    {
        QLOGE("Wrong json type for settings '{}'. Settings will be lost!!!!", path);
        m_emptyValue.SetObject();
        return m_emptyValue;
    }
    return *v;
}

auto HAL::get_bus_factory()    -> Factory<node::bus::IBus>&
{
    return m_bus_factory;
}
auto HAL::get_node_factory()  -> Factory<node::INode>&
{
    return m_node_factory;
}
auto HAL::get_buses()    -> Registry<node::bus::IBus>&
{
    return m_buses;
}
auto HAL::get_nodes()  -> Registry<node::INode>&
{
    return m_nodes;
}
auto HAL::get_streams()  -> Registry<node::stream::IStream>&
{
    return m_streams;
}

template<class T>
void write_gnu_plot(std::string const& name, std::vector<T> const& samples)
{
    q::data::File_Sink fs((q::Path(name)));
    std::string header("#x y\n");
    fs.write((uint8_t const*)header.c_str(), header.size());

    for(size_t i = 0; i < samples.size(); i++)
    {
        auto l = q::util::format2<std::string>("{} {.8}\n", i, samples[i]);
        fs.write((uint8_t const*)l.c_str(), l.size());
    }
}

auto HAL::create_bus(
        std::string const& type,
        std::string const& name,
        rapidjson::Value const& init_params) -> node::bus::IBus_ptr
{
    if (m_buses.find_by_name<node::bus::IBus>(name))
    {
        QLOGE("Bus '{}' already exist", name);
        return node::bus::IBus_ptr();
    }
    auto node = m_bus_factory.create_node(type);
    if (node && node->init(init_params))
    {
        auto res = m_buses.add(name, node); //this has to succeed since we already tested for duplicate names
        QASSERT(res);
        return node;
    }
    return node::bus::IBus_ptr();
}
auto HAL::create_node(
        std::string const& type,
        std::string const& name,
        rapidjson::Value const& init_params) -> node::INode_ptr
{
    if (m_nodes.find_by_name<node::INode>(name))
    {
        QLOGE("Node '{}' already exist", name);
        return node::INode_ptr();
    }
    auto node = m_node_factory.create_node(type);
    if (node && node->init(init_params))
    {
        auto res = m_nodes.add(name, node); //this has to succeed since we already tested for duplicate names
        QASSERT(res);
        auto outputs = node->get_outputs();
        for (auto const& x: outputs)
        {
            std::string stream_name = q::util::format2<std::string>("{}/{}", name, x.name);
            if (!m_streams.add(stream_name, x.stream))
            {
                QLOGE("Cannot add stream '{}'", stream_name);
                return node::INode_ptr();
            }
        }
        return node;
    }
    return node::INode_ptr();
}

auto HAL::create_buses(rapidjson::Value& json) -> bool
{
    if (!json.IsObject())
    {
        QLOGE("Wrong json type: {}", json.GetType());
        return false;
    }
    auto it = json.MemberBegin();
    for (; it != json.MemberEnd(); ++it)
    {
        std::string type(it->name.GetString());
        auto* namej = jsonutil::find_value(it->value, std::string("name"));
        if (!namej || namej->GetType() != rapidjson::kStringType)
        {
            QLOGE("Node type {} is missing the name", type);
            return false;
        }
        std::string name(namej->GetString());
        auto* init_paramsj = jsonutil::find_value(it->value, std::string("init_params"));
        if (!init_paramsj)
        {
            QLOGE("Node {} of type {} is missing the init_params", name, type);
            return false;
        }
        auto node = create_bus(type, name, *init_paramsj);
        if (!node)
        {
            QLOGE("Failed to create node {} of type '{}'", name, type);
            return false;
        }
    }
    return true;
}

auto HAL::create_nodes(rapidjson::Value& json) -> bool
{
    if (!json.IsObject())
    {
        QLOGE("Wrong json type: {}", json.GetType());
        return false;
    }
    auto it = json.MemberBegin();
    for (; it != json.MemberEnd(); ++it)
    {
        std::string type(it->name.GetString());
        auto* namej = jsonutil::find_value(it->value, std::string("name"));
        if (!namej || namej->GetType() != rapidjson::kStringType)
        {
            QLOGE("Node type {} is missing the name", type);
            return false;
        }
        std::string name(namej->GetString());
        auto* init_paramsj = jsonutil::find_value(it->value, std::string("init_params"));
        auto* configj = jsonutil::find_value(it->value, std::string("config"));
        if (!init_paramsj || !configj)
        {
            QLOGE("Node {} of type {} is missing the {}", name, type, init_paramsj ? "config" : "init_params");
            return false;
        }
        auto node = create_node(type, name, *init_paramsj);
        if (!node)
        {
            QLOGE("Failed to create node {} of type '{}'", name, type);
            return false;
        }
        if (!node->set_config(*configj))
        {
            QLOGE("Failed to set config for node '{}'", name);
            return false;
        }
    }
    return true;
}


auto HAL::init() -> bool
{
    using namespace silk::node;

    get_streams().remove_all();
    get_nodes().remove_all();

    if (!load_settings())
    {
        return false;
    }

//    uint32_t rate = 1000;
//    std::vector<float> samples;

//    {
//        const size_t elements = 1000;
//        const float noise = 0.3f;
//        std::vector<std::pair<float, float>> freq =
//        {{
//             { 10.f, 1.f },
//             { 70.f, 1.f/7.f },
//             { 130.f, 1.f/5.f }
//         }};
//        samples.resize(elements);
//        std::uniform_real_distribution<float> distribution(-noise, noise); //Values between 0 and 2
//        std::mt19937 engine; // Mersenne twister MT19937
//        auto generator = std::bind(distribution, engine);
//        for (size_t i = 0; i < samples.size(); i++)
//        {
//            float a = float(i) * math::anglef::_2pi / float(rate);
//            float output = 0.f;
//            for (auto& f: freq)
//            {
//                output += math::sin(a * f.first) * f.second;
//            }
//            samples[i] = output + generator();
//        }
//        for (size_t i = 0; i < samples.size(); i++)
//        {
//            samples[i] = i < (rate / 2) ? 1 : 0;
//        }
//    }

//    write_gnu_plot("in.dat", samples);

//    std::vector<float> out_samples;
//    for (size_t f = 1; f < rate / 2; f++)
//    {
//        util::Butterworth<double> lpf;
//        lpf.setup(4, rate, 100);
//        lpf.reset(0);
//        double inputs = 0;
//        double outputs = 0;
//        for (size_t x = 0; x < rate * 10; x++)
//        {
//            double a = x <= rate * 2 ? double(x) * math::angled::_2pi / double(rate) : 0.0;
//            double v = math::sin(a * f);
//            inputs += math::sqrt(math::abs(v));
//            lpf.process(v);
//            outputs += math::sqrt(math::abs(v));
//        }
//        out_samples.push_back(outputs / inputs);
//    }
//    write_gnu_plot("out.dat", out_samples);


    if (!create_buses(get_settings(q::Path("hal/buses"))) ||
        !create_nodes(get_settings(q::Path("hal/nodes"))))
    {
        return false;
    }

//    m_bus_factory.create_all();

    return true;
}

void HAL::shutdown()
{
}


//static std::vector<double> s_samples;
//static std::vector<double> s_samples_lpf;

void HAL::process()
{
//    for (auto const& n: m_buses.get_all())
//    {
//        n->process();
//    }
    for (auto const& n: m_nodes.get_all())
    {
        n.node->process();
    }

//    auto* stream = get_streams().find_by_name<node::stream::ILocation>("gps0/stream");
//    auto* stream_lpf = get_streams().find_by_name<node::stream::ILocation>("gps0_resampler/stream");
//    for (auto& s: stream->get_samples())
//    {
//        QLOGI("{.8}", s.value.latitude);
//        s_samples.push_back(s.value.latitude);
//    }
//    for (auto& s: stream_lpf->get_samples())
//    {
//        QLOGI("\t\t{.8}", s.value.latitude);
//        s_samples_lpf.push_back(s.value.latitude);
//    }

//    if (s_samples.size() == 50)
//    {
//        write_gnu_plot("out.dat", s_samples);
//        write_gnu_plot("rsout.dat", s_samples_lpf);
//    }
}



}

