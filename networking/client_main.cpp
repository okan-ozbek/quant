//
// Created by Dorza on 19/10/2026.
//

#include "include/client.h"
#include "include/data.h"

int main()
{
    try {
        const TCPClient client("127.0.0.1", 8080);

        while (true) {
            std::string message{};

            std::cout << "Message: ";
            if (!std::getline(std::cin, message)) {
                break;
            }

            client.Send(client.GetSocket(), Encode(message));
        }
    }
    catch (const std::exception& exception) {
        std::cerr << exception.what() << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
