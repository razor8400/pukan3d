#pragma once

#include "core/ref.h"

namespace engine
{
    class touch_listener : public ref
    {
    public:
        bool on_touch_began(const math::vector2d& location);
        void on_touch_moved(const math::vector2d& location);
        void on_touch_ended(const math::vector2d& location);
    public:
        int priority = -1;
        bool swallow_touches = false;
        
        std::function<bool(const math::vector2d&)> touch_began;
        std::function<void(const math::vector2d&)> touch_moved;
        std::function<void(const math::vector2d&)> touch_ended;
    };
}
