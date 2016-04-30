#pragma once

#include "def_lang/IVariant_Type.h"
#include "def_lang/IVariant_Value.h"

namespace ts
{

class Variant_Value final : virtual public IVariant_Value
{
public:

    Variant_Value(std::shared_ptr<IVariant_Type const> type);

    Result<bool> is_equal(IValue const& other) const override;

    Result<void> copy_assign(IValue const& other) override;
    Result<void> copy_assign(IInitializer const& initializer) override;

    std::shared_ptr<IValue> clone() const override;

    std::shared_ptr<IType const> get_type() const override;

    Result<void> parse_from_ui_string(std::string const& str) override;
    Result<std::string> get_ui_string() const override;

    std::shared_ptr<const IValue> select(Value_Selector&& selector) const override;
    std::shared_ptr<IValue> select(Value_Selector&& selector) override;

    Result<serialization::Value> serialize() const override;
    Result<void> deserialize(serialization::Value const&) override;

    std::shared_ptr<IVariant_Type const> get_specialized_type() const override;

    Result<void> set_value_type(std::shared_ptr<const IType> type) override;
    Result<void> set_value_type_index(size_t idx) override;
    size_t get_value_type_index() const override;

    Result<void> set_value(std::shared_ptr<const IValue> value) override;
    std::shared_ptr<const IValue> get_value() const override;
    std::shared_ptr<IValue> get_value() override;

private:
    std::shared_ptr<IVariant_Type const> m_type;
    std::shared_ptr<IValue> m_value;
};

}
