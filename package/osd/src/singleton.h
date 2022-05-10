#ifndef _SINGLETON_H_
#define _SINGLETON_H_

/**
 * @brief Singleton class wrapper. 
 * 
 * @tparam T 
 */
template<typename T>
class Singleton
{
    public:
        static T& getInstance()
        {
            static T    instance;
            return instance;
        }
    protected:
        Singleton() {}
        ~Singleton() {}
    public:
        Singleton(Singleton const&) = delete;
        Singleton& operator=(Singleton const&) = delete;
};

#endif