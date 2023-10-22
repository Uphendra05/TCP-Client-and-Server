#include"Buffer/buffer.h"
#include <iostream>



int main() {
    Buffer buf(512);

   
    buf.SerializeUInt32BE(12345);
    uint32_t value1 = buf.DeserializeUInt32BE();

    buf.SerializeUInt16BE(6789);
    uint16_t value2 = buf.DeserializeUInt16BE();

    buf.SerializeString("Hello");
    std::string value3 = buf.DeserializeString(5);

    std::cout << "Value 1: " << value1 << std::endl;
    std::cout << "Value 2: " << value2 << std::endl;
    std::cout << "Value 3: " << value3 << std::endl;

    return 0;
}