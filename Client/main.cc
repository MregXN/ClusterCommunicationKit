#include <string>
#include <iostream>

int main(int argc, const char** argv) {
    
    std::string hostName;
    std::cout << "Please input your hostname: " << std::endl;
    getline(std::cin,hostName);

    std::cout << "hello " << hostName << std::endl;
    return 0;
}