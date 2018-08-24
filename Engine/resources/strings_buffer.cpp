#include "common.h"
#include "strings_buffer.h"

namespace engine
{
    std::shared_ptr<strings_buffer> strings_buffer::load_from_file(const std::string& file_name)
    {
        auto data = file_utils::file_data();
        
        logger() << "[strings_buffer] load:" << file_name;
        
        if (file_utils::read_file(file_name, &data.buffer, &data.size))
        {
            auto name = file_utils::get_file_name(file_name);
            auto buffer = std::make_shared<strings_buffer>();
            
            if (buffer->load(data.buffer, data.size))
                return buffer;
        }
        
        logger() << "[strings_buffer] load error:" << file_name;
        
        return std::shared_ptr<strings_buffer>();
    }
    
    bool strings_buffer::load(const unsigned char* data, size_t size)
    {
        m_buffer.clear();
        m_buffer.reserve(size);
        m_buffer.insert(m_buffer.end(), data, data + size);
        
        return m_buffer.size() > 0;
    }
}
