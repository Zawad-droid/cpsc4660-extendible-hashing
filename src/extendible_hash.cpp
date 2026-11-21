#include "extendible_hash.h"
#include <stdexcept>

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
