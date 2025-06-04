#include "VietKEngine.h"
#include <ranges>
#include <algorithm>

// Bảng ánh xạ dấu thanh cho nguyên âm (precomposed Unicode)
static const std::map<std::string, std::map<char, std::string>> toneMap = {
    {"a", {{'s', "á"}, {'f', "à"}, {'r', "ả"}, {'x', "ã"}, {'j', "ạ"}, {'z', "a"}}},
    {"â", {{'s', "ấ"}, {'f', "ầ"}, {'r', "ẩ"}, {'x', "ẫ"}, {'j', "ậ"}, {'z', "â"}}},
    {"ă", {{'s', "ắ"}, {'f', "ằ"}, {'r', "ẳ"}, {'x', "ẵ"}, {'j', "ặ"}, {'z', "ă"}}},
    {"e", {{'s', "é"}, {'f', "è"}, {'r', "ẻ"}, {'x', "ẽ"}, {'j', "ẹ"}, {'z', "e"}}},
    {"ê", {{'s', "ế"}, {'f', "ề"}, {'r', "ể"}, {'x', "ễ"}, {'j', "ệ"}, {'z', "ê"}}},
    {"i", {{'s', "í"}, {'f', "ì"}, {'r', "ỉ"}, {'x', "ĩ"}, {'j', "ị"}, {'z', "i"}}},
    {"o", {{'s', "ó"}, {'f', "ò"}, {'r', "ỏ"}, {'x', "õ"}, {'j', "ọ"}, {'z', "o"}}},
    {"ô", {{'s', "ố"}, {'f', "ồ"}, {'r', "ổ"}, {'x', "ỗ"}, {'j', "ộ"}, {'z', "ô"}}},
    {"ơ", {{'s', "ớ"}, {'f', "ờ"}, {'r', "ở"}, {'x', "ỡ"}, {'j', "ợ"}, {'z', "ơ"}}},
    {"u", {{'s', "ú"}, {'f', "ù"}, {'r', "ủ"}, {'x', "ũ"}, {'j', "ụ"}, {'z', "u"}}},
    {"ư", {{'s', "ứ"}, {'f', "ừ"}, {'r', "ử"}, {'x', "ữ"}, {'j', "ự"}, {'z', "ư"}}},
    {"y", {{'s', "ý"}, {'f', "ỳ"}, {'r', "ỷ"}, {'x', "ỹ"}, {'j', "ỵ"}, {'z', "y"}}}
};


VietKEngine::VietKEngine() : isVietnameseMode(true) {
    // Initialize Telex base rules
    telexRules = {
        {"aa", "â"},
        {"ee", "ê"},
        {"oo", "ô"},
        {"dd", "đ"},
        {"uw", "ư"},
        {"ow", "ơ"},
        {"aw", "ă"}
    };

    // Initialize tone rules
    toneRules = {
        {'s', "sắc"}, {'f', "huyền"}, {'r', "hỏi"},
        {'x', "ngã"}, {'j', "nặng"}, {'z', "không"}
    };
}

void VietKEngine::toggleVietnameseMode() {
    isVietnameseMode = !isVietnameseMode;
}

std::string VietKEngine::applyTone(std::string_view word, char tone) {
    if (word.empty()) return "";

    // Tìm ký tự cuối hợp lệ (nguyên âm hoặc tổ hợp)
    std::string lastChar;
    size_t pos = word.size();
    while (pos > 0) {
        lastChar = std::string(word.substr(pos - 1, 1));
        if (toneMap.contains(lastChar)) break;
        --pos;
    }
    if (pos == 0) return std::string(word); // Không tìm thấy nguyên âm

    // Áp dụng dấu thanh
    auto tones = toneMap.at(lastChar);
    if (tones.contains(tone)) {
        return std::string(word.substr(0, pos - 1)) + tones.at(tone);
    }
    return std::string(word);
}

std::expected<std::string, std::string> VietKEngine::processKey(std::string_view key) {
    if (!isVietnameseMode) return std::string(key);
    if (key.empty() || key.size() > 1) return std::unexpected("Invalid key");

    char c = key[0];
    if (!std::isalnum(c)) {
        std::string result = currentWord + c;
        resetWord();
        return result;
    }

    currentWord += c;

    // Kiểm tra quy tắc Telex
    for (const auto& [pattern, replacement] : telexRules) {
        if (currentWord.ends_with(pattern)) {
            std::string result = currentWord.substr(0, currentWord.size() - pattern.size()) + replacement;
            currentWord = replacement; // Cập nhật currentWord
            return result;
        }
    }

    // Kiểm tra dấu thanh
    if (toneRules.contains(c)) {
        std::string result = applyTone(currentWord.substr(0, currentWord.size() - 1), c);
        currentWord = result; // Cập nhật currentWord
        return result;
    }

    return currentWord;
}

void VietKEngine::resetWord() {
    currentWord.clear();
}