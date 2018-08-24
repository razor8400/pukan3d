#pragma once

#include "resource.h"
#include "scripting/scripting.h"

struct lua_State;

namespace engine
{
    class script : public resource
    {
    public:
        static std::shared_ptr<script> load_from_file(const std::string& file_name);
        
        script(const std::string& name);
        ~script();
        
        bool load(const unsigned char* data, size_t size) override;
		bool run();
		void stop();

		void call_function(int handler);
        void call_function(const std::string& function);
        bool call_boolean_function(const std::string& function) const;
        
        bool running() const { return m_running; }
        
        template<class T>
        void push_user_data(const std::string& field, T* data);
        void push_vector(const std::string& field, const math::vector3d& v3);
        void push_string(const std::string& field, const std::string& str);

		void clear_ref(int handler);
		
		lua_State* get_state() const { return m_state; }
    private:
        std::string m_name;
        std::vector<char> m_buffer;
        
        bool m_running = false;
        
        lua_State* m_state = nullptr;
    };
    
    template<class T>
    void script::push_user_data(const std::string& field, T* data)
    {
        scripting::push_to_table(m_state, m_name, field, data);
    }
}
