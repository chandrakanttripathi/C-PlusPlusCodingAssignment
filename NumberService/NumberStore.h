#pragma once
#include <map>
#include <string>
#include <mutex>

class NumberStore {
public:
    // Insert number with timestamp; return true if inserted, false if duplicate
    bool Insert(int number);

    // Remove a number; return true if removed, false if not found
    bool Remove(int number);

    // Remove all numbers
    void Clear();

    // Check if number exists
    bool Contains(int number) const;

    bool TryGetValue(int number, long long& outTimestamp) const;

    // Return all numbers with timestamps in sorted order
    std::string PrintAll() const;

private:
    // Map of number -> insertion timestamp (Unix time)
    std::map<int, long long> numbers_;

    // Synchronization for multi-threaded IPC requests
    mutable std::mutex mutex_;
};
