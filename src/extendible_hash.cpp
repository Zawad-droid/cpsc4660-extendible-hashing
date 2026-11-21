#include "extendible_hash.h"

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
