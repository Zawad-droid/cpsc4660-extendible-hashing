#pragma once
#include <cstddef>
#include <vector>
#include <memory>
#include <cstdint>

/// A key-value pair stored in a hash bucket.
struct Record{
    int key;
    int value;
};

/// A bucket with a fixed record capacity and its own local depth.
class Bucket{
    friend class ExtendibleHash;
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
    int getLocalDepth() const{
        return localDepth;
    }

};

class ExtendibleHash {
private:
    int globalDepth;
    std::size_t bucketCapacity;
    std::vector<std::shared_ptr<Bucket>> directory;
    static std::uint32_t pseudokey(int key);
    void doubleDirectory();
    void splitBucket(std::size_t directoryIndex);

public:
    ExtendibleHash(std::size_t bucketCapacity);

    std::size_t getDirectoryIndex(int key) const;

    bool find(int key, int& value) const;

    bool insert(const Record& r);

    int getGlobalDepth() const {
        return globalDepth;
    }

    std::size_t getDirectorySize() const {
        return directory.size();
    }

    const Bucket& getBucket(std::size_t index) const {
        return *directory.at(index);
    }
};

