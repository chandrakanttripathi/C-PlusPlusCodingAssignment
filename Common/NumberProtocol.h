// NumberProtocol.h
#pragma once
#include <string>
#include <algorithm>

// canonical pipe name used everywhere
inline constexpr wchar_t kPipeName[] = LR"(\\.\pipe\NumberServicePipe)";

// Trim whitespace (narrow)
inline std::string Trim(const std::string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == std::string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}
