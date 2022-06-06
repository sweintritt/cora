#include <stdlib.h>
#include <iostream>
#include <exception>

#include "cora.hpp"
#include "utils.hpp"

int main(int argc, char* argv[]) {
    try {
        const std::vector<std::string> args = toStringVector(argc, argv);
        Cora cora;
        cora.run(args);
        return EXIT_SUCCESS;
    } catch (const std::exception& error) {
        std::cerr << error.what() << std::endl;
    } catch (const std::string& error) {
        std::cerr << error << std::endl;
    } catch (const char* error) {
        std::cerr << error << std::endl;
    } catch (...) {
        std::cerr << "error of unknown type" << std::endl;
    }

    return EXIT_FAILURE;
}
