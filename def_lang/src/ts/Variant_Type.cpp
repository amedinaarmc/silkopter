#include "def_lang/impl/Variant_Type.h"
#include "def_lang/impl/Variant_Value.h"
#include "def_lang/IAttribute.h"
#include "def_lang/impl/UI_Name_Attribute.h"

namespace ts
{

Variant_Type::Variant_Type(std::string const& name)
    : Symbol_EP(name)
    , m_ui_name(name)
{
}

Variant_Type::Variant_Type(Variant_Type const& other, std::string const& name)
    : Symbol_EP(other, name)
    , Attribute_Container_EP(other)
    , m_inner_types(other.m_inner_types)
    , m_ui_name(name)
{
}


Result<void> Variant_Type::init(std::vector<std::shared_ptr<const ITemplate_Argument>> const& arguments)
{
    if (arguments.size() <= 1)
    {
        return Error("Expected more than one template argument, got " + std::to_string(arguments.size()));
    }

    for (std::shared_ptr<const ITemplate_Argument> argument: arguments)
    {
        std::shared_ptr<const IType> inner_type = std::dynamic_pointer_cast<const IType>(argument);
        if (!inner_type)
        {
            return Error("Invalid template argument. Expected type");
        }
        m_inner_types.push_back(inner_type);
    }

    return success;
}

std::string const& Variant_Type::get_ui_name() const
{
    return m_ui_name;
}

Result<void> Variant_Type::validate_attribute(IAttribute const& attribute)
{
    if (UI_Name_Attribute const* att = dynamic_cast<UI_Name_Attribute const*>(&attribute))
    {
        m_ui_name = att->get_ui_name();
        return success;
    }
    else
    {
        return Error("Attribute " + attribute.get_name() + " not supported");
    }
}

std::shared_ptr<IType> Variant_Type::clone(std::string const& name) const
{
    return std::make_shared<Variant_Type>(*this, name);
}

std::string Variant_Type::get_template_instantiation_string() const
{
    return get_symbol_path().to_string();
}

size_t Variant_Type::get_inner_type_count() const
{
    return m_inner_types.size();
}

std::shared_ptr<const IType> Variant_Type::get_inner_type(size_t idx) const
{
    return m_inner_types[idx];
}

boost::optional<size_t> Variant_Type::find_inner_type_idx(std::shared_ptr<const IType> type) const
{
    auto it = std::find_if(m_inner_types.begin(), m_inner_types.end(), [&](std::shared_ptr<const IType> const& t) { return t == type; });
    return it != m_inner_types.end() ? std::distance(m_inner_types.begin(), it) : boost::optional<size_t>();
}

std::shared_ptr<IValue> Variant_Type::create_value() const
{
    return create_specialized_value();
}

std::shared_ptr<Variant_Type::value_type> Variant_Type::create_specialized_value() const
{
    return std::make_shared<Variant_Value>(shared_from_this());
}


}