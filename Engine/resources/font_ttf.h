#pragma once

#include "resource.h"
#include "utils/font_utils.h"

namespace engine
{
	class font_ttf : public resource
	{
        struct atlas
        {
            texture2d_ptr texture;
            font_utils::glyphs_map glyphs;
        };
	public:
        font_ttf(const std::string& font_name);
        ~font_ttf();
        
		static std::shared_ptr<font_ttf> load_from_file(const std::string& file_name);
        bool load(const unsigned char* data, size_t size) override;
        
        texture2d_ptr create_label(const std::string& text, int size, const math::vector2d& bounds,
                                    vertical_text_alignment vertical_alignment,
                                    horisontal_text_alignment horisontal_alignment,
                                    std::vector<gl::v3f_c4f_t2f>* vertices) const;
        math::vector2d text_size(const std::string& text, int size, int max_line_width) const;
        std::vector<std::string> wrap_text(const std::string& text, int size, int max_line_width) const;
    private:
        std::vector<std::string> wrap_text(const std::string& text, const font_utils::glyphs_map& glyphs, int max_line_width) const;
        void update_atlas(const std::string& text, int size) const;
        const atlas& get_atlas(const std::string& text, int size) const;
        
        float get_vertical_position(const std::string& text, int size, const math::vector2d& bounds, vertical_text_alignment alignment) const;
        float get_horisontal_position(int size, int line, int total_lines, const math::vector2d& bounds, horisontal_text_alignment alignment) const;
	private:
        std::string m_font_name;
		mutable std::map<int, atlas> m_cache;
	};
}
