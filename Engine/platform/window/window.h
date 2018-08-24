#pragma once

#include "common.h"

namespace engine
{
    class window
    {
    public:
		static std::unique_ptr<window> create();

		virtual bool create(const char* display_name, int width, int height) = 0;
		virtual void process() = 0;
		virtual void terminate() = 0;
		virtual const math::vector2d& get_win_size() const = 0;
		virtual math::vector2d get_mouse_location() const = 0;
    };
}
