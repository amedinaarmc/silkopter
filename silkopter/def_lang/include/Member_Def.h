#pragma once

#include <memory>
#include "impl/Symbol_EP.h"
#include "impl/Attribute_Container_EP.h"
#include "attributes/UI_Name_Attribute.h"

namespace ts
{

class IType;
class IValue;

class Member_Def final : public Symbol_EP, public Attribute_Container_EP
{
public:

    Member_Def(std::string const& name, IType const& type, std::unique_ptr<const IValue> default_value);
    ~Member_Def();

    auto get_type() const -> IType const&;
    auto get_default_value() const -> IValue const&;

protected:
    auto validate_attribute(IAttribute const& attribute) -> Result<void>;

private:
    IType const& m_type;
    std::unique_ptr<const IValue> m_default_value;
    UI_Name_Attribute const* m_ui_name_attribute = nullptr;
};

}
