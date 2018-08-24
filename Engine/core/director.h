#pragma once

#include "common.h"

namespace engine
{
	class renderer;
    
    class director
    {
    public:
        static director& instance();
        
        math::vector3d get_mouse_location() const;
        math::vector3d convert_screen_to_world(const math::vector2d& screen) const;
        
        void on_focus();
        void on_lost_focus();

		void start();
        void stop();
				
        void run_scene(scene* scene);
        
		void main_loop();
        
        const pointer<scene>& running_scene() const { return m_scene; }
        float get_delta_time() const;
        time_t get_local_time() const;
        
        float calculate_fps() const;
	private:
        void update(float delta_time);
        
#if DRAW_STATS
        void draw_stats();
#endif
        
        director();
        ~director();
    private:
        pointer<scene> m_scene;
        pointer<scene> m_next_scene;
        
#if DRAW_STATS
        pointer<label> m_stats_label;
        pointer<camera> m_default_camera;
#endif
        
        int m_frames = 0;
        float m_time = 0;

		bool m_running = false;
        bool m_paused = false;
        mutable bool m_reset_delta_time = false;

		pointer<renderer> m_renderer;
    };
}
