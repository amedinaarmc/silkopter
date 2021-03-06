#pragma once

#include "IValue_Editor.h"
#include "Qualified_Value.h"

class QCheckBox;

class Bool_Value_Editor : public IValue_Editor
{
public:
    Bool_Value_Editor(const Qualified_Value<ts::IBool_Value>& qualified_value);
    ~Bool_Value_Editor();

    QWidget*	get_widget() override;
    Qualified_Value<ts::IValue> get_qualified_value() override;
    void		refresh_editor() override;
    void		refresh_value() override;
    void		set_read_only_override(bool read_only) override;
    bool		is_read_only() const override;

private:
    void		refresh_read_only_state();
    void		set_value(bool value);

    Qualified_Value<ts::IBool_Value> m_qualified_value;
	QCheckBox* m_editor = nullptr;
    QMetaObject::Connection m_changed_connection;
    QMetaObject::Connection m_toggled_connection;

    bool m_read_only_override = false;
};

