#include "common.h"

#include "components/component.h"

#include "renderer/render_command.h"
#include "renderer/camera.h"
#include "renderer/renderer.h"

#include "director.h"
#include "scene.h"

#include "game_object.h"

namespace engine
{
    IMPLEMENT_TYPE_INFO(game_object)
    
    game_object::~game_object()
    {

    }
    
	bool game_object::init()
	{
		m_scale = math::vector3d(1.0f, 1.0f, 1.0f);
		m_anchor = math::vector3d(0.5f, 0.5f, 0.5f);
        m_shader_program = resources_manager::instance().load_resource_from_file<shader>(shaders::shader_position_color);
        
		return true;
	}

	void game_object::update(float dt)
	{
        if (!m_enabled)
            return;
        
		if (m_update_transform)
		{
			m_update_transform = false;
			m_transform = parent_transform();
		}
        
        m_components.lock([=]()
        {
            for (auto& component : m_components)
                component->update(dt);
        });
        
        m_children.lock([=]()
        {
            for (auto& obj : m_children)
                obj->update(dt);
        });
	}

	void game_object::draw(renderer* r, const math::mat4& t)
	{
        if (!m_enabled)
            return;
        
        auto model_view_transform = transform(t);
    
        for (auto& obj : m_children)
            obj->draw(r, model_view_transform);
        
        render(r, model_view_transform);
    }
    
    void game_object::render(renderer* r, const math::mat4& t)
    {
#if DEBUG_DRAW
		auto command = custom_render_command::create([=](const math::mat4& world)
		{
            if (m_shader_program)
                m_shader_program->use(world * t);
            
            std::vector<gl::v3f_c3f> vertices;
            
            vertices.push_back(gl::v3f_c3f{ { 0, 0, 0 }, { 1, 0, 0 } });
            vertices.push_back(gl::v3f_c3f{ { m_size.x, 0.0f, 0.0f }, { 1, 0, 0 } });
            vertices.push_back(gl::v3f_c3f{ { m_size.x, m_size.y, 0.0f }, { 1, 0, 0 } });
            vertices.push_back(gl::v3f_c3f{ { 0.0f, m_size.y, 0.0f }, { 1, 0, 0 } });
            
            vertices.push_back(gl::v3f_c3f{ { 0.0f, 0.0f, m_size.z }, { 1, 0, 0 } });
            vertices.push_back(gl::v3f_c3f{ { 0.0f, m_size.y, m_size.z }, { 1, 0, 0 } });
            
            vertices.push_back(gl::v3f_c3f{ { m_size.x, 0.0f, m_size.z }, { 1, 0, 0 } });
            vertices.push_back(gl::v3f_c3f{ { m_size.x, m_size.y, m_size.z }, { 1, 0, 0 } });
            
            std::vector<unsigned short> indices = {
                0, 1, 2, 2, 3, 0,
                0, 4, 5, 5, 3, 0,
                4, 6, 7, 7, 5, 4,
                1, 6, 7, 7, 2, 1
            };

            gl::draw_vertices(vertices, indices);
		});
        r->add_post_draw_command(command);
#endif
    }
	
    void game_object::on_enter()
    {
		m_components.lock([=]()
		{
			for (auto& component : m_components)
				component->start();
		});

		m_children.lock([=]()
		{
			for (auto& obj : m_children)
				obj->on_enter();
		});
        
        m_active = true;

		mark_dirty();
    }
    
    void game_object::on_exit()
    {
		m_components.lock([=]()
		{
			for (auto& component : m_components)
				component->stop();
		});

		m_children.lock([=]()
		{
			for (auto& obj : m_children)
				obj->on_exit();
		});
        
        m_active = false;
    }
    
	void game_object::add_child(game_object* obj)
	{
        assert(obj && obj != this);

        if (obj->m_parent)
            obj->remove_from_parent();
        
        if (m_active)
            obj->on_enter();
        
		obj->m_parent = this;
        
		m_children.push_back(obj);
	}

	void game_object::remove_child(game_object* obj)
	{
        assert(obj && obj != this);

        m_children.erase(obj);

        if (m_active)
            obj->on_exit();
        
        obj->m_parent = nullptr;
	}

	void game_object::remove_from_parent()
	{
		if (m_parent)
			m_parent->remove_child(this);
	}
    
    void game_object::add_component(component* component)
    {
        assert(component);

        component->m_parent = this;
        
        if (m_active)
            component->start();
        
        m_components.push_back(component);
    }
    
    void game_object::remove_component(component* component)
    {
        assert(component);

        if (m_active)
            component->stop();
        
        component->m_parent = nullptr;
        m_components.erase(component);
    }
    
    math::vector3d game_object::global_to_local(const math::vector3d& v3) const
    {
        auto transform = math::mat4::inverse(world_transform());
        return math::transform_point(v3, transform);
    }
    
    math::vector3d game_object::local_to_global(const math::vector3d& v3) const
    {
        return math::transform_point(v3, world_transform());
    }

	math::mat4 game_object::transform(const math::mat4& parent) const
    {
        return parent * m_transform;
    }

	math::mat4 game_object::parent_transform() const
	{
        auto anchor = anchor_point();
		auto position = math::mat4::translate(m_position.x + anchor.x, m_position.y + anchor.y, m_position.z + anchor.z);
		auto rotation = math::mat4::rotate(math::vector3d::right, m_rotation.x)
			* math::mat4::rotate(math::vector3d::up, m_rotation.y)
			* math::mat4::rotate(math::vector3d(0, 0, 1), m_rotation.z);

		auto scale = math::mat4::scale(m_scale.x, m_scale.y, m_scale.z);
        
        return position * rotation * scale * math::mat4::translate(-anchor.x, -anchor.y, -anchor.z);
    }

	math::mat4 game_object::world_transform() const
	{
		auto tr = m_transform;

		for (auto obj = m_parent; obj != nullptr; obj = obj->m_parent)
			tr *= obj->m_transform;

		return tr;
	}
}
