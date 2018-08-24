#pragma once

#include "resource.h"

namespace engine
{
    class shader : public resource
    {
    public:
        static std::shared_ptr<shader> load_from_file(const std::string& file_name);
        ~shader();
        
        bool load(const unsigned char* data, size_t size) override;
        void use(const math::mat4& t);
    private:
        unsigned int m_program_id = -1;
        std::string m_uniform_matrix;
    };
}
