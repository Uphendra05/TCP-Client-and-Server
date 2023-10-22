#include "buffer.h"


Buffer::Buffer(size_t size) : m_Size(size), m_WriteIndex(0), m_ReadIndex(0) 
{
    m_Buffer.resize(size);
}

void Buffer::Grow(size_t newSize) 
{
    if (newSize > m_Size) {
        m_Buffer.resize(newSize);
        m_Size = newSize;
    }
}

void Buffer::SerializeUInt32BE(uint32_t value) 
{
    if (m_WriteIndex + sizeof(value) > m_Size) 
    {
        Grow(m_WriteIndex + sizeof(value));
    }

    m_Buffer[m_WriteIndex + 3] = (value);
    m_Buffer[m_WriteIndex + 2] = (value >> 8);
    m_Buffer[m_WriteIndex + 1] = (value >> 16);
    m_Buffer[m_WriteIndex] = (value >> 24);

    m_WriteIndex += sizeof(value);
}

uint32_t Buffer::DeserializeUInt32BE() 
{
    if (m_ReadIndex + sizeof(uint32_t) > m_Size) 
    {
        return 0;
    }

    uint32_t value = 0;
    value |= (m_Buffer[m_ReadIndex]) << 24;
    value |= (m_Buffer[m_ReadIndex + 1]) << 16;
    value |= (m_Buffer[m_ReadIndex + 2]) << 8;
    value |= (m_Buffer[m_ReadIndex + 3]);

    m_ReadIndex += sizeof(value);

    return value;
}

void Buffer::SerializeUInt16BE(uint16_t value) 
{
    if (m_WriteIndex + sizeof(value) > m_Size) 
    {
        Grow(m_WriteIndex + sizeof(value));
    }

    m_Buffer[m_WriteIndex + 1] = (value);
    m_Buffer[m_WriteIndex] = (value >> 8);

    m_WriteIndex += sizeof(value);
}

uint16_t Buffer::DeserializeUInt16BE() 
{
    if (m_ReadIndex + sizeof(uint16_t) > m_Size) 
    {
        return 0;
    }

    uint16_t value = 0;
    value |= (m_Buffer[m_ReadIndex]) << 8;
    value |= (m_Buffer[m_ReadIndex + 1]);

    m_ReadIndex += sizeof(value);

    return value;
}

void Buffer::SerializeString(const std::string& value) 
{
    SerializeUInt32BE(static_cast<uint32_t>(value.size()));

    if (m_WriteIndex + value.size() > m_Size) 
    {
        Grow(m_WriteIndex + value.size());
    }

    for (char c : value) {
        m_Buffer[m_WriteIndex++] = c;
    }
}

std::string Buffer::DeserializeString(uint32_t length)
{
     length = DeserializeUInt32BE();

    if (m_ReadIndex + length > m_Size) 
    {
        return "";
    }

    std::string value;
    value.reserve(length);

    for (uint32_t i = 0; i < length; i++) 
    {
        value.push_back(static_cast<char>(m_Buffer[m_ReadIndex++]));
    }
    return value;
}