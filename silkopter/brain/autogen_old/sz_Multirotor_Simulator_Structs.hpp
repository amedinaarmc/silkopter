#pragma once

// The MIT License (MIT)
//
// Copyright (c) 2014 Siyuan Ren (netheril96@gmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <autojsoncxx/autojsoncxx.hpp>

// The comments are reserved for replacement
// such syntax is chosen so that the template file looks like valid C++

//struct Motor_State;

namespace autojsoncxx {

template <>
class SAXEventHandler< ::silk::node::IMultirotor_Simulator::Motor_State > {
private:
    utility::scoped_ptr<error::ErrorBase> the_error;
    int state;
    int depth;

    SAXEventHandler< float > handler_0;
SAXEventHandler< float > handler_1;
SAXEventHandler< float > handler_2;bool has_drag_factor;
bool has_throttle;
bool has_thrust;

    bool check_depth(const char* type)
    {
        if (depth <= 0) {
            the_error.reset(new error::TypeMismatchError("object", type));
            return false;
        }
        return true;
    }

    const char* current_member_name() const
    {
        switch (state) {
            case 0:
    return "drag_factor";
case 1:
    return "throttle";
case 2:
    return "thrust";
        default:
            break;
        }
        return "<UNKNOWN>";
    }

    bool checked_event_forwarding(bool success)
    {
        if (!success)
            the_error.reset(new error::ObjectMemberError(current_member_name()));
        return success;
    }

    void set_missing_required(const char* name)
    {
        if (the_error.empty() || the_error->type() != error::MISSING_REQUIRED)
            the_error.reset(new error::RequiredFieldMissingError());

        std::vector<std::string>& missing =
            static_cast<error::RequiredFieldMissingError*>(the_error.get())->missing_members();

        missing.push_back(name);
    }

    void reset_flags()
    {
        has_drag_factor = false;
has_throttle = false;
has_thrust = false;
    }

public:
    explicit SAXEventHandler( ::silk::node::IMultirotor_Simulator::Motor_State * obj)
        : state(-1)
        , depth(0)
        , handler_0(&obj->drag_factor)
, handler_1(&obj->throttle)
, handler_2(&obj->thrust)
    {
        reset_flags();
    }

    bool Null()
    {
        if (!check_depth("null"))
            return false;

        switch (state) {

        case 0:
    return checked_event_forwarding(handler_0.Null());

case 1:
    return checked_event_forwarding(handler_1.Null());

case 2:
    return checked_event_forwarding(handler_2.Null());

        default:
            break;
        }
        return true;
    }

    bool Bool(bool b)
    {
        if (!check_depth("bool"))
            return false;

        switch (state) {

        case 0:
    return checked_event_forwarding(handler_0.Bool(b));

case 1:
    return checked_event_forwarding(handler_1.Bool(b));

case 2:
    return checked_event_forwarding(handler_2.Bool(b));

        default:
            break;
        }
        return true;
    }

    bool Int(int i)
    {
        if (!check_depth("int"))
            return false;

        switch (state) {

        case 0:
    return checked_event_forwarding(handler_0.Int(i));

case 1:
    return checked_event_forwarding(handler_1.Int(i));

case 2:
    return checked_event_forwarding(handler_2.Int(i));

        default:
            break;
        }
        return true;
    }

    bool Uint(unsigned i)
    {
        if (!check_depth("unsigned"))
            return false;

        switch (state) {

        case 0:
    return checked_event_forwarding(handler_0.Uint(i));

case 1:
    return checked_event_forwarding(handler_1.Uint(i));

case 2:
    return checked_event_forwarding(handler_2.Uint(i));

        default:
            break;
        }
        return true;
    }

    bool Int64(utility::int64_t i)
    {
        if (!check_depth("int64_t"))
            return false;

        switch (state) {

        case 0:
    return checked_event_forwarding(handler_0.Int64(i));

case 1:
    return checked_event_forwarding(handler_1.Int64(i));

case 2:
    return checked_event_forwarding(handler_2.Int64(i));

        default:
            break;
        }
        return true;
    }

    bool Uint64(utility::uint64_t i)
    {
        if (!check_depth("uint64_t"))
            return false;

        switch (state) {

        case 0:
    return checked_event_forwarding(handler_0.Uint64(i));

case 1:
    return checked_event_forwarding(handler_1.Uint64(i));

case 2:
    return checked_event_forwarding(handler_2.Uint64(i));

        default:
            break;
        }
        return true;
    }

    bool Double(double d)
    {
        if (!check_depth("double"))
            return false;

        switch (state) {

        case 0:
    return checked_event_forwarding(handler_0.Double(d));

case 1:
    return checked_event_forwarding(handler_1.Double(d));

case 2:
    return checked_event_forwarding(handler_2.Double(d));

        default:
            break;
        }
        return true;
    }

    bool String(const char* str, SizeType length, bool copy)
    {
        if (!check_depth("string"))
            return false;

        switch (state) {

        case 0:
    return checked_event_forwarding(handler_0.String(str, length, copy));

case 1:
    return checked_event_forwarding(handler_1.String(str, length, copy));

case 2:
    return checked_event_forwarding(handler_2.String(str, length, copy));

        default:
            break;
        }
        return true;
    }

    bool Key(const char* str, SizeType length, bool copy)
    {
        if (!check_depth("object"))
            return false;

        if (depth == 1) {
            if (0) {
            }
            else if (utility::string_equal(str, length, "\x44\x72\x61\x67\x20\x46\x61\x63\x74\x6f\x72\x20\x28\x30\x2d\x31\x29", 17))
						 { state=0; has_drag_factor = true; }
else if (utility::string_equal(str, length, "\x54\x68\x72\x6f\x74\x74\x6c\x65\x20\x28\x30\x2d\x31\x29", 14))
						 { state=1; has_throttle = true; }
else if (utility::string_equal(str, length, "\x54\x68\x72\x75\x73\x74\x20\x28\x4e\x29", 10))
						 { state=2; has_thrust = true; }
            else {
                state = -1;
                return true;
            }

        } else {
            switch (state) {

            case 0:
    return checked_event_forwarding(handler_0.Key(str, length, copy));

case 1:
    return checked_event_forwarding(handler_1.Key(str, length, copy));

case 2:
    return checked_event_forwarding(handler_2.Key(str, length, copy));

            default:
                break;
            }
        }
        return true;
    }

    bool StartArray()
    {
        if (!check_depth("array"))
            return false;

        switch (state) {

        case 0:
    return checked_event_forwarding(handler_0.StartArray());

case 1:
    return checked_event_forwarding(handler_1.StartArray());

case 2:
    return checked_event_forwarding(handler_2.StartArray());

        default:
            break;
        }
        return true;
    }

    bool EndArray(SizeType length)
    {
        if (!check_depth("array"))
            return false;

        switch (state) {

        case 0:
    return checked_event_forwarding(handler_0.EndArray(length));

case 1:
    return checked_event_forwarding(handler_1.EndArray(length));

case 2:
    return checked_event_forwarding(handler_2.EndArray(length));

        default:
            break;
        }
        return true;
    }

    bool StartObject()
    {
        ++depth;
        if (depth > 1) {

            switch (state) {

            case 0:
    return checked_event_forwarding(handler_0.StartObject());

case 1:
    return checked_event_forwarding(handler_1.StartObject());

case 2:
    return checked_event_forwarding(handler_2.StartObject());

            default:
                break;
            }
        }
        return true;
    }

    bool EndObject(SizeType length)
    {
        --depth;
        if (depth > 0) {

            switch (state) {

            case 0:
    return checked_event_forwarding(handler_0.EndObject(length));

case 1:
    return checked_event_forwarding(handler_1.EndObject(length));

case 2:
    return checked_event_forwarding(handler_2.EndObject(length));

            default:
                break;
            }
        } else {
            if (!has_drag_factor) set_missing_required("drag_factor");
if (!has_throttle) set_missing_required("throttle");
if (!has_thrust) set_missing_required("thrust");
        }
        return the_error.empty();
    }

    bool HasError() const
    {
        return !this->the_error.empty();
    }

    bool ReapError(error::ErrorStack& errs)
    {
        if (this->the_error.empty())
            return false;

        errs.push(this->the_error.release());

        switch (state) {

        case 0:
     handler_0.ReapError(errs); break;
case 1:
     handler_1.ReapError(errs); break;
case 2:
     handler_2.ReapError(errs); break;

        default:
            break;
        }

        return true;
    }

    void PrepareForReuse()
    {
        depth = 0;
        state = -1;
        the_error.reset();
        reset_flags();
        handler_0.PrepareForReuse();
handler_1.PrepareForReuse();
handler_2.PrepareForReuse();

    }
};

template < class Writer48ef64f309e4664f822b5435670f9c64113597b4b791a03f24708659675ba0e4 >
struct Serializer< Writer48ef64f309e4664f822b5435670f9c64113597b4b791a03f24708659675ba0e4, ::silk::node::IMultirotor_Simulator::Motor_State > {

    void operator()( Writer48ef64f309e4664f822b5435670f9c64113597b4b791a03f24708659675ba0e4& w, const ::silk::node::IMultirotor_Simulator::Motor_State& value) const
    {
        w.StartObject();

        w.Key("\x44\x72\x61\x67\x20\x46\x61\x63\x74\x6f\x72\x20\x28\x30\x2d\x31\x29", 17, false); Serializer< Writer48ef64f309e4664f822b5435670f9c64113597b4b791a03f24708659675ba0e4, float >()(w, value.drag_factor);
w.Key("\x54\x68\x72\x6f\x74\x74\x6c\x65\x20\x28\x30\x2d\x31\x29", 14, false); Serializer< Writer48ef64f309e4664f822b5435670f9c64113597b4b791a03f24708659675ba0e4, float >()(w, value.throttle);
w.Key("\x54\x68\x72\x75\x73\x74\x20\x28\x4e\x29", 10, false); Serializer< Writer48ef64f309e4664f822b5435670f9c64113597b4b791a03f24708659675ba0e4, float >()(w, value.thrust);

        w.EndObject(3);
    }

};
}


// The MIT License (MIT)
//
// Copyright (c) 2014 Siyuan Ren (netheril96@gmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <autojsoncxx/autojsoncxx.hpp>

// The comments are reserved for replacement
// such syntax is chosen so that the template file looks like valid C++

//struct UAV_State;

namespace autojsoncxx {

template <>
class SAXEventHandler< ::silk::node::IMultirotor_Simulator::UAV_State > {
private:
    utility::scoped_ptr<error::ErrorBase> the_error;
    int state;
    int depth;

    SAXEventHandler< math::vec3f > handler_0;
SAXEventHandler< math::vec3f > handler_1;
SAXEventHandler< math::vec3f > handler_2;
SAXEventHandler< math::vec3f > handler_3;
SAXEventHandler< math::quatf > handler_4;
SAXEventHandler< math::vec3f > handler_5;
SAXEventHandler< std::vector<silk::node::IMultirotor_Simulator::Motor_State> > handler_6;bool has_enu_position;
bool has_enu_velocity;
bool has_enu_linear_acceleration;
bool has_acceleration;
bool has_local_to_enu_rotation;
bool has_angular_velocity;
bool has_motors;

    bool check_depth(const char* type)
    {
        if (depth <= 0) {
            the_error.reset(new error::TypeMismatchError("object", type));
            return false;
        }
        return true;
    }

    const char* current_member_name() const
    {
        switch (state) {
            case 0:
    return "enu_position";
case 1:
    return "enu_velocity";
case 2:
    return "enu_linear_acceleration";
case 3:
    return "acceleration";
case 4:
    return "local_to_enu_rotation";
case 5:
    return "angular_velocity";
case 6:
    return "motors";
        default:
            break;
        }
        return "<UNKNOWN>";
    }

    bool checked_event_forwarding(bool success)
    {
        if (!success)
            the_error.reset(new error::ObjectMemberError(current_member_name()));
        return success;
    }

    void set_missing_required(const char* name)
    {
        if (the_error.empty() || the_error->type() != error::MISSING_REQUIRED)
            the_error.reset(new error::RequiredFieldMissingError());

        std::vector<std::string>& missing =
            static_cast<error::RequiredFieldMissingError*>(the_error.get())->missing_members();

        missing.push_back(name);
    }

    void reset_flags()
    {
        has_enu_position = false;
has_enu_velocity = false;
has_enu_linear_acceleration = false;
has_acceleration = false;
has_local_to_enu_rotation = false;
has_angular_velocity = false;
has_motors = false;
    }

public:
    explicit SAXEventHandler( ::silk::node::IMultirotor_Simulator::UAV_State * obj)
        : state(-1)
        , depth(0)
        , handler_0(&obj->enu_position)
, handler_1(&obj->enu_velocity)
, handler_2(&obj->enu_linear_acceleration)
, handler_3(&obj->acceleration)
, handler_4(&obj->local_to_enu_rotation)
, handler_5(&obj->angular_velocity)
, handler_6(&obj->motors)
    {
        reset_flags();
    }

    bool Null()
    {
        if (!check_depth("null"))
            return false;

        switch (state) {

        case 0:
    return checked_event_forwarding(handler_0.Null());

case 1:
    return checked_event_forwarding(handler_1.Null());

case 2:
    return checked_event_forwarding(handler_2.Null());

case 3:
    return checked_event_forwarding(handler_3.Null());

case 4:
    return checked_event_forwarding(handler_4.Null());

case 5:
    return checked_event_forwarding(handler_5.Null());

case 6:
    return checked_event_forwarding(handler_6.Null());

        default:
            break;
        }
        return true;
    }

    bool Bool(bool b)
    {
        if (!check_depth("bool"))
            return false;

        switch (state) {

        case 0:
    return checked_event_forwarding(handler_0.Bool(b));

case 1:
    return checked_event_forwarding(handler_1.Bool(b));

case 2:
    return checked_event_forwarding(handler_2.Bool(b));

case 3:
    return checked_event_forwarding(handler_3.Bool(b));

case 4:
    return checked_event_forwarding(handler_4.Bool(b));

case 5:
    return checked_event_forwarding(handler_5.Bool(b));

case 6:
    return checked_event_forwarding(handler_6.Bool(b));

        default:
            break;
        }
        return true;
    }

    bool Int(int i)
    {
        if (!check_depth("int"))
            return false;

        switch (state) {

        case 0:
    return checked_event_forwarding(handler_0.Int(i));

case 1:
    return checked_event_forwarding(handler_1.Int(i));

case 2:
    return checked_event_forwarding(handler_2.Int(i));

case 3:
    return checked_event_forwarding(handler_3.Int(i));

case 4:
    return checked_event_forwarding(handler_4.Int(i));

case 5:
    return checked_event_forwarding(handler_5.Int(i));

case 6:
    return checked_event_forwarding(handler_6.Int(i));

        default:
            break;
        }
        return true;
    }

    bool Uint(unsigned i)
    {
        if (!check_depth("unsigned"))
            return false;

        switch (state) {

        case 0:
    return checked_event_forwarding(handler_0.Uint(i));

case 1:
    return checked_event_forwarding(handler_1.Uint(i));

case 2:
    return checked_event_forwarding(handler_2.Uint(i));

case 3:
    return checked_event_forwarding(handler_3.Uint(i));

case 4:
    return checked_event_forwarding(handler_4.Uint(i));

case 5:
    return checked_event_forwarding(handler_5.Uint(i));

case 6:
    return checked_event_forwarding(handler_6.Uint(i));

        default:
            break;
        }
        return true;
    }

    bool Int64(utility::int64_t i)
    {
        if (!check_depth("int64_t"))
            return false;

        switch (state) {

        case 0:
    return checked_event_forwarding(handler_0.Int64(i));

case 1:
    return checked_event_forwarding(handler_1.Int64(i));

case 2:
    return checked_event_forwarding(handler_2.Int64(i));

case 3:
    return checked_event_forwarding(handler_3.Int64(i));

case 4:
    return checked_event_forwarding(handler_4.Int64(i));

case 5:
    return checked_event_forwarding(handler_5.Int64(i));

case 6:
    return checked_event_forwarding(handler_6.Int64(i));

        default:
            break;
        }
        return true;
    }

    bool Uint64(utility::uint64_t i)
    {
        if (!check_depth("uint64_t"))
            return false;

        switch (state) {

        case 0:
    return checked_event_forwarding(handler_0.Uint64(i));

case 1:
    return checked_event_forwarding(handler_1.Uint64(i));

case 2:
    return checked_event_forwarding(handler_2.Uint64(i));

case 3:
    return checked_event_forwarding(handler_3.Uint64(i));

case 4:
    return checked_event_forwarding(handler_4.Uint64(i));

case 5:
    return checked_event_forwarding(handler_5.Uint64(i));

case 6:
    return checked_event_forwarding(handler_6.Uint64(i));

        default:
            break;
        }
        return true;
    }

    bool Double(double d)
    {
        if (!check_depth("double"))
            return false;

        switch (state) {

        case 0:
    return checked_event_forwarding(handler_0.Double(d));

case 1:
    return checked_event_forwarding(handler_1.Double(d));

case 2:
    return checked_event_forwarding(handler_2.Double(d));

case 3:
    return checked_event_forwarding(handler_3.Double(d));

case 4:
    return checked_event_forwarding(handler_4.Double(d));

case 5:
    return checked_event_forwarding(handler_5.Double(d));

case 6:
    return checked_event_forwarding(handler_6.Double(d));

        default:
            break;
        }
        return true;
    }

    bool String(const char* str, SizeType length, bool copy)
    {
        if (!check_depth("string"))
            return false;

        switch (state) {

        case 0:
    return checked_event_forwarding(handler_0.String(str, length, copy));

case 1:
    return checked_event_forwarding(handler_1.String(str, length, copy));

case 2:
    return checked_event_forwarding(handler_2.String(str, length, copy));

case 3:
    return checked_event_forwarding(handler_3.String(str, length, copy));

case 4:
    return checked_event_forwarding(handler_4.String(str, length, copy));

case 5:
    return checked_event_forwarding(handler_5.String(str, length, copy));

case 6:
    return checked_event_forwarding(handler_6.String(str, length, copy));

        default:
            break;
        }
        return true;
    }

    bool Key(const char* str, SizeType length, bool copy)
    {
        if (!check_depth("object"))
            return false;

        if (depth == 1) {
            if (0) {
            }
            else if (utility::string_equal(str, length, "\x50\x6f\x73\x69\x74\x69\x6f\x6e\x20\x28\x45\x4e\x55\x29", 14))
						 { state=0; has_enu_position = true; }
else if (utility::string_equal(str, length, "\x56\x65\x6c\x6f\x63\x69\x74\x79\x20\x28\x45\x4e\x55\x29", 14))
						 { state=1; has_enu_velocity = true; }
else if (utility::string_equal(str, length, "\x4c\x69\x6e\x65\x61\x72\x20\x41\x63\x63\x65\x6c\x65\x72\x61\x74\x69\x6f\x6e\x20\x28\x45\x4e\x55\x29", 25))
						 { state=2; has_enu_linear_acceleration = true; }
else if (utility::string_equal(str, length, "\x41\x63\x63\x65\x6c\x65\x72\x61\x74\x69\x6f\x6e", 12))
						 { state=3; has_acceleration = true; }
else if (utility::string_equal(str, length, "\x52\x6f\x74\x61\x74\x69\x6f\x6e\x20\x28\x4c\x6f\x63\x61\x6c\x2d\x3e\x45\x4e\x55\x29", 21))
						 { state=4; has_local_to_enu_rotation = true; }
else if (utility::string_equal(str, length, "\x41\x6e\x67\x75\x6c\x61\x72\x20\x56\x65\x6c\x6f\x63\x69\x74\x79", 16))
						 { state=5; has_angular_velocity = true; }
else if (utility::string_equal(str, length, "\x4d\x6f\x74\x6f\x72\x73", 6))
						 { state=6; has_motors = true; }
            else {
                state = -1;
                return true;
            }

        } else {
            switch (state) {

            case 0:
    return checked_event_forwarding(handler_0.Key(str, length, copy));

case 1:
    return checked_event_forwarding(handler_1.Key(str, length, copy));

case 2:
    return checked_event_forwarding(handler_2.Key(str, length, copy));

case 3:
    return checked_event_forwarding(handler_3.Key(str, length, copy));

case 4:
    return checked_event_forwarding(handler_4.Key(str, length, copy));

case 5:
    return checked_event_forwarding(handler_5.Key(str, length, copy));

case 6:
    return checked_event_forwarding(handler_6.Key(str, length, copy));

            default:
                break;
            }
        }
        return true;
    }

    bool StartArray()
    {
        if (!check_depth("array"))
            return false;

        switch (state) {

        case 0:
    return checked_event_forwarding(handler_0.StartArray());

case 1:
    return checked_event_forwarding(handler_1.StartArray());

case 2:
    return checked_event_forwarding(handler_2.StartArray());

case 3:
    return checked_event_forwarding(handler_3.StartArray());

case 4:
    return checked_event_forwarding(handler_4.StartArray());

case 5:
    return checked_event_forwarding(handler_5.StartArray());

case 6:
    return checked_event_forwarding(handler_6.StartArray());

        default:
            break;
        }
        return true;
    }

    bool EndArray(SizeType length)
    {
        if (!check_depth("array"))
            return false;

        switch (state) {

        case 0:
    return checked_event_forwarding(handler_0.EndArray(length));

case 1:
    return checked_event_forwarding(handler_1.EndArray(length));

case 2:
    return checked_event_forwarding(handler_2.EndArray(length));

case 3:
    return checked_event_forwarding(handler_3.EndArray(length));

case 4:
    return checked_event_forwarding(handler_4.EndArray(length));

case 5:
    return checked_event_forwarding(handler_5.EndArray(length));

case 6:
    return checked_event_forwarding(handler_6.EndArray(length));

        default:
            break;
        }
        return true;
    }

    bool StartObject()
    {
        ++depth;
        if (depth > 1) {

            switch (state) {

            case 0:
    return checked_event_forwarding(handler_0.StartObject());

case 1:
    return checked_event_forwarding(handler_1.StartObject());

case 2:
    return checked_event_forwarding(handler_2.StartObject());

case 3:
    return checked_event_forwarding(handler_3.StartObject());

case 4:
    return checked_event_forwarding(handler_4.StartObject());

case 5:
    return checked_event_forwarding(handler_5.StartObject());

case 6:
    return checked_event_forwarding(handler_6.StartObject());

            default:
                break;
            }
        }
        return true;
    }

    bool EndObject(SizeType length)
    {
        --depth;
        if (depth > 0) {

            switch (state) {

            case 0:
    return checked_event_forwarding(handler_0.EndObject(length));

case 1:
    return checked_event_forwarding(handler_1.EndObject(length));

case 2:
    return checked_event_forwarding(handler_2.EndObject(length));

case 3:
    return checked_event_forwarding(handler_3.EndObject(length));

case 4:
    return checked_event_forwarding(handler_4.EndObject(length));

case 5:
    return checked_event_forwarding(handler_5.EndObject(length));

case 6:
    return checked_event_forwarding(handler_6.EndObject(length));

            default:
                break;
            }
        } else {
            if (!has_enu_position) set_missing_required("enu_position");
if (!has_enu_velocity) set_missing_required("enu_velocity");
if (!has_enu_linear_acceleration) set_missing_required("enu_linear_acceleration");
if (!has_acceleration) set_missing_required("acceleration");
if (!has_local_to_enu_rotation) set_missing_required("local_to_enu_rotation");
if (!has_angular_velocity) set_missing_required("angular_velocity");
if (!has_motors) set_missing_required("motors");
        }
        return the_error.empty();
    }

    bool HasError() const
    {
        return !this->the_error.empty();
    }

    bool ReapError(error::ErrorStack& errs)
    {
        if (this->the_error.empty())
            return false;

        errs.push(this->the_error.release());

        switch (state) {

        case 0:
     handler_0.ReapError(errs); break;
case 1:
     handler_1.ReapError(errs); break;
case 2:
     handler_2.ReapError(errs); break;
case 3:
     handler_3.ReapError(errs); break;
case 4:
     handler_4.ReapError(errs); break;
case 5:
     handler_5.ReapError(errs); break;
case 6:
     handler_6.ReapError(errs); break;

        default:
            break;
        }

        return true;
    }

    void PrepareForReuse()
    {
        depth = 0;
        state = -1;
        the_error.reset();
        reset_flags();
        handler_0.PrepareForReuse();
handler_1.PrepareForReuse();
handler_2.PrepareForReuse();
handler_3.PrepareForReuse();
handler_4.PrepareForReuse();
handler_5.PrepareForReuse();
handler_6.PrepareForReuse();

    }
};

template < class Writer4ad500b245e3f66a02b421a1ee0fa1cdea0a7053f591af449203839be529e6b2 >
struct Serializer< Writer4ad500b245e3f66a02b421a1ee0fa1cdea0a7053f591af449203839be529e6b2, ::silk::node::IMultirotor_Simulator::UAV_State > {

    void operator()( Writer4ad500b245e3f66a02b421a1ee0fa1cdea0a7053f591af449203839be529e6b2& w, const ::silk::node::IMultirotor_Simulator::UAV_State& value) const
    {
        w.StartObject();

        w.Key("\x50\x6f\x73\x69\x74\x69\x6f\x6e\x20\x28\x45\x4e\x55\x29", 14, false); Serializer< Writer4ad500b245e3f66a02b421a1ee0fa1cdea0a7053f591af449203839be529e6b2, math::vec3f >()(w, value.enu_position);
w.Key("\x56\x65\x6c\x6f\x63\x69\x74\x79\x20\x28\x45\x4e\x55\x29", 14, false); Serializer< Writer4ad500b245e3f66a02b421a1ee0fa1cdea0a7053f591af449203839be529e6b2, math::vec3f >()(w, value.enu_velocity);
w.Key("\x4c\x69\x6e\x65\x61\x72\x20\x41\x63\x63\x65\x6c\x65\x72\x61\x74\x69\x6f\x6e\x20\x28\x45\x4e\x55\x29", 25, false); Serializer< Writer4ad500b245e3f66a02b421a1ee0fa1cdea0a7053f591af449203839be529e6b2, math::vec3f >()(w, value.enu_linear_acceleration);
w.Key("\x41\x63\x63\x65\x6c\x65\x72\x61\x74\x69\x6f\x6e", 12, false); Serializer< Writer4ad500b245e3f66a02b421a1ee0fa1cdea0a7053f591af449203839be529e6b2, math::vec3f >()(w, value.acceleration);
w.Key("\x52\x6f\x74\x61\x74\x69\x6f\x6e\x20\x28\x4c\x6f\x63\x61\x6c\x2d\x3e\x45\x4e\x55\x29", 21, false); Serializer< Writer4ad500b245e3f66a02b421a1ee0fa1cdea0a7053f591af449203839be529e6b2, math::quatf >()(w, value.local_to_enu_rotation);
w.Key("\x41\x6e\x67\x75\x6c\x61\x72\x20\x56\x65\x6c\x6f\x63\x69\x74\x79", 16, false); Serializer< Writer4ad500b245e3f66a02b421a1ee0fa1cdea0a7053f591af449203839be529e6b2, math::vec3f >()(w, value.angular_velocity);
w.Key("\x4d\x6f\x74\x6f\x72\x73", 6, false); Serializer< Writer4ad500b245e3f66a02b421a1ee0fa1cdea0a7053f591af449203839be529e6b2, std::vector<silk::node::IMultirotor_Simulator::Motor_State> >()(w, value.motors);

        w.EndObject(7);
    }

};
}


