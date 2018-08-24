#include "common.h"

#include "core/director.h"
#include "core/application.h"
#include "core/input/touch_dispatcher.h"

#include "platform/window/glfw3/glfw_window.h"

#include "GLFW3/glfw3.h"

namespace engine
{      
	std::unique_ptr<window> window::create()
	{
		return std::make_unique<glfw_window>();
	}

	void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT)
		{
            if (action == GLFW_PRESS)
            {
                touch_dispatcher::instance().on_touch_began();
            }
            else if (action == GLFW_RELEASE)
            {
                touch_dispatcher::instance().on_touch_ended();
            }
		}
	}

	void mouse_move_callback(GLFWwindow* window, double x, double y)
	{
        touch_dispatcher::instance().on_touch_moved();
	}

    void window_close_callback(GLFWwindow* window)
    {
        application::instance().on_terminated();
    }
    
    void window_iconify_callback(GLFWwindow* window, int iconified)
    {
        if (iconified)
        {
            application::instance().on_enter_background();
        }
        else
        {
            application::instance().on_enter_foreground();
        }
    }
    
    void window_focus_callback(GLFWwindow* window, int focused)
    {
        if (focused)
        {
            director::instance().on_focus();
        }
        else
        {
            director::instance().on_lost_focus();
        }
    }
    
	bool glfw_window::create(const char* display_name, int width, int height)
	{
		if (!glfwInit())
			return false;

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPEN_GL_MAJOR_VERSION);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPEN_GL_MINOR_VERSION);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		m_window = glfwCreateWindow(width, height, display_name, NULL, NULL);
		m_win_size.x = (float)width;
		m_win_size.y = (float)height;
		
		if (!m_window)
		{
			glfwTerminate();
			return false;
		}
        
        glfwMakeContextCurrent(m_window);
        
		glfwSetCursorPosCallback(m_window, mouse_move_callback);
		glfwSetMouseButtonCallback(m_window, mouse_button_callback);
        
        glfwSetWindowCloseCallback(m_window, window_close_callback);
        glfwSetWindowIconifyCallback(m_window, window_iconify_callback);
        
        glfwSetWindowFocusCallback(m_window, window_focus_callback);

		return true;
	}

	void glfw_window::process()
    {
		while (!glfwWindowShouldClose(m_window))
        {           
			director::instance().main_loop();
            
            glfwSwapBuffers(m_window);
            glfwPollEvents();
        }
        
        terminate();
    }
    
    void glfw_window::terminate()
    {
        glfwTerminate();
    }
    
    math::vector2d glfw_window::get_mouse_location() const
    {
        double x, y;
        glfwGetCursorPos(m_window, &x, &y);
        
        y = m_win_size.y - y;
        
        return math::vector2d((float)x, (float)y);
    }
}
