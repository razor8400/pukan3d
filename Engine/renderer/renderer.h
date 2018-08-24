#pragma once

#include "core/ref.h"

namespace engine
{
    class renderer : public ref
	{
	public:
        void add_command(const render_command_ptr& command);
        void add_post_draw_command(const render_command_ptr& command);
        
        void execute_commands(const math::mat4& t);
	private:
        std::vector<render_command_ptr> m_draw_commands;
        std::vector<render_command_ptr> m_post_draw_commands;
	};
}
