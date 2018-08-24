#pragma once

namespace engine
{
    class ref
    {
    public:
        template<class T, class ...Args>
        static T* create(Args... args);
        
        ref* retain();
        ref* autorelease();
        void release();
        
        int counter() const { return m_counter; }
    protected:
        ref();
        virtual ~ref();
    private:
        int m_counter = 0;
    };
    
    template<class T, class ...Args>
    T* ref::create(Args... args)
    {
        auto obj = new T(args...);
        
        return dynamic_cast<T*>(obj->autorelease());
    }
}

