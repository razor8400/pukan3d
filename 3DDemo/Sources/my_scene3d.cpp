#include "engine.h"
#include "my_scene3d.h"

using namespace engine;

static const float scroll_speed = 0.25f;

void my_scene3d::on_enter()
{
    scene::on_enter();
    
    auto obj = game_object::create<game_object>();
    
    obj->set_size(math::vector3d(0.5f, 0.5f, 0.5f));
    
    add_child(obj);
    
    m_listener = ref::create<touch_listener>();
    m_listener->swallow_touches = true;
    m_listener->touch_began = std::bind(&my_scene3d::on_touch_began, this, std::placeholders::_1);
    m_listener->touch_moved = std::bind(&my_scene3d::on_touch_moved, this, std::placeholders::_1);
    m_listener->touch_ended = std::bind(&my_scene3d::on_touch_ended, this, std::placeholders::_1);
    
    touch_dispatcher::instance().add_touch_listener(m_listener);
}

void my_scene3d::on_exit()
{
    scene::on_exit();
    
    touch_dispatcher::instance().remove_touch_listener(m_listener);
}

bool my_scene3d::on_touch_began(const math::vector2d& location)
{
	m_touch_began = true;
	m_mouse = location;

    return true;
}

void my_scene3d::on_touch_moved(const math::vector2d& location)
{
    if (m_touch_began)
    {
        auto delta = location - m_mouse;
		
		m_vertical_scroll += delta.x * scroll_speed;
		m_horisontal_scroll -= delta.y * scroll_speed;
    }
    
    m_mouse = location;
}

void my_scene3d::on_touch_ended(const math::vector2d& location)
{
	m_touch_began = false;
}

void my_scene3d::draw(engine::renderer* r)
{
    auto command = custom_render_command::create([=](const math::mat4& world)
    {
        auto program = resources_manager::instance().load_resource_from_file<shader>(shaders::shader_position_color);
        
        if (program)
            program->use(world * get_transform());
        
        for (auto x = 1; x < m_size.x; ++x)
            gl::draw_line(x, 0, 0, x, 0, m_size.z, math::vector3d(0.0f, 0.7f, 0.0f));
        
        for (auto z = 1; z < m_size.z; ++z)
            gl::draw_line(0, 0, z, m_size.x, 0, z, math::vector3d(0.0f, 0.7f, 0.0f));
    });
    
    r->add_command(command);
    
    scene::draw(r);
}

void my_scene3d::update(float dt)
{
    scene::update(dt);
    
    auto camera = get_camera();
    auto position = camera->get_position();
    auto target = camera->get_target();
        
	position += target * m_horisontal_scroll * dt;
	position += math::vector3d::cross(target, math::vector3d::up) * m_vertical_scroll * dt;
	
	camera->set_position(position);

	m_vertical_scroll *= 0.9f;
	m_horisontal_scroll *= 0.9f;
}
