#pragma once

#include "Stream_Base.h"
#include "IFrame.h"

namespace silk
{
namespace stream
{


template<class T> struct Input_Value
{
    typedef T Value;

    Input_Value() = default;
    Input_Value(T value) : value(value) {}

    void set(Value const& v) { if (value != v) { value = v; version++; } }
    void set_unversioned(Value const& v) { value = v; }
    T const& get() const { return value; }

//    bool operator==(Input_Value const& other) = delete;//{ return value == other.value; }
//    bool operator!=(Input_Value const& other) = delete;//{ return !operator==(other); }
//    Input_Value const& operator=(Input_Value const& other)  = delete;//{ value = other.value; return *this; }

    uint8_t version = 0;
private:
    T value = T();
};
template<> struct Input_Value<bool>
{
    typedef bool Value;

    Input_Value() : version(0), value(0) {}
    Input_Value(bool value) : version(0), value(value ? 1 : 0) {}

    void set(Value const& v) { if (value != v) { value = v; version++; } }
    void set_unversioned(Value const& v) { value = v; }
    bool get() const { return value != 0; }

//    bool operator==(Input_Value const& other) = delete;// { return value == other.value; }
//    bool operator!=(Input_Value const& other) = delete;// { return !operator==(other); }
//    Input_Value const& operator=(Input_Value const& other) = delete;// { value = other.value; return *this; }

    uint8_t version : 7;
private:
    uint8_t value : 1;
};


class IMultirotor_Commands : public IScalar_Stream<Semantic::MULTIROTOR_COMMANDS>
{
public:
    typedef std::false_type can_be_filtered_t;

    struct Toggles
    {
        Input_Value<bool> take_off;
        Input_Value<bool> land;
        Input_Value<bool> return_home;
    };

    enum class Mode : uint8_t
    {
        IDLE,
        ARMED,
    };

    struct Vertical
    {
        Vertical() = default;

        enum class Mode : uint8_t
        {
            THRUST_RATE,
            THRUST,
            ALTITUDE,
        };
        Input_Value<Mode> mode = Mode::THRUST;

        Input_Value<float> thrust_rate; //Newtons per second
        Input_Value<float> thrust;      //Newtons
        Input_Value<float> altitude;    //meters
    };

    struct Horizontal
    {
        Horizontal() = default;

        enum class Mode : uint8_t
        {
            ANGLE_RATE,
            ANGLE,
            POSITION,
        };
        Input_Value<Mode> mode = Mode::ANGLE;

        Input_Value<math::vec2f> angle_rate;   //angle rate of change - radians per second
        Input_Value<math::vec2f> angle;        //angle from horizontal. zero means horizontal
        Input_Value<math::vec2f> position;     //meters
    };

    struct Yaw
    {
        enum class Mode : uint8_t
        {
            ANGLE_RATE,
            ANGLE,
        };
        Input_Value<Mode> mode = Mode::ANGLE_RATE;

        Input_Value<float> angle_rate;  //radians per second
        Input_Value<float> angle;       //radians
    };

    struct Gimbal
    {
        enum class Reference_Frame : uint8_t
        {
            GIMBAL,
            UAV,
        };
        Input_Value<Reference_Frame> reference_frame = Reference_Frame::UAV;
        Input_Value<math::quatf> target_frame;
    };

    struct Camera
    {
//        Input_Value<float> shutter_speed; //ms
    };

    //the reference frame for the user controls
    enum class Reference_Frame : uint8_t
    {
        LOCAL, 	//normal mode - back means back of uav
        USER,	//simple mode - back means towards the user, front away from her.
    };

    struct Helpers
    {
        Input_Value<bool> stay_in_range; //avoid out of range situations.
        Input_Value<bool> stay_in_battery_range; //avoid going too far considering current battery.
        Input_Value<bool> stay_in_perimeter; //stay in a configured perimeter.
        Input_Value<bool> avoid_altitude_drop; //avoid dropping too much altitude too fast.
        Input_Value<bool> avoid_the_user; //avoid getting too close to the launch position (the user).
        Input_Value<bool> avoid_proximity; //maintains a min distance from all objects around.
    };

    ///////////////////////////////
    /// Data

    struct Value
    {
        Toggles toggles;
        Vertical vertical;
        Horizontal horizontal;
        Yaw yaw;
        Gimbal gimbal;

        Input_Value<Mode> mode = Mode::IDLE;
        Input_Value<Reference_Frame> reference_frame = Reference_Frame::LOCAL;
        Helpers assists;
    };

    typedef stream::Sample<Value>     Sample;

    virtual ~IMultirotor_Commands() = default;

    virtual auto get_samples() const -> std::vector<Sample> const& = 0;


    //helper function that applies a functor on every member of the value
    template<class F, class... V>
    static bool apply(F& functor, V&&... values)
    {
        return     functor("toggles.land", values.toggles.land...)
                && functor("toggles.return_home", values.toggles.return_home...)
                && functor("toggles.take_off", values.toggles.take_off...)
                && functor("vertical.mode", values.vertical.mode...)
                && functor("vertical.thrust_rate", values.vertical.thrust_rate...)
                && functor("vertical.thrust", values.vertical.thrust...)
                && functor("vertical.altitude", values.vertical.altitude...)
                && functor("horizontal.mode", values.horizontal.mode...)
                && functor("horizontal.angle_rate", values.horizontal.angle_rate...)
                && functor("horizontal.angle", values.horizontal.angle...)
                && functor("horizontal.position", values.horizontal.position...)
                && functor("yaw.mode", values.yaw.mode...)
                && functor("yaw.angle_rate", values.yaw.angle_rate...)
                && functor("yaw.angle", values.yaw.angle...)
                && functor("gimbal.reference_frame", values.gimbal.reference_frame...)
                && functor("gimbal.target_frame", values.gimbal.target_frame...)
                && functor("mode", values.mode...)
                && functor("reference_frame", values.reference_frame...)
                && functor("assists.stay_in_battery_range", values.assists.stay_in_battery_range...)
                && functor("assists.stay_in_perimeter", values.assists.stay_in_perimeter...)
                && functor("assists.stay_in_range", values.assists.stay_in_range...)
                && functor("assists.avoid_altitude_drop", values.assists.avoid_altitude_drop...)
                && functor("assists.avoid_proximity", values.assists.avoid_proximity...)
                && functor("assists.avoid_the_user", values.assists.avoid_the_user...);
    }

    struct Equality_Functor
    {
        template<class T> bool operator()(T const& v1, T const& v2) { return v1.value == v2.value; }
    };
    struct Equality_Version_Functor
    {
        template<class T> bool operator()(T const& v1, T const& v2) { return v1.value == v2.value && v1.version == v2.version; }
    };
    struct Assignment_Functor
    {
        template<class T> bool operator()(T& v1, T const& v2) { v1.value = v2.value; return true; }
    };
    struct Assignment_Version_Functor
    {
        template<class T> bool operator()(T& v1, T const& v2) { v1.value = v2.value; v1.version = v2.version; return true; }
    };
};
DECLARE_CLASS_PTR(IMultirotor_Commands);


}
}


namespace util
{
namespace serialization
{

namespace detail
{
struct Serializer
{
    Serializer(Buffer_t& buffer, size_t& off) : m_buffer(buffer), m_off(off) {}
    Buffer_t& m_buffer;
    size_t& m_off;

    template<class T>
    bool operator()(const char*, T const& v)
    {
        serialize(m_buffer, v.get(), m_off);
        serialize(m_buffer, v.version, m_off);
        return true;
    }
    bool operator()(const char*, silk::stream::Input_Value<bool> const& v)
    {
        uint8_t x = v.version & 0x7F;
        x |= v.get() << 7;
        serialize(m_buffer, x, m_off);
        return true;
    }
};
struct Deserializer
{
    Deserializer(Buffer_t const& buffer, size_t& off) : m_buffer(buffer), m_off(off) {}
    Buffer_t const& m_buffer;
    size_t& m_off;

    template<class T>
    bool operator()(const char*, T& v)
    {
        typename T::Value x;
        bool res = deserialize(m_buffer, x, m_off) && deserialize(m_buffer, v.version, m_off);
        if (res)
        {
            v.set_unversioned(x);
        }
        return res;
    }
    bool operator()(const char*, silk::stream::Input_Value<bool>& v)
    {
        uint8_t x = 0;
        if (!deserialize(m_buffer, x, m_off))
        {
            return false;
        }
        v.version = x & 0x7F;
        v.set_unversioned(x >> 7);
        return true;
    }
};
}

template<> inline void serialize(Buffer_t& buffer, silk::stream::IMultirotor_Commands::Value const& value, size_t& off)
{
    detail::Serializer serializer(buffer, off);
    silk::stream::IMultirotor_Commands::apply(serializer, value);
}

template<> inline auto deserialize(Buffer_t const& buffer, silk::stream::IMultirotor_Commands::Value& value, size_t& off) -> bool
{
    detail::Deserializer deserializer(buffer, off);
    return silk::stream::IMultirotor_Commands::apply(deserializer, value);
}


}
}
