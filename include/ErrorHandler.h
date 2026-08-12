#pragma once
#include <stdexcept>
#include <string>

inline void panic(const std::string& error_message) {
    throw std::runtime_error("[ArgueWithCpp Error] " + error_message);
}
