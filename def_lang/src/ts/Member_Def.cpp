#include "def_lang/impl/Member_Def.h"
#include "def_lang/IValue.h"
#include "def_lang/impl/UI_Name_Attribute.h"
#include "def_lang/impl/UI_Suffix_Attribute.h"

namespace ts
{

Member_Def::Member_Def(std::string const& name, std::shared_ptr<IType const> type, std::shared_ptr<const IValue> default_value)
    : Symbol_EP(name)
    , m_type(type)
    , m_default_value(std::move(default_value))
{
    TS_ASSERT(m_default_value);
    TS_ASSERT(m_default_value->is_constructed());
}

Member_Def::~Member_Def()
{

}

std::shared_ptr<IType const> Member_Def::get_type() const
{
    return m_type;
}
std::shared_ptr<const IValue> Member_Def::get_default_value() const
{
    return m_default_value;
}

std::string const& Member_Def::get_ui_name() const
{
    return m_ui_name.empty() ? get_name() : m_ui_name;
}

std::string const& Member_Def::get_ui_suffix() const
{
    return m_ui_suffix;
}

Result<void> Member_Def::validate_attribute(IAttribute const& attribute)
{
    if (UI_Name_Attribute const* att = dynamic_cast<UI_Name_Attribute const*>(&attribute))
    {
        m_ui_name = att->get_ui_name();
    }
    else if (UI_Suffix_Attribute const* att = dynamic_cast<UI_Suffix_Attribute const*>(&attribute))
    {
        m_ui_suffix = att->get_ui_suffix();
    }
    else
    {
        return Error("Attribute " + attribute.get_name() + " not supported");
    }

    return success;
}

}
