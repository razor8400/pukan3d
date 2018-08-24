#include "common.h"
#include "touch_dispatcher.h"
#include "touch_listener.h"

namespace engine
{
    touch_dispatcher& touch_dispatcher::instance()
    {
        static touch_dispatcher instance;
        return instance;
    }
    
    touch_dispatcher::touch_dispatcher()
    {
        
    }
    
    touch_dispatcher::~touch_dispatcher()
    {

    }
    
    void touch_dispatcher::add_touch_listener(touch_listener* listener)
    {
        m_listeners.push_back(listener);
        std::sort(m_listeners.begin(), m_listeners.end(), [](touch_listener* a, touch_listener* b)
        {
            return a->priority < b->priority;
        });
    }
    
    void touch_dispatcher::remove_touch_listener(touch_listener* listener)
    {
        m_listeners.erase(listener);
    }
    
    void touch_dispatcher::on_touch_began()
    {
        auto location = application::instance().get_mouse_location();
        
        for (auto& listener : m_listeners)
        {
            if (listener->on_touch_began(location))
            {
                m_active_listeners.push_back(listener);
                
                if (!listener->swallow_touches)
                    break;
            }
        }
    }
    
    void touch_dispatcher::on_touch_moved()
    {
        auto location = application::instance().get_mouse_location();
        
        for (auto& listener : m_active_listeners)
            listener->on_touch_moved(location);
        
        for (auto& listener : m_listeners)
        {
            if (listener->swallow_touches)
                listener->on_touch_moved(location);
        }
    }
    
    void touch_dispatcher::on_touch_ended()
    {
        auto location = application::instance().get_mouse_location();
        
        for (auto& listener : m_active_listeners)
            listener->on_touch_ended(location);
        
        for (auto& listener : m_listeners)
        {
            if (listener->swallow_touches)
                listener->on_touch_ended(location);
        }
        
        m_active_listeners.clear();
    }
}
