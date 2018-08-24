#pragma once

namespace engine
{
    class profiler
    {
    public:
        profiler(const std::string& c, const std::string& m);
        ~profiler();
    private:
        time_t m_start = 0;
        std::string m_class, m_method;
    };
}

#define BEGIN_PROFILE(class, method) \
{\
engine::profiler p(#class, #method);

#define END_PROFILE \
}
