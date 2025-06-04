#include <iostream>
#include <windows.h>
#include "VietKEngine.h"

int main() {
    // Cấu hình console để hỗ trợ UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    VietKEngine engine;
    std::string keys = "aas ";

    for (char key : keys) {
        auto result = engine.processKey(std::string(1, key));
        if (result) {
            std::cout << "Output: " << *result << std::endl;
        } else {
            std::cerr << "Error: " << result.error() << std::endl;
        }
        if (key == ' ') engine.resetWord();
    }

    return 0;
}