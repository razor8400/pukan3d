#include "common.h"
#include "scene.h"

#include "renderer/render_command.h"
#include "renderer/camera.h"
#include "renderer/renderer.h"

namespace engine
{
    IMPLEMENT_INHERITANCE_INFO(scene, game_object);
    
    void scene::draw(renderer* r)
    {
        for (auto& obj : m_children)
            obj->draw(r, m_transform);
        
        assert(m_camera);
        
        r->execute_commands(m_camera->get_projection());
    }
}
