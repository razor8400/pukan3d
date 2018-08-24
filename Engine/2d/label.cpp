#include "common.h"
#include "resources/resources_manager.h"
#include "resources/font_ttf.h"

#include "renderer/render_command.h"
#include "renderer/renderer.h"

#include "label.h"

namespace engine
{   
    bool label::init(const std::string& font_name)
    {
        return init(font_name, 0);
    }
    
    bool label::init(const std::string& font_name, int font_size)
    {
        return init(font_name, font_size, empty_string);
    }
    
    bool label::init(const std::string& font_name, int font_size, const std::string& caption)
    {
        if (!game_object::init())
            return false;
        
        auto font = resources_manager::instance().load_resource_from_file<font_ttf>(font_name);
        
        if (!font)
            return false;
        
        m_font_size = font_size;
        m_caption = caption;
        m_shader_program = resources_manager::instance().load_resource_from_file<shader>(shaders::shader_font_position_color_alpha);

        set_font(font);

        return true;
    }
    
    void label::set_font(const std::string& font_name)
    {
        auto font = resources_manager::instance().load_resource_from_file<font_ttf>(font_name);
        set_font(font);
    }
    
    void label::update_texture()
    {
        clear_texture();
        
        m_vertices.clear();
        
        if (m_font)
        {
            auto size = m_font->text_size(m_caption, m_font_size + m_outline_size, m_max_line_width);
            m_size.x = std::max(size.x, m_size.x);
            m_size.y = std::max(size.y, m_size.y);
            m_texture = m_font->create_label(m_caption, m_font_size, math::vector2d(m_size.x, m_size.y), m_vertical_alignment, m_horisontal_alignment, &m_vertices);
        }
    }
}
