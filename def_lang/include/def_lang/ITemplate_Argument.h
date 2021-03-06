#pragma once

#include <string>

namespace ts
{

class ITemplate_Argument
{
public:
    virtual ~ITemplate_Argument() = default;

    virtual std::string get_template_instantiation_string() const = 0;
};

}
