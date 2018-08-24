#pragma once

#include "resource.h"

namespace engine
{
    class strings_buffer : public resource
    {
    public:
        static std::shared_ptr<strings_buffer> load_from_file(const std::string& file_name);
    
        bool load(const unsigned char* data, size_t size) override;
        
        const std::string data() const { return std::string(m_buffer.data(), m_buffer.data() + m_buffer.size()); }
        size_t size() const { return m_buffer.size(); }
    private:
        std::vector<char> m_buffer;
    };
}
