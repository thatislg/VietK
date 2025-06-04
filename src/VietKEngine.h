//
// Created by VJP-USER on 2025/06/04.
//

#ifndef VIETKENGINE_H
#define VIETKENGINE_H

#endif //VIETKENGINE_H

#pragma once

#include <map> // Thay flat_map bằng map
#include <string>
#include <string_view>
#include <expected>

class VietKEngine {
private:
    std::map<std::string, std::string> telexRules; // Các ký tự cơ bản (aa -> â)
    std::map<char, std::string> toneRules;        // Sử dụng dấu (s -> sắc)
    std::string currentWord;                      // Current input buffer
    bool isVietnameseMode;

    // Helper to apply tone to a character
    std::string applyTone(std::string_view word, char tone);

public:
    VietKEngine();
    void toggleVietnameseMode();
    std::expected<std::string, std::string> processKey(std::string_view key);
    void resetWord();
};