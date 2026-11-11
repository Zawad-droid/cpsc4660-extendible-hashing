#pragma once
#include <cstddef>
#include <vector>

/// A key-value pair stored in a hash bucket.
struct Record{
    int key;
    int value;
};

/// A bucket with a fixed record capacity and its own local depth.
class Bucket{
private:
    // Number of hash bits used to distinguish this bucket.
    int localDepth;
    // Maximum number of records this bucket can hold.
    std::size_t capacity;
    // Records currently stored in this bucket.
    std::vector<Record> records;
public:
    /// Creates an empty bucket with the given local depth and record capacity.
    Bucket(int localDepth, std::size_t capacity);

    /// Returns true when the record count has reached or exceeded capacity.
    bool isFull() const;

    /// Looks up a key, writing its associated value to value when found.
    /// Returns true if the key is found, or false otherwise.
    bool find(int key, int& value) const;

    /// Attempts to store a record; returns true on success or false on failure.
    bool insert(const Record& r);

};
