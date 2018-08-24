#include "common.h"
#include "texture_protocol.h"

#include "resources/texture2d.h"

#include "renderer/render_command.h"
#include "renderer/renderer.h"

namespace engine
{
    void texture_protocol::set_alpha(float alpha)
    {
        set_opacity(int(alpha * 255));
    }
    
    float texture_protocol::get_alpha() const
    {
        float alpha = get_opacity() / 255.0f;
        
        for (auto parent = get_parent(); parent != nullptr; parent = parent->get_parent())
            alpha *= parent->get_opacity() / 255.0f;
        
        return alpha;
    }
    
    void texture_protocol::render(renderer* r, const math::mat4& t)
    {
        auto texture = get_texture();
        
        if (texture)
        {
            auto command = quads_command::create(texture->get_texture_id(), m_blend_func, m_shader_program);
            auto color = get_color_rgba();
            
            for (auto v : m_vertices)
            {
                v.vertice = math::transform_point(v.vertice, t);
                v.color = color;
                command->add_vertice(v);
            }
            
            r->add_command(command);
        }
        
#if DEBUG_DRAW
        auto command = custom_render_command::create([=](const math::mat4& world)
        {
            if (m_shader_program)
                m_shader_program->use(world * t);
            
            gl::draw_rect(0, 0, m_size.x, m_size.y);
        });
        
        r->add_post_draw_command(command);
#endif
    }
    
    void texture_protocol::clear_texture()
    {
        m_texture.reset();
    }
}
