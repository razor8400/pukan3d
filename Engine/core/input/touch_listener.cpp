#include "common.h"
#include "touch_dispatcher.h"
#include "touch_listener.h"

namespace engine
{
    bool touch_listener::on_touch_began(const math::vector2d& location)
    {
        if (touch_began)
            return touch_began(location);
        return false;
    }
    
    void touch_listener::on_touch_moved(const math::vector2d& location)
    {
        if (touch_moved)
            touch_moved(location);
    }
    
    void touch_listener::on_touch_ended(const math::vector2d& location)
    {
        if (touch_ended)
            touch_ended(location);
    }
}
