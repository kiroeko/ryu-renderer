#ifndef __SINGLETON_H__
#define __SINGLETON_H__

namespace OGLRenderer::Common
{
    template<class T>
    class Singleton
    {
        friend T;
    public:
        static T& GetInstance()
        {
            static T instance;
            return instance;
        }

        Singleton(const Singleton&) = delete;
        Singleton& operator= (const Singleton&) = delete;
    private:
        Singleton() {}
        ~Singleton() {}
    };
}

#endif