#include "calibration/Magnetic_Field_Calibration_Wizard.h"
#include <QPushButton>

#include "sz_math.hpp"
#include "sz_Calibration_Data.hpp"

#include "ui_Magnetic_Field_Calibration_Wizard_Reset.h"
#include "ui_Magnetic_Field_Calibration_Wizard_Instructions.h"
#include "ui_Magnetic_Field_Calibration_Wizard_Collect.h"
#include "ui_Magnetic_Field_Calibration_Wizard_Done.h"

constexpr std::chrono::seconds DATA_COLLECTION_DURATION(60);

Magnetic_Field_Calibration_Wizard::Magnetic_Field_Calibration_Wizard(silk::HAL& hal, silk::Comms& comms, silk::node::gs::Node_ptr node, size_t output_idx, QWidget* parent)
    : QDialog(parent)
    , m_hal(hal)
    , m_comms(comms)
    , m_node(node)
    , m_output(node->outputs[output_idx])
{
    m_stream = std::static_pointer_cast<silk::stream::gs::Magnetic_Field>(m_output.stream);

    m_hal.set_stream_telemetry_active(m_output.stream->name, true);

    m_initial_calibration = get_calibration_points();
    set_calibration_points(sz::calibration::Magnetic_Field_Points());

    m_step = Step::RESET;

    setLayout(new QVBoxLayout(this));
    layout()->setMargin(0);
    layout()->setSpacing(0);
    layout()->setContentsMargins(4, 4, 4, 4);

    prepare_step();
}

void Magnetic_Field_Calibration_Wizard::advance()
{
    if (m_step == Step::RESET)
    {
        m_step = Step::SHOW_INSTRUCTIONS;
    }
    else if (m_step == Step::SHOW_INSTRUCTIONS)
    {
        m_step = Step::COLLECT;
    }
    else if (m_step == Step::COLLECT)
    {
        m_connection.disconnect();
        m_step = Step::DONE;
    }

    prepare_step();
}

void Magnetic_Field_Calibration_Wizard::cancel()
{
    m_connection.disconnect();

    set_calibration_points(m_initial_calibration);

    close();
}

void Magnetic_Field_Calibration_Wizard::prepare_step()
{
    delete m_content;
    m_content = nullptr;

    if (m_step == Step::RESET)
    {
        m_content = new QWidget(this);
        layout()->addWidget(m_content);

        Ui::Magnetic_Field_Calibration_Wizard_Reset ui;
        ui.setupUi(m_content);
        if (m_initial_calibration.points.size() > 0)
        {
            ui.info->setText(q::util::format<std::string>("There are currently {} calibration points.\n"
                                                           "Do you want to clear these points or keep them?", m_initial_calibration.points.size()).c_str());
            auto* clear = ui.buttonBox->addButton("Clear", QDialogButtonBox::ResetRole);
            QObject::connect(clear, &QPushButton::released, [this]() { advance(); });

            auto* keep = ui.buttonBox->addButton("Keep", QDialogButtonBox::AcceptRole);
            QObject::connect(keep, &QPushButton::released, [this]() { m_crt_calibration = m_initial_calibration; advance(); });
        }
        else
        {
            ui.info->setText("There are no existing calibration data points.\nLet's add one");
            auto* ok = ui.buttonBox->addButton("Ok", QDialogButtonBox::AcceptRole);
            QObject::connect(ok, &QPushButton::released, [this]() { advance(); });
        }


        QObject::connect(ui.buttonBox, &QDialogButtonBox::rejected, [this]() { cancel(); });
    }
    else if (m_step == Step::SHOW_INSTRUCTIONS)
    {
        m_content = new QWidget(this);
        layout()->addWidget(m_content);

        Ui::Magnetic_Field_Calibration_Wizard_Instructions ui;
        ui.setupUi(m_content);
        ui.instructions->setText(q::util::format<std::string>(
                                    "Please rotate the UAV along all its axis for {} seconds.\n"
                                    "\n"
                                    "Ready?", DATA_COLLECTION_DURATION).c_str());

        QObject::connect(ui.buttonBox, &QDialogButtonBox::accepted, [this]() { advance(); });
        QObject::connect(ui.buttonBox, &QDialogButtonBox::rejected, [this]() { cancel(); });
    }
    else if (m_step == Step::COLLECT)
    {
        m_content = new QWidget(this);
        layout()->addWidget(m_content);

        Ui::Magnetic_Field_Calibration_Wizard_Collect ui;
        ui.setupUi(m_content);
        auto* info = ui.info;
        auto* progress = ui.progressBar;
        auto* bias = ui.bias;
        auto* scale = ui.scale;

        m_samples.clear();

        QObject::connect(ui.buttonBox, &QDialogButtonBox::rejected, [this]() { cancel(); });

        m_connection = m_stream->samples_available_signal.connect([this, info, progress, bias, scale](silk::stream::gs::Magnetic_Field::Samples const& samples)
        {
            on_samples_received(samples);
            info->setText(q::util::format<std::string>("Collected {} samples...", m_samples.size()).c_str());
            size_t needed_samples = std::chrono::seconds(DATA_COLLECTION_DURATION).count() * m_output.rate;
            progress->setValue(float(m_samples.size() * 100.f) / float(needed_samples));
            bias->setText(q::util::format<std::string>("{}", m_box.get_center()).c_str());

            auto extent = math::max(m_box.get_extent(), math::vec3f(1.f));
            float avg = (extent.x + extent.y + extent.z) / 3.f;

            scale->setText(q::util::format<std::string>("{}", avg / extent).c_str());

            if (m_samples.size() >= needed_samples)
            {
                advance();
            }
        });
    }
    else if (m_step == Step::DONE)
    {
        math::vec3f bias = m_box.get_center();

        auto extent = math::max(m_box.get_extent(), math::vec3f(1.f));
        float avg = (extent.x + extent.y + extent.z) / 3.f;
        math::vec3f scale = avg / extent;

        m_content = new QWidget(this);
        layout()->addWidget(m_content);

        Ui::Magnetic_Field_Calibration_Wizard_Done ui;
        ui.setupUi(m_content);

        ui.info->setText("Done!\n"
                         "The new Bias & Scale are:");
        ui.bias->setText(q::util::format<std::string>("{}", bias).c_str());
        ui.scale->setText(q::util::format<std::string>("{}", scale).c_str());

        sz::calibration::Magnetic_Field point;
        point.temperature = 0;
        point.bias = bias;
        point.scale = scale;
        m_crt_calibration.points.push_back(point);

        QObject::connect(ui.temperature, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double value)
        {
            m_crt_calibration.points.back().temperature = static_cast<float>(value);
        });

        QObject::connect(ui.buttonBox, &QDialogButtonBox::accepted, [this]()
        {
            set_calibration_points(m_crt_calibration);
            this->accept();
        });
        QObject::connect(ui.buttonBox, &QDialogButtonBox::rejected, [this]() { cancel(); });
    }
}

void Magnetic_Field_Calibration_Wizard::set_calibration_points(sz::calibration::Magnetic_Field_Points const& data)
{
    rapidjson::Document calibrationj;
    calibrationj.SetObject();
    autojsoncxx::to_document(data, calibrationj);

    q::Path path("Calibration");
    path += m_output.name;

    rapidjson::Document configj = jsonutil::clone_value(m_node->config);
    if (!jsonutil::remove_value(configj, path))
    {
        QASSERT(0);
        return;
    }

    if (!jsonutil::add_value(configj, path, std::move(calibrationj), configj.GetAllocator()))
    {
        QASSERT(0);
        return;
    }

    m_hal.set_node_config(m_node, configj);
}

auto Magnetic_Field_Calibration_Wizard::get_calibration_points() const -> sz::calibration::Magnetic_Field_Points
{
    q::Path path("Calibration");
    path += m_output.name;
    auto const* calibrationj = jsonutil::find_value(m_node->config, path);
    if (!calibrationj)
    {
        QASSERT(0);
        return sz::calibration::Magnetic_Field_Points();
    }
    sz::calibration::Magnetic_Field_Points calibration;
    autojsoncxx::error::ErrorStack result;
    if (!autojsoncxx::from_value(calibration, *calibrationj, result))
    {
        QASSERT(0);
        return sz::calibration::Magnetic_Field_Points();
    }
    return calibration;
}


void Magnetic_Field_Calibration_Wizard::on_samples_received(silk::stream::gs::Magnetic_Field::Samples const& samples)
{
    m_samples.reserve(m_samples.size() + samples.size());
    for (auto const& s: samples)
    {
        if (m_samples.empty())
        {
            m_box.reset(s.value, s.value);
        }
        else
        {
            m_box.add_internal_point(s.value);
        }
        m_samples.push_back(s.value);
    }
}

