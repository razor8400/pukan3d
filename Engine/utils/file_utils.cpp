#include "common.h"

#include "file_utils.h"
#include "platform/file_system/file_system.h"

namespace engine
{
    namespace file_utils
    {
        bool read_file(const std::string& file_name, std::vector<char>* buffer)
        {
            if (!buffer)
                return false;
            
            unsigned char* data;
            size_t size;
            
            if (read_file(file_name, &data, &size))
            {
                buffer->clear();
                buffer->reserve(size);
                buffer->insert(buffer->end(), data, data + size);
                
                delete [] data;
                
                return true;
            }
            
            return false;
        }
        
        bool file_exist(const std::string& file_name)
        {
            auto file = file_system::open_file(file_name.c_str(), "rb");
            
            if (file != NULL)
            {
                fclose(file);
                return true;
            }
            
            return false;
        }
        
        bool read_file(const std::string& file_name, unsigned char** data, size_t* size)
        {
            auto file = file_system::open_file(file_name.c_str(), "rb");
            
            if (file != NULL)
            {
                fseek(file, 0, SEEK_END);
                
                *size = ftell(file);
                *data = new unsigned char[*size];

                fseek(file, 0, 0);
                
                auto bytes_read = fread(*data, sizeof(unsigned char), *size, file);
                
                fclose(file);
                
                return bytes_read == *size;
            }
            
            return false;
        }
        
        std::string get_file_name(const std::string& path)
        {
            auto file = path;
            auto i = file.find_last_of('/');
            
            if (i != std::string::npos)
            {
                file = file.substr(i + 1);
                i = file.find_last_of('.');
                
                if (i != std::string::npos)
                    file = file.substr(0, i);
            }
            
            return file;
        }

		file_data::~file_data()
		{
			if (buffer)
				delete[] buffer;
		}
	}
}
