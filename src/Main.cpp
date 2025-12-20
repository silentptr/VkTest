#include <iostream>

#include "VkTest/App.h"

int main()
{
    std::cout << "Initialising application...\n\n";

    try
    {
        VkTest::App app;
    }
    catch (const std::runtime_error& e)
    {
        std::cerr << "Error occured: " << e.what() << "\n";
        return 1;
    }
    
    std::cout << "\nExited successfully.\n";
    return 0;
}