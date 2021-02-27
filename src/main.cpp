#include <stdlib.h>
#include <iostream>
#include <exception>

#include "cora.hpp"

int main(int argc, char* argv[]) {
    std::cout << "Starting cora" << std::endl;
    try {
        Cora cora;
        cora.init(argc, argv);
        cora.run();
        return EXIT_SUCCESS;
    } catch (const std::exception& error) {
        std::cerr << error.what() << std::endl;
    } catch (const std::string& error) {
        std::cerr << error << std::endl;
    } catch (const char* error) {
        std::cerr << error << std::endl;
    } catch (...) {
        std::cerr << "error of unknown type << std::endl";
    }

    return EXIT_FAILURE;
}
