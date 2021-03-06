#include "Sim_Window.h"

#include "sz_math.hpp"
#include "sz_Multi_Simulator_Structs.hpp"

#include "Axis.h"

Sim_Window::Sim_Window(silk::HAL& hal, silk::node::gs::Node_ptr sim_node, silk::Comms& comms, Render_Context& context, QWidget *parent)
    : QMainWindow(parent)
    , m_hal(hal)
    , m_sim_node(sim_node)
    , m_comms(comms)
    , m_context(context)
    , m_camera_controller(context.camera)
{
    QASSERT(sim_node);

    m_context.camera.set_position(math::vec3f(5, -5, 3));

    setWindowTitle("Simulator");
    setMouseTracking(true);

    m_ui.setupUi(this);

    m_connections.push_back(sim_node->message_received_signal.connect(std::bind(&Sim_Window::sim_message_received, this, std::placeholders::_1)));

    //m_ui.action_simulation_on->setChecked(m_config.environment.is_simulation_enabled);
    QObject::connect(m_ui.action_simulation, &QAction::toggled, [this](bool v)
    {
        auto* vj = jsonutil::find_value(m_sim_node->config, std::string("Simulation Enabled"));
        if (vj)
        {
            vj->SetBool(v);
            m_hal.set_node_config(m_sim_node, m_sim_node->config);
        }
    });
    QObject::connect(m_ui.action_drag, &QAction::toggled, [this](bool v)
    {
        auto* vj = jsonutil::find_value(m_sim_node->config, std::string("Drag Enabled"));
        if (vj)
        {
            vj->SetBool(v);
            m_hal.set_node_config(m_sim_node, m_sim_node->config);
        }
    });
    QObject::connect(m_ui.action_stop_motion, &QAction::triggered, [this](bool)
    {
        rapidjson::Document message;
        jsonutil::add_value(message, std::string("message"), rapidjson::Value("stop motion"), message.GetAllocator());
        m_hal.send_node_message(m_sim_node, message);
    });
    QObject::connect(m_ui.action_reset, &QAction::triggered, [this](bool)
    {
        rapidjson::Document message;
        jsonutil::add_value(message, std::string("message"), rapidjson::Value("reset"), message.GetAllocator());
        m_hal.send_node_message(m_sim_node, message);
    });
    QObject::connect(m_ui.action_ground, &QAction::triggered, [this](bool v)
    {
        auto* vj = jsonutil::find_value(m_sim_node->config, std::string("Ground Enabled"));
        if (vj)
        {
            vj->SetBool(v);
            m_hal.set_node_config(m_sim_node, m_sim_node->config);
        }
    });
    QObject::connect(m_ui.action_gravity, &QAction::triggered, [this](bool v)
    {
        auto* vj = jsonutil::find_value(m_sim_node->config, std::string("Gravity Enabled"));
        if (vj)
        {
            vj->SetBool(v);
            m_hal.set_node_config(m_sim_node, m_sim_node->config);
        }
    });

    //    QActionGroup* ag = new QActionGroup(this);
    //    m_ui.action_move->setActionGroup(ag);
    //    m_ui.action_rotate->setActionGroup(ag);
    //    QObject::connect(m_ui.action_move, &QAction::triggered, [this](bool) { m_action = Action::MOVE; });
    //    QObject::connect(m_ui.action_rotate, &QAction::triggered, [this](bool) { m_action = Action::ROTATE; });

    //    QObject::connect(m_ui.action_test_ahrs, &QAction::triggered, [this](bool) { setup_test_ahrs(); });

    setMinimumHeight(200);

    //    m_render_widget = new Render_Widget(this);
    //    m_ui.centralWidget->layout()->addWidget(m_render_widget);

    auto s = new QShortcut(QKeySequence("CTRL+R"), this, 0, 0, Qt::WindowShortcut);
    connect(s, &QShortcut::activated, [&]()
    {
    });


    m_connections.push_back( sim_node->changed_signal.connect([this]() { read_config(); }) );
    read_config();

    //////////////////////////////////////////////////////////////////////////

    m_ui.render_widget->init();
}

Sim_Window::~Sim_Window()
{
    m_connections.clear();
}

void Sim_Window::read_config()
{
    auto* vj = jsonutil::find_value(m_sim_node->config, std::string("Simulation Enabled"));
    if (vj)
    {
        m_ui.action_simulation->setChecked(vj->GetBool());
    }
    vj = jsonutil::find_value(m_sim_node->config, std::string("Gravity Enabled"));
    if (vj)
    {
        m_ui.action_gravity->setChecked(vj->GetBool());
    }
    vj = jsonutil::find_value(m_sim_node->config, std::string("Ground Enabled"));
    if (vj)
    {
        m_ui.action_ground->setChecked(vj->GetBool());
    }
    vj = jsonutil::find_value(m_sim_node->config, std::string("Drag Enabled"));
    if (vj)
    {
        m_ui.action_drag->setChecked(vj->GetBool());
    }
}


void Sim_Window::render_ground()
{
    math::vec3s32 offset(m_uav.sim_state.enu_position);
    offset.z = 0;

    math::trans3df trans;
    trans.set_translation(math::vec3f(offset));
    m_context.painter.push_post_clip_transform(trans);

    auto mat = m_context.materials.primitive;
    mat.get_render_state(0).set_depth_test(true);
    mat.get_render_state(0).set_depth_write(false);
    mat.get_render_state(0).set_culling(false);
    mat.get_render_state(0).set_blend_formula(q::video::Render_State::Blend_Formula::Preset::ALPHA);
    m_context.painter.set_material(mat);

    const float k_size = 10000;
    const int k_line_count = 300;
    const int k_half_line_size = k_line_count / 2;

    //m_context.painter.fill_rectangle(q::draw::Vertex(math::vec3f(-k_size, -k_size, 0), 0x20FFFFFF), q::draw::Vertex(math::vec3f(k_size, k_size, 0), 0x20FFFFFF));

    float max_alpha = 20.f;
    float color_inc = 2.f / float(k_line_count);
    for (int i = 0; i < k_line_count; i++)
    {
        float x = float(i) - k_half_line_size;
        uint32_t color = 0x00FFFFFF;
        uint32_t color2 = color | static_cast<int>((1.f - math::abs(x) * color_inc) * max_alpha) << 24;

        m_context.painter.draw_line(q::draw::Vertex(math::vec3f(-k_half_line_size, x, 0), color), q::draw::Vertex(math::vec3f(0, x, 0), color2));
        m_context.painter.draw_line(q::draw::Vertex(math::vec3f(0, x, 0), color2), q::draw::Vertex(math::vec3f(k_half_line_size, x, 0), color));

        m_context.painter.draw_line(q::draw::Vertex(math::vec3f(x, -k_half_line_size, 0), color), q::draw::Vertex(math::vec3f(x, 0, 0), color2));
        m_context.painter.draw_line(q::draw::Vertex(math::vec3f(x, 0, 0), color2), q::draw::Vertex(math::vec3f(x, k_half_line_size, 0), color));
    }

    m_context.painter.pop_post_clip_transform();
}

void Sim_Window::render_uav(math::trans3df const& trans)
{
//    math::trans3df trans;
//    trans.set_rotation(m_uav.state.local_to_enu_rotation);
//    trans.set_translation(m_uav.state.enu_position);
    m_context.painter.push_post_clip_transform(trans);

    auto mat = m_context.materials.primitive;
    mat.get_render_state(0).set_depth_test(true);
    mat.get_render_state(0).set_depth_write(false);
    mat.get_render_state(0).set_culling(false);
    mat.get_render_state(0).set_blend_formula(q::video::Render_State::Blend_Formula::Preset::ALPHA);

    m_context.painter.set_material(mat);

    //render motors
    auto config = m_hal.get_multi_config();
    if (config && m_uav.sim_state.motors.size() == config->motors.size())
    {
        //float pitch = math::dot(m_local_to_world_mat.get_axis_y(), math::vec3f(0, 0, 1));
        //float roll = math::dot(m_local_to_world_mat.get_axis_x(), math::vec3f(0, 0, 1));
        //m_uav.get_motor_mixer().set_data(0.5f, 0, -pitch, -roll);

        //UAV radius
        m_context.painter.draw_circle(q::draw::Vertex(math::vec3f::zero, 0xFFFFFFFF), config->radius, 32); //motor

        const float propeller_radius = 0.12f;
        const float motor_radius = 0.02f;
        for (size_t i = 0; i < m_uav.sim_state.motors.size(); i++)
        {
            auto const& mc = config->motors[i];
            auto const& m = m_uav.sim_state.motors[i];

            m_context.painter.draw_line(q::draw::Vertex(math::vec3f::zero, 0xFFFFFFFF), q::draw::Vertex(mc.position, 0xFFFFFFFF));
            m_context.painter.fill_circle(q::draw::Vertex(mc.position, 0xFFFFFFFF), motor_radius, 16); //motor

            m_context.painter.fill_circle(q::draw::Vertex(mc.position, 0x40FFFFFF), propeller_radius, 32); //motor + prop
            float ratio = m.thrust / float(config->motor_thrust);
            m_context.painter.fill_circle(q::draw::Vertex(mc.position, 0xAA00FF00), math::lerp(0.f, propeller_radius, ratio), 32);
        }
    }

    //render axis
    if (m_ui.action_show_axis->isChecked())
    {
        render_axis(m_context.painter, 1.f, 0.2f);
    }

    m_context.painter.pop_post_clip_transform();

    //render altitude meter
    if (m_ui.action_show_altitude->isChecked())
    {
        math::vec3f p0 = m_uav.sim_state.enu_position;
        math::vec3f p1 = p0;
        p0.z = 0;
        math::vec3f p2 = p1 + math::vec3f(0, 0, math::sgn(p0.z)) * 2.f;
        m_context.painter.draw_line(q::draw::Vertex(p0, 0x50FFFFFF), q::draw::Vertex(p1, 0x50FFFFFF));
        m_context.painter.draw_line(q::draw::Vertex(p1, 0x50FFFFFF), q::draw::Vertex(p2, 0x00FFFFFF));
        m_context.painter.fill_circle(q::draw::Vertex(p0, 0x50FFFFFF), 0.05, 16);
        m_context.painter.fill_sphere(q::draw::Vertex(p1, 0x50FFFFFF), 0.05, 4);
    }
}

void Sim_Window::render_brain_state()
{
    auto mat = m_context.materials.primitive;
    mat.get_render_state(0).set_depth_test(true);
    mat.get_render_state(0).set_depth_write(false);
    mat.get_render_state(0).set_culling(false);
    mat.get_render_state(0).set_blend_formula(q::video::Render_State::Blend_Formula::Preset::ALPHA);

    m_context.painter.set_material(mat);

    auto home_lla_position = util::coordinates::ecef_to_lla(m_uav.brain_state.value.home_position.value);
    auto enu_to_ecef_trans = util::coordinates::enu_to_ecef_transform(home_lla_position);
    auto ecef_to_enu_trans = math::inverse(enu_to_ecef_trans);

    //auto lla_position = util::coordinates::ecef_to_lla(m_uav.brain_state.value.ecef_position.value);
    //QLOGI("LAT: {}, LON: {}, ALT: {}", lla_position.latitude, lla_position.longitude, lla_position.altitude);

    //render what the brain _thinks_ the orientation is
    {
        math::trans3df trans;
        trans.set_rotation(m_uav.brain_state.value.frame.value);
        trans.set_translation(m_uav.sim_state.enu_position);
        m_context.painter.push_post_clip_transform(trans);

        //render axis
        if (m_ui.action_show_axis->isChecked())
        {
            render_axis(m_context.painter, 1.f, 0.15f);
        }

        m_context.painter.pop_post_clip_transform();
    }

    //render where the brain _thinks_ it is
    {
        auto enu_position = math::vec3f(math::transform(ecef_to_enu_trans, m_uav.brain_state.value.position.value));

        math::trans3df trans;
        trans.set_rotation(m_uav.brain_state.value.frame.value);
        trans.set_translation(enu_position);
        m_context.painter.push_post_clip_transform(trans);

        //render axis
        if (m_ui.action_show_axis->isChecked())
        {
            render_axis(m_context.painter, 1.f, 0.15f);
        }

        m_context.painter.pop_post_clip_transform();

        m_context.painter.fill_sphere(q::draw::Vertex(enu_position, 0x50FFFFFF), 0.05, 4);
    }
}

void Sim_Window::render_world_axis()
{
    auto mat = m_context.materials.primitive;
    mat.get_render_state(0).set_depth_test(true);
    mat.get_render_state(0).set_depth_write(false);
    mat.get_render_state(0).set_culling(false);
    mat.get_render_state(0).set_blend_formula(q::video::Render_State::Blend_Formula::Preset::ALPHA);

    m_context.painter.set_material(mat);

    math::trans3df trans;
    m_context.painter.push_post_clip_transform(trans);

    q::scene::Camera camera;
    camera.set_parallel_zoom(4.f);
    camera.set_viewport(q::video::Viewport(math::vec2u32(0, 0), math::vec2u32(150, 150)));
    camera.set_near_distance(-100.0f);
    camera.set_far_distance( 100.0f);

    //ENU
    {
        math::quatf rot(m_context.camera.get_rotation());

        camera.set_transform(math::rotate(rot, math::vec3f(0, 5, 0)), rot);
        m_context.painter.set_camera(camera);
        render_axis(m_context.painter, 1.f, 1.5f);
    }

    //ECEF
    {
        auto home_lla_position = util::coordinates::ecef_to_lla(m_uav.brain_state.value.home_position.value);
        auto enu_to_ecef_rotation = util::coordinates::enu_to_ecef_rotation(home_lla_position);
        math::quatd rotd;
        rotd.set_from_mat3(enu_to_ecef_rotation);
        math::quatf rot(rotd);
        rot = rot * m_context.camera.get_rotation();

        camera.set_viewport(q::video::Viewport(math::vec2u32(0, 150), math::vec2u32(150, 150)));
        camera.set_transform(math::rotate(rot, math::vec3f(0, 5, 0)), rot);
        m_context.painter.set_camera(camera);
        render_axis(m_context.painter, 1.f, 1.5f);
    }

    m_context.painter.pop_post_clip_transform();
    m_context.painter.set_camera(m_context.camera);
}


void Sim_Window::process()
{
    if (!isVisible())
    {
        return;
    }

    auto now = q::Clock::now();

    //request state
    if (now - m_last_state_request_tp > std::chrono::milliseconds(33) && m_state_requests > 0)
    {
        m_state_requests--;
        rapidjson::Document message;
        jsonutil::add_value(message, std::string("message"), rapidjson::Value("get state"), message.GetAllocator());
        m_hal.send_node_message(m_sim_node, message);
    };


    m_ui.render_widget->begin_rendering();

    m_context.camera.set_viewport_and_aspect_ratio(q::video::Viewport(math::vec2u32::zero, math::vec2u32(m_ui.render_widget->width(), m_ui.render_widget->height())));

    auto color = math::color::u32_to_rgba<float>(0x2c3e50);
    q::System::inst().get_renderer()->get_render_target()->set_color_clear_value(color);
    q::System::inst().get_renderer()->get_render_target()->clear_all();

    {
        auto delta = m_uav.sim_state.enu_position - m_camera_position_target;
        delta *= 0.9f;
        m_camera_position_target += delta;
        m_context.camera.set_position(m_context.camera.get_position() + delta);
        m_camera_controller.set_focus_point(m_camera_position_target);
    }

    m_camera_controller.process();

    m_context.painter.set_camera(m_context.camera);

    //setup ENU space
    {
//        auto home_lla_position = util::coordinates::ecef_to_lla(m_uav.brain_state.value.ecef_home_position);
//        auto enu_to_ecef_rotation = util::coordinates::enu_to_ecef_rotation(home_lla_position);
//        auto ecef_to_enu_rotation = math::inverse(enu_to_ecef_rotation);
//        m_context.painter.push_post_clip_transform(math::trans3df(math::vec3f::zero, math::mat3f(ecef_to_enu_rotation), math::vec3f::one));
    }
//    m_context.painter.set_material(m_context.materials.primitive);
//    m_context.painter.fill_sphere(q::draw::Vertex(math::vec3f(0, 0, 0), 0xFF00FF), 0.1, 0);
    render_ground();

    {
        math::trans3df trans;
        trans.set_rotation(m_uav.sim_state.local_to_enu_rotation);
        trans.set_translation(m_uav.sim_state.enu_position);
        render_uav(trans);
    }
    {
        auto const& samples = m_comms.get_multi_state_samples();
        if (!samples.empty())
        {
            m_uav.brain_state = samples.back();
        }
        render_brain_state();
    }

    render_world_axis();

//    m_context.painter.pop_post_clip_transform();
    m_context.painter.flush();

    m_ui.render_widget->end_rendering();
    m_ui.render_widget->update();
}

/////////////////////////////////////////////////////////////////////////

void Sim_Window::mousePressEvent(QMouseEvent* event)
{
    QWidget::mousePressEvent(event);
    m_camera_controller.mouse_press_event(event);
}

//////////////////////////////////////////////////////////////////////////

void Sim_Window::mouseReleaseEvent(QMouseEvent* event)
{
    QWidget::mouseReleaseEvent(event);
    m_camera_controller.mouse_release_event(event);
}

//////////////////////////////////////////////////////////////////////////

void Sim_Window::mouseMoveEvent(QMouseEvent* event)
{
    QWidget::mouseMoveEvent(event);
    m_camera_controller.mouse_move_event(event);
}

//////////////////////////////////////////////////////////////////////////

void Sim_Window::wheelEvent(QWheelEvent* event)
{
    QWidget::wheelEvent(event);
    m_camera_controller.wheel_event(event);
}

//////////////////////////////////////////////////////////////////////////

void Sim_Window::sim_message_received(rapidjson::Document const& message)
{
    silk::node::IMulti_Simulator::UAV_State state;
    autojsoncxx::error::ErrorStack result;
    if (autojsoncxx::from_value(state, message, result))
    {
        m_uav.sim_state = state;
    }
    m_state_requests++;
}
