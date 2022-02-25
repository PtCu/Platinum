

#ifndef CORE_SINGLETON_H_
#define CORE_SINGLETON_H_

namespace platinum
{
    template <typename T>
    class Singleton
    {
    public:
        static T &GetInstance()
        {
            static T instance;
            return instance;
        }
        static T &GetInstance(size_t size)
        {
            static T instance(size);
            return instance;
        }
        Singleton(const Singleton &) = delete;
        Singleton &operator=(const Singleton &) = delete;
        virtual ~Singleton() = default;

    protected:
        //构造函数需要是protected的，以能够继承
        Singleton() = default;
    };
}

#endif