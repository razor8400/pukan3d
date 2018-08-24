#include "common.h"
#include "shader.h"

#include "strings_buffer.h"

#include "utils/file_utils.h"
#include "nlohmann/json.hpp"

namespace engine
{
    std::shared_ptr<shader> shader::load_from_file(const std::string& file_name)
    {
        auto data = file_utils::file_data();
        
        logger() << "[shader] load:" << file_name;
        
        if (file_utils::read_file(file_name, &data.buffer, &data.size))
        {
            auto program = std::make_shared<shader>();
            
            if (program->load(data.buffer, data.size))
                return program;
        }
        
        logger() << "[shader] load error:" << file_name;
        
        return std::shared_ptr<shader>();
    }
    
    shader::~shader()
    {
        if (m_program_id != -1)
            gl::delete_gl_program(m_program_id);
    }
    
    bool shader::load(const unsigned char* data, size_t size)
    {
        try
        {
            auto buffer = std::string(data, data + size);
            
            logger() << "[shader] parse json:\n" << buffer;
            
            auto json = nlohmann::json::parse(buffer);
            
            m_uniform_matrix = json["uniform_matrix"];
            
            auto vert = resources_manager::instance().load_resource_from_file<strings_buffer>(json["vert"]);
            auto frag = resources_manager::instance().load_resource_from_file<strings_buffer>(json["frag"]);
            
            if (!vert || !frag)
            {
                logger() << "[shader] error by loading shaders";
                return false;
            }
            
            if (!gl::create_gl_program(&m_program_id, vert->data().c_str(), frag->data().c_str()))
            {
                logger() << "[shader] error by compiling shaders";
                return false;
            }
            
            return true;
        }
        catch (const nlohmann::json::parse_error& e)
        {
            logger() << "[shader] " << e.what();
        }
        catch (const nlohmann::json::type_error& e)
        {
            logger() << "[shader] " << e.what();
        }
        
        return false;
    }
    
    void shader::use(const math::mat4& t)
    {
        auto location = gl::get_uniform_location(m_program_id, m_uniform_matrix.c_str());
        
        gl::use_program(m_program_id);
        gl::uniform_matrix(location, t);
    }
}
