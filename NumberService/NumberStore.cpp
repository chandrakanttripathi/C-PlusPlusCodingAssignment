// NumberStore.cpp
#include "NumberStore.h"
#include <chrono>
#include <sstream>

bool NumberStore::Insert(int number) {
    if (number <= 0) return false;
    std::lock_guard<std::mutex> lock(mutex_);
    if (numbers_.count(number) > 0) return false;
    long long timestamp = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    numbers_[number] = timestamp;
    return true;
}

bool NumberStore::Remove(int number) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = numbers_.find(number);
    if (it == numbers_.end()) return false;
    numbers_.erase(it);
    return true;
}

void NumberStore::Clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    numbers_.clear();
}

bool NumberStore::Contains(int number) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return numbers_.count(number) > 0;
}

bool NumberStore::TryGetValue(int number, long long& outTimestamp) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = numbers_.find(number);
    if (it != numbers_.end()) {
        outTimestamp = it->second;
        return true;
    }
    return false;
}

std::string NumberStore::PrintAll() const {
    std::lock_guard<std::mutex> lock(mutex_);
    if (numbers_.empty()) return "No numbers stored.\n";
    std::ostringstream oss;
    for (const auto& p : numbers_) {
        oss << p.first << " (timestamp: " << p.second << ")\n";
    }
    return oss.str();
}
