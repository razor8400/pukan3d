#include "common.h"
#include "platform/file_system/file_system.h"
#include "resources_manager.h"

namespace engine
{
    resources_manager& resources_manager::instance()
    {
        static resources_manager manager;
        
        return manager;
    }
    
    resources_manager::resources_manager()
    {
        
    }
    
    void resources_manager::clear_cache()
    {
        logger() << "[resources_manager] clear cache";
        m_resources.clear();
    }
    
    void resources_manager::add_resources_folder(const std::string& folder)
    {
        logger() << "[resources_manager] add_resources_folder:" << folder;
        m_folders.push_back(folder);
    }
    
    std::string resources_manager::get_path_to_resource(const std::string& resource) const
    {
        auto directory = file_system::get_current_directory();
        
        for (auto& folder : m_folders)
        {
            auto path = directory + '/' + folder + '/' + resource;
            if (file_utils::file_exist(path))
                return path;
        }
        
        return resource;
    }
}
