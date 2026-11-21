#include "extendible_hash.h"
#include <stdexcept>
#include <utility>

Bucket::Bucket(int localDepth, std::size_t capacity){
    this->localDepth = localDepth;
    this->capacity = capacity;
}

bool Bucket::isFull() const{
    return records.size() >= capacity;
}

bool Bucket::find(int key, int& value) const{
    for(const auto& i : records){
        if(i.key == key){
            value = i.value;
            return true;
        }
    }
    return false;
}

bool Bucket::insert(const Record& r){
   for(auto& record : records){
        if(record.key == r.key){
            record.value = r.value;
            return true;
        }
   }
   if(isFull()){
    return false;
   }
   records.push_back(r);
   return true;
}



ExtendibleHash::ExtendibleHash(std::size_t bucketCapacity)
    : globalDepth(1), bucketCapacity(bucketCapacity) {

    if (bucketCapacity == 0) {
        throw std::invalid_argument(
            "Bucket capacity must be positive"
        );
    }

    directory.push_back(
        std::make_shared<Bucket>(globalDepth, bucketCapacity)
    );

    directory.push_back(
        std::make_shared<Bucket>(globalDepth, bucketCapacity)
    );
}

std::uint32_t ExtendibleHash::pseudokey(int key) {
    std::uint32_t x = static_cast<std::uint32_t>(key);

    x ^= x >> 16;
    x *= 0x85ebca6bu;
    x ^= x >> 13;
    x *= 0xc2b2ae35u;
    x ^= x >> 16;

    return x;
}

std::size_t ExtendibleHash::getDirectoryIndex(int key) const {
    if (globalDepth < 1 || globalDepth > 32) {
        throw std::logic_error("Invalid global depth");
    }

    std::uint32_t hash = pseudokey(key);

    return static_cast<std::size_t>(
        hash >> (32 - globalDepth)
    );
}


bool ExtendibleHash::find(int key, int& value) const {
    std::size_t index = getDirectoryIndex(key);

    return directory.at(index)->find(key, value);
}



void ExtendibleHash::doubleDirectory(
    std::vector<std::shared_ptr<Bucket>>& expanded) const {

    if (globalDepth >= 32 ||
        directory.size() > directory.max_size() / 2) {
        throw std::overflow_error(
            "Cannot grow directory further"
        );
    }

    expanded.reserve(directory.size() * 2);

    for (const auto& bucket : directory) {
        expanded.push_back(bucket);
        expanded.push_back(bucket);
    }
}


void ExtendibleHash::splitBucket(std::size_t directoryIndex) {
    auto original = directory.at(directoryIndex);
    const int originalDepth = original->localDepth;

    if (originalDepth < 1 || originalDepth > globalDepth) {
        throw std::logic_error("Invalid local depth");
    }

    if (originalDepth == 32) {
        throw std::overflow_error("Pseudokey bits exhausted");
    }

    const bool requiresDoubling =
        (originalDepth == globalDepth);

    const int newGlobalDepth =
        globalDepth + (requiresDoubling ? 1 : 0);

    const int newLocalDepth = originalDepth + 1;

    const std::size_t originalPrefix =
        directoryIndex >> (globalDepth - originalDepth);

    auto sibling = std::make_shared<Bucket>(
        newLocalDepth, bucketCapacity
    );

    // Prepare directory expansion without modifying the live table.
    std::vector<std::shared_ptr<Bucket>> expanded;

    if (requiresDoubling) {
        doubleDirectory(expanded);
    }

    // Prepare redistributed records before making any changes.
    std::vector<Record> retained;
    std::vector<Record> moved;

    retained.reserve(original->records.size());
    moved.reserve(original->records.size());

    for (const Record& record : original->records) {
        std::uint32_t hash = pseudokey(record.key);

        if ((hash >> (32 - originalDepth)) != originalPrefix) {
            throw std::logic_error(
                "Record not in its expected bucket"
            );
        }

        if (((hash >> (32 - newLocalDepth)) & 1U) == 0) {
            retained.push_back(record);
        } else {
            moved.push_back(record);
        }
    }

    // Determine which directory entries belong to each child.
    const std::size_t remainingBits =
        static_cast<std::size_t>(
            newGlobalDepth - newLocalDepth
        );

    const std::size_t childSpan =
        std::size_t{1} << remainingBits;

    const std::size_t firstChildStart =
        (originalPrefix << 1) << remainingBits;

    const std::size_t siblingStart =
        firstChildStart + childSpan;

    const auto& preparedDirectory =
        requiresDoubling ? expanded : directory;

    // Verify directory pointers before committing the split.
    for (std::size_t i = firstChildStart;
         i < siblingStart + childSpan; ++i) {

        if (preparedDirectory.at(i) != original) {
            throw std::logic_error(
                "Inconsistent bucket directory"
            );
        }
    }

    // Commit after all allocations and validation have succeeded.
    original->records.swap(retained);
    sibling->records.swap(moved);

    original->localDepth = newLocalDepth;

    auto& destinationDirectory =
        requiresDoubling ? expanded : directory;

    for (std::size_t i = siblingStart;
         i < siblingStart + childSpan; ++i) {

        destinationDirectory[i] = sibling;
    }

    if (requiresDoubling) {
        directory.swap(expanded);
        ++globalDepth;
    }
}


bool ExtendibleHash::insert(const Record& record) {
    while (true) {
        std::size_t index = getDirectoryIndex(record.key);
        auto bucket = directory.at(index);

        if (bucket->insert(record)) {
            return true;
        }

        splitBucket(index);
    }
}
