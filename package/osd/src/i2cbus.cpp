#include "i2cbus.h"
#include <unistd.h>     //open/close
#include <fcntl.h>      // O_RDWR

#define I2C_DEV "/dev/i2c-1"

/**
 * @brief Construct a new I2CBus::I2CBus object
 * 
 */
I2CBus::I2CBus()
    : m_I2CFileDescriptor(0)
{
    
}

/**
 * @brief Destroy the I2CBus::I2CBus object.
 * Closes the file descriptor if still open.
 * 
 */
I2CBus::~I2CBus()
{
    Close();
}

/**
 * @brief Checks if I2C device file has been opened.
 * 
 * @return true if I2C device file descriptor exists
 * @return false if I2C device file descriptor does not exist
 */
bool I2CBus::IsOpen() const
{
    return m_I2CFileDescriptor != 0;
}

/**
 * @brief Opens the I2C device file and stores the
 * file descriptor.
 * 
 * @return true If opened successfully.
 * @return false If was open already or open failed.
 */
bool I2CBus::Open()
{
    if (IsOpen())
        return false;

    m_I2CFileDescriptor = open(I2C_DEV, O_RDWR);
 
    return IsOpen();
}

/**
 * @brief Closes the I2C device file descriptor
 * if it is open.
 * 
 */
void I2CBus::Close()
{
   
    if (IsOpen())
        close(m_I2CFileDescriptor);

    m_I2CFileDescriptor = 0; 

}

/**
 * @brief Tries to open I2C device file
 * if not open already, then calls given
 * 'func' with the file descriptor as a
 * parameter. The function utilizes mutex
 * for threadsafe operations.
 * 
 * @param func function to execute.
 * @return true if all succeed.
 * @return false othewise.
 */
bool I2CBus::Call(I2CCALL func)
{
    m_mutex.lock();
    
    if (!IsOpen() && !Open())
    {
        m_mutex.unlock();
        return false;
    } 

    bool res = func(m_I2CFileDescriptor);

    m_mutex.unlock();
    return res;
}
