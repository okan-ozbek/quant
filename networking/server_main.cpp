//
// Created by Dorza on 18/10/2026.
//

#include "include/server.h"

int main()
{
    try {
        TCPServer server("127.0.0.1", 8080);
    }
    catch (const std::exception& exception) {
        std::cerr << "Error: " << exception.what() << "\n";
        std::cin.get();
        return EXIT_FAILURE;
    }

    std::cout << "Press Enter to exit..." << "\n";
    std::cin.get();

    return 0;
}
