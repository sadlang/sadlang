#include "../../include/data/types/value.h"
#include <iostream>

using namespace Sad::Data;

int main() {
    Value v("مرحبا");
    std::cout << "Type: " << static_cast<int>(v.getType()) << std::endl;
    std::cout << "Expected STRING (3): " << static_cast<int>(::Sad::Types::SadTypeKind::String) << std::endl;
    std::cout << "isString: " << v.isString() << std::endl;
    std::cout << "isInteger: " << v.isInteger() << std::endl;
    std::cout << "isDouble: " << v.isDouble() << std::endl;
    std::cout << "isBoolean: " << v.isBoolean() << std::endl;
    std::cout << "isVoid: " << v.isVoid() << std::endl;
    
    // Try with const char*
    const char* str = "مرحبا";
    std::string temp(str);
    Value v2(temp);
    std::cout << "\nWith std::string conversion:" << std::endl;
    std::cout << "Type: " << static_cast<int>(v2.getType()) << std::endl;
    std::cout << "isString: " << v2.isString() << std::endl;
    
    return 0;
}
