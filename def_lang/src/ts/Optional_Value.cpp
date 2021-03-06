#include "def_lang/impl/Optional_Value.h"
#include "def_lang/Value_Selector.h"
#include "def_lang/Serialization.h"
#include "def_lang/IDeclaration_Scope.h"
#include "def_lang/impl/Initializer_List.h"
#include "def_lang/Qualified_Type.h"

namespace ts
{

Optional_Value::Optional_Value(std::shared_ptr<IOptional_Type const> type)
    : m_type(type)
{
}

bool Optional_Value::is_constructed() const
{
    return m_is_constructed;
}

Result<bool> Optional_Value::is_equal(IValue const& other) const
{
    if (!is_constructed() || !other.is_constructed())
    {
        TS_ASSERT(false);
        return Error("Unconstructed value");
    }
    IOptional_Value const* v = dynamic_cast<const IOptional_Value*>(&other);
    if (!v)
    {
        return Error("incompatible values");
    }

    if (get_type() != v->get_type())
    {
        return Error("incompatible types");
    }

    if (is_set() != v->is_set())
    {
        return false;
    }

    if (!is_set())
    {
        return true;
    }

    return get_value()->is_equal(*v->get_value());
}

Result<void> Optional_Value::construct(IInitializer_List const& initializer_list)
{
    if (is_constructed())
    {
        TS_ASSERT(false);
        return Error("Already constructed value");
    }
    if (initializer_list.get_initializer_count() != 0)
    {
        return Error("Not supported");
    }

    m_is_constructed = true;

    return success;
}
Result<void> Optional_Value::copy_construct(IValue const& other)
{
    auto result = construct();
    if (result != success)
    {
        return result;
    }
    return copy_assign(other);
}

Result<void> Optional_Value::copy_assign(IValue const& other)
{
    if (!is_constructed() || !other.is_constructed())
    {
        TS_ASSERT(false);
        return Error("Unconstructed value");
    }
    IOptional_Value const* v = dynamic_cast<const IOptional_Value*>(&other);
    if (!v)
    {
        return Error("incompatible values");
    }

    if (get_type() != v->get_type())
    {
        return Error("incompatible types");
    }

    if (!v->is_set())
    {
        return mark_as_set(v->is_set());
    }

    return set_value(v->get_value());
}
Result<void> Optional_Value::copy_assign(IInitializer_List const& initializer_list)
{
    if (!is_constructed())
    {
        TS_ASSERT(false);
        return Error("Unconstructed value");
    }
    return Error("not implemented");
}

std::shared_ptr<IType const> Optional_Value::get_type() const
{
    return m_type;
}

Result<void> Optional_Value::parse_from_ui_string(std::string const& str)
{
    if (!is_constructed())
    {
        TS_ASSERT(false);
        return Error("Unconstructed value");
    }
    return Error("Not Supported");
}
Result<std::string> Optional_Value::get_ui_string() const
{
    if (!is_constructed())
    {
        TS_ASSERT(false);
        return Error("Unconstructed value");
    }
    if (is_set())
    {
        return m_value->get_ui_string();
    }
    return "<none>";
}

std::shared_ptr<const IValue> Optional_Value::select(Value_Selector&& selector) const
{
    if (!is_constructed())
    {
        TS_ASSERT(false);
        return nullptr;
    }
    return const_cast<Optional_Value*>(this)->select(std::move(selector));
}
std::shared_ptr<IValue> Optional_Value::select(Value_Selector&& selector)
{
    if (!is_constructed())
    {
        TS_ASSERT(false);
        return nullptr;
    }
    return nullptr;
}

std::shared_ptr<IOptional_Type const> Optional_Value::get_specialized_type() const
{
    return m_type;
}

Result<sz::Value> Optional_Value::serialize() const
{
    if (!is_constructed())
    {
        TS_ASSERT(false);
        return Error("Unconstructed value");
    }
    if (!is_set())
    {
        return sz::Value();
    }
    else
    {
        return get_value()->serialize();
    }
}

Result<void> Optional_Value::deserialize(sz::Value const& sz_value)
{
    if (!is_constructed())
    {
        TS_ASSERT(false);
        return Error("Unconstructed value");
    }
    if (sz_value.is_empty())
    {
        return mark_as_set(false);
    }

    auto result = mark_as_set(true);
    if (result != success)
    {
        return result;
    }

    return get_value()->deserialize(sz_value);
}

Result<void> Optional_Value::create_value()
{
    std::shared_ptr<IValue> value = m_type->get_inner_qualified_type()->get_type()->create_value();
    auto result = value->construct();
    if (result != success)
    {
        return result;
    }

    m_value = value;
    return success;
}

bool Optional_Value::is_set() const
{
    return m_is_set;
}

Result<void> Optional_Value::mark_as_set(bool set)
{
    if (m_is_set == set)
    {
        return ts::success;
    }

    if (!set)
    {
        sig_will_be_unset();
    }

    m_is_set = set;
    if (m_is_set && !m_value)
    {
        auto result = create_value();
        if (result != success)
        {
            return result;
        }
    }

    if (set)
    {
        sig_was_set();
    }

    return ts::success;
}


std::shared_ptr<const IValue> Optional_Value::get_value() const
{
    TS_ASSERT(is_constructed());
    return m_is_set ? m_value : std::shared_ptr<const IValue>();
}
std::shared_ptr<IValue> Optional_Value::get_value()
{
    if (get_specialized_type()->get_inner_qualified_type()->is_const())
    {
        return nullptr;
    }
    TS_ASSERT(is_constructed());
    return m_is_set ? m_value : std::shared_ptr<IValue>();
}

Result<void> Optional_Value::set_value(std::shared_ptr<const IValue> value)
{
    if (!value)
    {
        return Error("Cannot set null value in optional '" + m_type->get_symbol_path().to_string() + "'");
    }
    if (!is_constructed() || !value->is_constructed())
    {
        TS_ASSERT(false);
        return Error("Unconstructed value");
    }

    if (value->get_type() != m_type->get_inner_qualified_type()->get_type())
    {
        return Error("Type '" + value->get_type()->get_symbol_path().to_string() + "' not allowed in optional '" + m_type->get_symbol_path().to_string() + "'");
    }

    auto result = mark_as_set(true);
    if (result != success)
    {
        return result;
    }

    return m_value->copy_assign(*value);
}



}
