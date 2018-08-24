#pragma once

namespace engine
{
	namespace file_utils
	{
        std::string get_file_name(const std::string& path);
        
        bool file_exist(const std::string& file_name);
        
		bool read_file(const std::string& file_name, unsigned char** data, size_t* size);
        bool read_file(const std::string& file_name, std::vector<char>* buffer);

		struct file_data
		{
			unsigned char* buffer = nullptr;
			size_t size = 0;

			~file_data();
		};
	};
}
