#include "common.h"

#include "utils/file_utils.h"
#include "script.h"

namespace engine
{
    std::shared_ptr<script> script::load_from_file(const std::string& file_name)
    {
		auto data = file_utils::file_data();
		
		logger() << "[script] load:" << file_name;

        if (file_utils::read_file(file_name, &data.buffer, &data.size))
        {
            auto name = file_utils::get_file_name(file_name);
            auto lua = std::make_shared<script>(name);

            if (lua->load(data.buffer, data.size))
                return lua;
        }

        logger() << "[script] load error:" << file_name;
                
        return std::shared_ptr<script>();
    }
    
    script::script(const std::string& name) : m_name(name)
    {

    }
    
    script::~script()
    {
        stop();
    }
    
    bool script::run()
    {
        m_state = scripting::create_state();
        
        if (m_state)
        {
			logger() << "[script] run:" << m_name;

            scripting::create_class(m_state, m_name.c_str());
            scripting::register_objects(m_state);
            scripting::register_functions(m_state);
            
            m_running = true;
            
            return scripting::load_script(m_state, m_buffer.data(), m_buffer.size(), m_name);
        }

		return false;
    }
    
    void script::stop()
    {
        if (m_state)
        {
			logger() << "[script] stop:" << m_name;

            scripting::close_state(m_state);
            m_running = false;
            m_state = nullptr;
        }
    }
    
    bool script::load(const unsigned char* data, size_t size)
    {
        m_buffer.clear();
        m_buffer.reserve(size);
        m_buffer.insert(m_buffer.end(), data, data + size);
        
        return m_buffer.size() > 0;
    }
    
	void script::call_function(int handler)
	{
		if (m_state)
			scripting::call_method(m_state, handler);
	}

	void script::call_function(const std::string& function)
    {
		if (m_state)
			scripting::call_method(m_state, m_name, function);
    }
    
    bool script::call_boolean_function(const std::string& function) const
    {
		if (m_state)
			return scripting::call_boolean_method(m_state, m_name, function);
		return false;
    }
    
    void script::push_vector(const std::string& field, const math::vector3d& v3)
    {
		if (m_state)
			scripting::push_to_table(m_state, m_name, field, v3);
    }
    
    void script::push_string(const std::string& field, const std::string& str)
    {
        if (m_state)
            scripting::push_to_table(m_state, m_name, field, str);
    }

	void script::clear_ref(int handler)
	{
		if (m_state)
			scripting::clear_ref(m_state, handler);
	}
}
