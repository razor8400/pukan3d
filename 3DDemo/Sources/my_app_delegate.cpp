#include "my_scene3d.h"
#include "my_app_delegate.h"

bool my_app_delegate::application_launched(engine::application* application)
{
	application->set_win_size(1024, 768);
	application->set_display_name("Test Application");
 
    if (application->create_context_window())
    {
        engine::director& director = engine::director::instance();
        
        gl::enable_depth();
        
        auto scene = my_scene3d::create<my_scene3d>();
        auto camera = engine::ref::create<engine::camera>();
        
        camera->set_field_of_view(45.0);
        camera->set_projection_mode(engine::perspective);
        camera->set_position(math::vector3d(0, 3, -1));
        camera->set_target(math::vector3d(0, 0, 1));
        camera->set_far_plane(0.1f);
        camera->set_near_plane(100.0f);
        
        scene->set_camera(camera);
		scene->set_position(math::vector3d(-50, 0, -50));
        scene->set_size(math::vector3d(100, 100, 100));
        
        director.start();
        director.run_scene(scene);
        
        application->run();
        
        return true;
    }
    
    return false;
}

void my_app_delegate::application_terminated(engine::application* application)
{
    application->shutdown();
}

void my_app_delegate::application_enter_background(engine::application* application)
{
    engine::resources_manager::instance().clear_cache();
}

void my_app_delegate::application_enter_foreground(engine::application* application)
{
    engine::logger() << "[my_app_delegate] restart current scene";
}


