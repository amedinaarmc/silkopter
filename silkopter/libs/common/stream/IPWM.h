#pragma once

#include "Stream_Base.h"

namespace silk
{
namespace stream
{

class IPWM : public IScalar_Stream<Type::PWM>
{
public:
    typedef std::true_type can_be_filtered_t;

    typedef float                   Value; //0 .. 1 representing duty cycle
    typedef stream::Sample<Value>     Sample;
    virtual auto get_samples() const -> std::vector<Sample> const& = 0;
};
DECLARE_CLASS_PTR(IPWM);


}
}