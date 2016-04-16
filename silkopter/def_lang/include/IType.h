#pragma once

#include "ISymbol.h"
#include "IAttribute_Container.h"
#include "ITemplate_Argument.h"

namespace ts
{

class IValue;

class IType : virtual public ISymbol, virtual public IAttribute_Container, virtual public ITemplate_Argument
{
public:
    virtual ~IType() = default;

    virtual auto clone(std::string const& name) const -> std::unique_ptr<IType> = 0;

    virtual auto create_value() const -> std::unique_ptr<IValue> = 0;
};

}