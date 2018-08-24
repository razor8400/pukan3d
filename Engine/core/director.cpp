#include "common.h"

#include <chrono>

#include "director.h"
#include "application.h"

#include "platform/platform.h"

#include "renderer/renderer.h"
#include "renderer/render_command.h"
#include "resources/resources_manager.h"

#include "2d/label.h"

#include "scene.h"
#include "pool_manager.h"

namespace engine
{
    director& director::instance()
    {
        static director instance;
        return instance;
    }
    
    director::director()
    {
        m_renderer = ref::create<renderer>();
    }
    
    director::~director()
    {

    }
    
    math::vector3d director::get_mouse_location() const
    {
        auto mouse = application::instance().get_mouse_location();
        return convert_screen_to_world(mouse);
    }
    
    math::vector3d director::convert_screen_to_world(const math::vector2d& screen) const
    {
        auto win_size = application::instance().get_win_size();
        return screen - win_size / 2.0f;
    }

	void director::start()
    {
		logger() << "[director] start";

        m_running = true;
		m_reset_delta_time = true;
        
#if DRAW_STATS
        m_default_camera = ref::create<camera>();
        m_default_camera->set_far_plane(100);
        m_default_camera->set_near_plane(-100);
        m_default_camera->set_position(math::vector3d(0.0f, 0.0f, -1.0f));
        m_default_camera->update_projection();
        
        m_stats_label = label::create<label>("fonts/arial.ttf", 18);
        
        if (m_stats_label)
            m_stats_label->set_anchor(math::vector2d::zero);
#endif
    }
    
    void director::stop()
    {
        m_running = false;
    }
    
    void director::run_scene(scene* scene)
    {
		logger() << "[director] run scene";
        
        m_next_scene = scene;
    }
    
    void director::main_loop()
    {
        if (m_paused || !m_running)
            return;
        
        auto errors = gl::get_errors();
        
        for (auto error : errors)
            logger() << "[gl] " << error;
        
        gl::clear_errors();
        
        if (m_next_scene)
        {
            if (m_scene)
                m_scene->on_exit();
            
            m_scene = m_next_scene;
            m_scene->on_enter();
            
            m_next_scene = nullptr;
        }
        
        if (m_running)
        {
            auto delta = get_delta_time();
            
            update(delta);
        }
    }
    
    void director::update(float delta_time)
    {
        gl::clear();
        
        if (m_scene)
        {
            m_scene->update(delta_time);
            m_scene->draw(m_renderer);
        }
        
        ++m_frames;
        m_time += delta_time;
        
#if DRAW_STATS
        draw_stats();
#endif
        
        gl::clear_draw_calls();
        
        pool_manager::instance().update();
    }
    
#if DRAW_STATS
    void director::draw_stats()
    {
        auto win_size = application::instance().get_win_size();
     
        std::stringstream stats;
        
        stats.precision(4);
        stats << "fps:" << calculate_fps() << std::endl
                        << "draw calls:" << gl::get_draw_calls() << " per frame" << std::endl
                        << "memory used:" << platform::instance().get_memory_used() << "KB";
    
		if (m_stats_label)
		{
			m_stats_label->set_caption(stats.str());
            m_stats_label->set_position(math::vector2d(-win_size.x / 2, win_size.y / 2 - m_stats_label->get_size().y));

			m_stats_label->update(0);
            m_stats_label->draw(m_renderer, math::mat4::identity);
            m_renderer->execute_commands(m_default_camera->get_projection());
		}
    }
#endif
    
    float director::get_delta_time() const
    {
        static auto last_update = std::chrono::steady_clock::now();
        
        auto now = std::chrono::steady_clock::now();
        auto delta = std::chrono::duration_cast<std::chrono::duration<float>>(now - last_update);
        
        last_update = now;
        
        if (m_reset_delta_time)
        {
            m_reset_delta_time = false;
            return 0;
        }
        
        return delta.count();
    }
    
    time_t director::get_local_time() const
    {
        auto now = std::chrono::steady_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());

        return ms.count();
    }
    
    float director::calculate_fps() const
    {       
        return m_frames / m_time;
    }
    
    void director::on_focus()
    {
        m_paused = false;
        m_reset_delta_time = true;
    }
    
    void director::on_lost_focus()
    {
        m_paused = true;
    }
}
