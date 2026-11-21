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


void ExtendibleHash::doubleDirectory() {
    if (globalDepth >= 32 ||
        directory.size() > directory.max_size() / 2) {
        throw std::overflow_error(
            "Cannot grow directory further"
        );
    }

    std::vector<std::shared_ptr<Bucket>> expanded;
    expanded.reserve(directory.size() * 2);

    for (const auto& bucket : directory) {
        expanded.push_back(bucket);
        expanded.push_back(bucket);
    }

    directory.swap(expanded);
    ++globalDepth;
}


void ExtendibleHash::splitBucket(std::size_t directoryIndex) {
    auto oldBucket = directory.at(directoryIndex);
    int oldDepth = oldBucket->localDepth;

    if (oldDepth >= 32) {
        throw std::overflow_error(
            "Pseudokey bits exhausted"
        );
    }

    if (oldDepth == globalDepth) {
        doubleDirectory();
    }

    auto newBucket = std::make_shared<Bucket>(
        oldDepth + 1, bucketCapacity
    );

    oldBucket->localDepth = oldDepth + 1;

    std::size_t shift = static_cast<std::size_t>(
        globalDepth - (oldDepth + 1)
    );

    for (std::size_t i = 0; i < directory.size(); ++i) {
        if (directory[i] == oldBucket &&
            ((i >> shift) & 1U)) {
            directory[i] = newBucket;
        }
    }

    std::vector<Record> previous =
        std::move(oldBucket->records);

    oldBucket->records.clear();

    for (const Record& record : previous) {
        std::size_t index = getDirectoryIndex(record.key);

        if (!directory.at(index)->insert(record)) {
            throw std::logic_error(
                "Failed to redistribute record"
            );
        }
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
