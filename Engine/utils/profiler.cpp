#include "common.h"
#include "profiler.h"

namespace engine
{
    profiler::profiler(const std::string& c, const std::string& m) : m_class(c), m_method(m), m_start(director::instance().get_local_time())
    {
        
    }
    
    profiler::~profiler()
    {
        auto delta = director::instance().get_local_time() - m_start;
        
        logger() << "[profiler] " << m_class << "::" << m_method << ":" << delta << "ms";
    }
}
