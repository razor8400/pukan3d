#include "common.h"
#include "renderer.h"
#include "render_command.h"

#include "core/application.h"
#include "core/scene.h"

namespace engine
{
    void renderer::execute_commands(const math::mat4& t)
    {
        for (auto& command : m_draw_commands)
        {
            command->execute(t);
            command->reset();
        }
        
        for (auto& command : m_post_draw_commands)
        {
            command->execute(t);
            command->reset();
        }
        
        m_post_draw_commands.clear();
        m_draw_commands.clear();
    }
    
    void renderer::add_command(const render_command_ptr& command)
    {
        m_draw_commands.push_back(command);
    }

    void renderer::add_post_draw_command(const render_command_ptr& command)
    {
        m_post_draw_commands.push_back(command);
    }
}
