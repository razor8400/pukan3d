#include "common.h"

#include "utils/file_utils.h"
#include "texture2d.h"
#include "font_ttf.h"

namespace engine
{
    static const int max_atlas = 1024;
    
    font_ttf::font_ttf(const std::string& font_name) : m_font_name(font_name)
    {
        
    }
    
    font_ttf::~font_ttf()
    {
        font_utils::unload_font(m_font_name);
    }
    
	std::shared_ptr<font_ttf> font_ttf::load_from_file(const std::string& file_name)
	{
		logger() << "[font_ttf] load:" << file_name;

        auto font_name = file_utils::get_file_name(file_name);
        
        if (font_utils::load_font(file_name, font_name))
            return std::make_shared<font_ttf>(font_name);
        
		return std::shared_ptr<font_ttf>();
	}
    
    bool font_ttf::load(const unsigned char* data, size_t size)
    {
        return font_utils::load_font(data, size, m_font_name);
    }
    
    texture2d_ptr font_ttf::create_label(const std::string& text, int size, const math::vector2d& bounds,
                                         vertical_text_alignment vertical_alignment,
                                         horisontal_text_alignment horisontal_alignment,
                                         std::vector<gl::v3f_c4f_t2f>* vertices) const
    {
        if (text.empty())
            return texture2d_ptr();
        
        auto& atlas = get_atlas(text, size);
        auto strings = wrap_text(text, atlas.glyphs, (int)bounds.x);
        
        auto width = atlas.texture->get_width();
        auto height = atlas.texture->get_height();
        
        int line = 1;
        
        for (auto& string : strings)
        {
            auto x = get_vertical_position(string, size, bounds, vertical_alignment);
            auto y = get_horisontal_position(size, line++, (int)strings.size(), bounds, horisontal_alignment);
            
            for (auto& ch : string)
            {
                auto it = atlas.glyphs.find(ch);
                
                if (it == atlas.glyphs.end())
                    continue;
                
                auto& glyph = it->second;
                
                float tx = glyph.tx;
                float ty = glyph.ty;
                
                auto quad = gl::v3f_c4f_t2f_quad();
                
                quad[gl::bottom_left].vertice = { x + glyph.bl, y + glyph.bt, 0.0f };
                quad[gl::bottom_right].vertice = { x + glyph.bl + glyph.bw, y + glyph.bt, 0.0f };
                quad[gl::top_right].vertice = { x + glyph.bl + glyph.bw, y + glyph.bt - glyph.bh, 0.0f };
                quad[gl::top_left].vertice = { x + glyph.bl, y + glyph.bt - glyph.bh, 0.0f };
                
                quad[gl::bottom_left].tex_coord = { tx, ty };
                quad[gl::bottom_right].tex_coord = { tx + (float)glyph.bw / width, ty };
                quad[gl::top_right].tex_coord = { tx + (float)glyph.bw / width, ty + ((float)glyph.bh / height) };
                quad[gl::top_left].tex_coord = { tx, ty + ((float)glyph.bh / height) };
                
                vertices->insert(vertices->end(), quad.begin(), quad.end());
                
                x += glyph.ax;
            }
        }
        
        return atlas.texture;
    }
    
    const font_ttf::atlas& font_ttf::get_atlas(const std::string& text, int size) const
    {
        auto it = m_cache.find(size);
        
        if (it != m_cache.end())
        {
            auto& glyphs = it->second.glyphs;
            
            std::string buffer;
            for (auto& ch : text)
            {
                if (glyphs.find(ch) != glyphs.end())
                    continue;
                
                buffer.push_back(ch);
            }
            
            if (buffer.size() > 0)
                update_atlas(buffer, size);
        }
        else
        {
            update_atlas(text, size);
        }
        
        return m_cache[size];
    }
    
    void font_ttf::update_atlas(const std::string& text, int size) const
    {
        auto& atlas = m_cache[size];
        std::string buffer;
        
        for (auto& it : atlas.glyphs)
            buffer.push_back(it.first);
        
        buffer.insert(buffer.end(), text.begin(), text.end());
        
        auto info = font_utils::create_atlas(m_font_name, size, buffer, max_atlas);
        auto texture = std::make_shared<texture2d>(info.width, info.height, GL_RED, info.texture);
        
        m_cache[size] = { texture, info.glyphs };
    }
    
    float font_ttf::get_vertical_position(const std::string& text, int size, const math::vector2d& bounds, vertical_text_alignment alignment) const
    {
        if (alignment == vertical_text_alignment::left)
            return 0;
        
        auto sz = text_size(text, size, (int)bounds.x);
        
        if (alignment == vertical_text_alignment::center)
            return bounds.x / 2 - sz.x / 2.0f;
        
        return bounds.x - sz.x;
    }
    
    float font_ttf::get_horisontal_position(int size, int line, int total_lines, const math::vector2d& bounds, horisontal_text_alignment alignment) const
    {
        if (alignment == horisontal_text_alignment::top)
            return bounds.y - (size * line);
        
        if (alignment == horisontal_text_alignment::center)
            return (bounds.y / 2) - ((size * total_lines) / 2) + (size * (line - 1));
        
        return (float)(size * total_lines) - (size * line);
    }
    
    std::vector<std::string> font_ttf::wrap_text(const std::string& text, int size, int max_line_width) const
    {
        auto& atlas = get_atlas(text, size);
        return wrap_text(text, atlas.glyphs, max_line_width);
    }
    
    std::vector<std::string> font_ttf::wrap_text(const std::string& text, const font_utils::glyphs_map& glyphs, int max_line_width) const
    {
        std::vector<std::string> strings;
        std::string str;
        
        int x = 0;
        
        for (auto& ch : text)
        {
            if (END_LINE(ch, x, max_line_width))
            {
                strings.push_back(str);
                str.clear();
                x = 0;
                
                continue;
            }
            
            auto it = glyphs.find(ch);
            
            if (it == glyphs.end())
                continue;
            
            x += it->second.ax;
            
            str += ch;
        }
        
        if (str.size() > 0)
            strings.push_back(str);
        
        return strings;
    }
    
    math::vector2d font_ttf::text_size(const std::string& text, int size, int max_line_width) const
    {
        auto sz = font_utils::text_size(m_font_name, size, text, max_line_width);
        return math::vector2d(sz.w, sz.h);
    }
}
