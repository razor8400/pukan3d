#pragma once

#include "resource.h"

namespace engine
{        
    class resources_manager
    {
    public:
        static resources_manager& instance();
        
        template<class T>
        std::shared_ptr<T> load_resource_from_file(const std::string& file_name) const;
        
		template<class T>
		std::vector<std::shared_ptr<T>> get_resources() const;
        
        std::string get_path_to_resource(const std::string& resource) const;
        void clear_cache();
        void add_resources_folder(const std::string& folder);
    private:
        resources_manager();
        
        mutable std::map<std::string, resource_ptr> m_resources;
        std::vector<std::string> m_folders;
    };
}

#include "resources_manager.inl"

