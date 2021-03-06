#include "def_lang/impl/Optional_Type.h"
#include "def_lang/impl/Optional_Value.h"
#include "def_lang/IAttribute.h"
#include "def_lang/impl/UI_Name_Attribute.h"
#include "def_lang/impl/Native_Type_Attribute.h"
#include "def_lang/Qualified_Type.h"

namespace ts
{

Optional_Type::Optional_Type(std::string const& name)
    : Symbol_EP(name)
    , m_ui_name(name)
{
}

Optional_Type::Optional_Type(Optional_Type const& other, std::string const& name)
    : Symbol_EP(other, name)
    , Attribute_Container_EP(other)
    , m_inner_qualified_type(other.m_inner_qualified_type)
    , m_ui_name(name)
    , m_native_type(other.m_ui_name)
{
}


Result<void> Optional_Type::init(std::vector<std::shared_ptr<const ITemplate_Argument>> const& arguments)
{
    if (arguments.size() != 1)
    {
        return Error("Expected one template argument, got " + std::to_string(arguments.size()));
    }

    std::shared_ptr<const Qualified_Type> qtype = std::dynamic_pointer_cast<const Qualified_Type>(arguments[0]);
    if (!qtype)
    {
        return Error("Invalid template argument. Expected type");
    }
    m_inner_qualified_type = qtype;

    return success;
}

std::string const& Optional_Type::get_ui_name() const
{
    return m_ui_name;
}
Symbol_Path Optional_Type::get_native_type() const
{
    return m_native_type;
}

Result<void> Optional_Type::validate_attribute(IAttribute const& attribute)
{
    if (UI_Name_Attribute const* att = dynamic_cast<UI_Name_Attribute const*>(&attribute))
    {
        m_ui_name = att->get_ui_name();
        return success;
    }
    else if (Native_Type_Attribute const* att = dynamic_cast<Native_Type_Attribute const*>(&attribute))
    {
        m_native_type = att->get_native_type();
        return success;
    }
    else
    {
        return Error("Attribute " + attribute.get_name() + " not supported");
    }
}

std::shared_ptr<IType> Optional_Type::clone(std::string const& name) const
{
    return std::make_shared<Optional_Type>(*this, name);
}
std::shared_ptr<IType> Optional_Type::alias(std::string const& name) const
{
    std::shared_ptr<Optional_Type> alias = std::make_shared<Optional_Type>(*this, name);
    alias->m_native_type = Symbol_Path(); //clear the native type as this is an alias
    alias->m_aliased_type = this->shared_from_this();
    return alias;
}
std::shared_ptr<const IType> Optional_Type::get_aliased_type() const
{
    return m_aliased_type;
}

std::shared_ptr<const Qualified_Type> Optional_Type::get_inner_qualified_type() const
{
    return m_inner_qualified_type;
}

std::shared_ptr<IValue> Optional_Type::create_value() const
{
    return create_specialized_value();
}

std::shared_ptr<Optional_Type::value_type> Optional_Type::create_specialized_value() const
{
    return std::make_shared<Optional_Value>(shared_from_this());
}

}
