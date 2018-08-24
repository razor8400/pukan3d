#pragma once

#define END_LINE(ch, width, max_width) \
    ch == '\n' || (max_width > 0 && width >= max_width)

namespace engine
{
    namespace font_utils
    {
        struct glyph
        {
			long ax;   // advance.x
			long ay;   // advance.y

			int bw;    // bitmap.width;
            int bh;    // bitmap.rows;

            int bl;    // bitmap_left;
            int bt;    // bitmap_top;
            
            float tx, ty;
        };

		typedef std::map<char, glyph> glyphs_map;

        struct size
        {
            int w = 0, h = 0;
        };
        
		struct atlas_info
		{
			int texture;
            int width;
            int height;
			glyphs_map glyphs;
		};
        
        bool load_font(const unsigned char* buffer, size_t data, const std::string& font_name);
		bool load_font(const std::string& file_name, const std::string& font_name);
		void unload_font(const std::string& font_name);

		atlas_info create_atlas(const std::string& font_name, int font_size, const std::string& text, int max_width);
		size text_size(const std::string& font_name, int font_size, const std::string& text, int max_width);
    }
}
