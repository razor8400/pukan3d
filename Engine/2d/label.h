#pragma once

#include "core/game_object.h"

namespace engine
{
    class label : public texture_protocol
    {
    public:
        bool init(const std::string& font_name);
        bool init(const std::string& font_name, int font_size);
        bool init(const std::string& font_name, int font_size, const std::string& caption);
        
        void set_font(const std::string& font_name);
        
        const font_ttf_ptr& get_font() const { return m_font; }
        void set_font(const font_ttf_ptr& font) { m_font = font; update_texture(); }
        
        const std::string& get_caption() const { return m_caption; }
        void set_caption(const std::string& caption) { m_caption = caption; update_texture(); }

		int get_font_size() const { return m_font_size; }
        void set_font_size(int font_size) { m_font_size = font_size; update_texture(); }

		int get_max_line_width() const { return m_max_line_width; }
        void set_max_line_width(int max_line_width) { m_max_line_width = max_line_width; update_texture(); }

		vertical_text_alignment get_vertical_alignment() const { return m_vertical_alignment; }
        void set_vertical_alignment(vertical_text_alignment vertical_alignment) { m_vertical_alignment = vertical_alignment; update_texture(); }

		horisontal_text_alignment get_horisonal_alignment() const { return m_horisontal_alignment; }
        void set_horisonal_alignment(horisontal_text_alignment horisontal_alignment) { m_horisontal_alignment = horisontal_alignment; update_texture(); }
        
        const math::vector3d& get_outline_color() const { return m_outline_color; }
        void set_outline_color(const math::vector3d& color) { m_outline_color = color; update_texture(); }
        
        int get_outline_size() const { return m_outline_size; }
        void set_outline_size(int outline_size) { m_outline_size = outline_size; update_texture(); }
    private:
        void update_texture();
    private:
        std::string m_caption;
        math::vector3d m_outline_color;

        int m_font_size = 0;
        int m_outline_size = 0;
        int m_max_line_width = 0;
        
		vertical_text_alignment m_vertical_alignment = vertical_text_alignment::left;
		horisontal_text_alignment m_horisontal_alignment = horisontal_text_alignment::bottom;

		font_ttf_ptr m_font;
    };
}
