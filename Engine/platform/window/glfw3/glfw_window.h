#pragma once

#include "platform/window/window.h"

struct GLFWwindow;

namespace engine
{
	class glfw_window : public window
	{
	public:
		bool create(const char* display_name, int width, int height) override;
		void process() override;
		void terminate() override;
		const math::vector2d& get_win_size() const override { return m_win_size; }
		math::vector2d get_mouse_location() const override;
	private:
		GLFWwindow* m_window = nullptr;
		math::vector2d m_win_size;
	};
}