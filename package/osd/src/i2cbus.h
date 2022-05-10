#ifndef _I2CSBUS_H_
#define _I2CBUS_H_

#include <functional>
#include <mutex>
#include "singleton.h"

/**
 * @brief Provided parameter is a file descriptor
 * to open I2C device. Return true for success and false
 * for failure.
 */
typedef std::function<bool(const int&)> I2CCALL;


/**
 * @brief Singleton of I2CBus with threadsafe
 * Call function. Designed to allow safe I2C bus 
 * access from multiple threads.
 */
class I2CBus : public Singleton<I2CBus>
{
    friend class Singleton<I2CBus>;
 
private:
   I2CBus();
   ~I2CBus();

public:
    bool IsOpen() const;
    bool Open();
    void Close();

    bool Call(I2CCALL func);

private:
    std::mutex  m_mutex;
    int m_I2CFileDescriptor;
};

#endif