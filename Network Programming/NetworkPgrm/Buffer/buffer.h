
#include <vector>
#include <string>

typedef unsigned int uint32;
typedef unsigned short uint16_t;
typedef unsigned int uint8;


class Buffer 
{
public:
    Buffer(size_t size );
    void Grow(size_t newSize);

    void SerializeUInt32BE(uint32_t value);
    uint32_t DeserializeUInt32BE();

    void SerializeUInt16BE(uint16_t value);
    uint16_t DeserializeUInt16BE();

    void SerializeString(const std::string& value);
    std::string DeserializeString(uint32_t length);


    
    size_t m_Size;
    size_t m_ReadIndex = 0;
    size_t m_WriteIndex = 0;

    std::vector<uint8> m_Buffer;

private:
    
};