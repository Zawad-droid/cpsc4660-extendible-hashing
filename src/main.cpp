#include <iostream>
#include "extendible_hash.h"
#include <initializer_list>
#include <stdexcept>
#include <algorithm>
#include <limits>
#include <random>
#include <unordered_map>
#include <unordered_set>
#include <vector>


int main() {
    Bucket bucket(1, 3);  // localDepth = 1, capacity = 3

    int value = 0;

    // Test 1: New bucket should be empty
    std::cout << "Test 1 - Initially not full: "
              << (!bucket.isFull() ? "PASS" : "FAIL") << '\n';

    // Test 2: Insert three records
    bool a = bucket.insert({10, 100});
    bool b = bucket.insert({20, 200});
    bool c = bucket.insert({30, 300});

    std::cout << "Test 2 - Insert three records: "
              << (a && b && c ? "PASS" : "FAIL") << '\n';

    // Test 3: Bucket should now be full
    std::cout << "Test 3 - Bucket is full: "
              << (bucket.isFull() ? "PASS" : "FAIL") << '\n';

    // Test 4: Fourth distinct record should be rejected
    bool inserted = bucket.insert({40, 400});

    std::cout << "Test 4 - Reject insertion when full: "
              << (!inserted ? "PASS" : "FAIL") << '\n';

    // Test 5: Find existing key
    bool found = bucket.find(20, value);

    std::cout << "Test 5 - Find existing key: "
              << (found && value == 200 ? "PASS" : "FAIL") << '\n';

    // Test 6: Update existing key even when full
    bool updated = bucket.insert({20, 500});
    found = bucket.find(20, value);

    std::cout << "Test 6 - Update existing key: "
              << (updated && found && value == 500 ? "PASS" : "FAIL") << '\n';

    // Test 7: Find nonexistent key
    found = bucket.find(99, value);

    std::cout << "Test 7 - Missing key: "
              << (!found ? "PASS" : "FAIL") << '\n';

    // Test 8: Rejected insertion must not remove existing records
    bool found10 = bucket.find(10, value) && value == 100;
    bool found30 = bucket.find(30, value) && value == 300;

    std::cout << "Test 8 - Existing records preserved: "
              << (found10 && found30 ? "PASS" : "FAIL") << '\n';

    
    ExtendibleHash hashTable(3);

    std::cout << "Test 9 - Global depth: "
              << (hashTable.getGlobalDepth() == 1 ? "PASS" : "FAIL")
              << '\n';

    std::cout << "Test 10 - Directory size: "
              << (hashTable.getDirectorySize() == 2 ? "PASS" : "FAIL")
              << '\n';

    std::cout << "Test 11 - Separate buckets: "
              << (&hashTable.getBucket(0) != &hashTable.getBucket(1)
                      ? "PASS" : "FAIL")
              << '\n';

    std::cout << "Test 12 - Local depths: "
              << (hashTable.getBucket(0).getLocalDepth() == 1 &&
                  hashTable.getBucket(1).getLocalDepth() == 1
                      ? "PASS" : "FAIL")
              << '\n';

    // Test 13: Directory indices are within bounds
    bool validIndex = true;

    for (int key : {0, 1, 3, 25, -5, 100}) {
        if (hashTable.getDirectoryIndex(key) >=
            hashTable.getDirectorySize()) {
            validIndex = false;
        }
    }

    std::cout << "Test 13 - Valid directory index: "
              << (validIndex ? "PASS" : "FAIL") << '\n';

    // Test 14: Hashing is deterministic
    bool consistent =
        hashTable.getDirectoryIndex(42) ==
        hashTable.getDirectoryIndex(42);

    std::cout << "Test 14 - Consistent hashing: "
              << (consistent ? "PASS" : "FAIL") << '\n';

    // Test 15: Prefix-based directory indexing
    bool correctPrefix =
        hashTable.getDirectoryIndex(3) == 1 &&
        hashTable.getDirectoryIndex(25) == 0 &&
        hashTable.getDirectoryIndex(-5) == 1;

    std::cout << "Test 15 - Prefix-based indexing: "
              << (correctPrefix ? "PASS" : "FAIL") << '\n';


    // Test 16: Lookup in an empty hash table
    int lookupValue = -999;
    bool lookupFound = hashTable.find(25, lookupValue);

    std::cout << "Test 16 - Empty table lookup: "
              << (!lookupFound ? "PASS" : "FAIL")
              << '\n';

    // Test 17: Failed lookup must not modify output value
    std::cout << "Test 17 - Preserve value on failure: "
              << (lookupValue == -999 ? "PASS" : "FAIL")
              << '\n';

    // Test 18: Lookup several missing keys
    bool allMissing = true;

    for (int key : {-100, -5, 0, 1, 42, 100000}) {
        int result = -777;

        if (hashTable.find(key, result) || result != -777) {
            allMissing = false;
        }
    }

    std::cout << "Test 18 - Multiple missing keys: "
              << (allMissing ? "PASS" : "FAIL")
              << '\n';

    // Test 19: Insertion through the directory
    ExtendibleHash growTable(1);

    bool firstInsert = growTable.insert({0, 100});
    bool secondInsert = growTable.insert({2, 200});

    std::cout << "Test 19 - Directory insertion: "
              << (firstInsert && secondInsert ? "PASS" : "FAIL")
              << '\n';

    // Test 20: Repeated directory doubling
    std::cout << "Test 20 - Directory doubling: "
              << (growTable.getGlobalDepth() == 3 &&
                  growTable.getDirectorySize() == 8
                      ? "PASS" : "FAIL")
              << '\n';

    // Test 21: Existing records survive splitting
    int result = 0;

    bool preserved =
        growTable.find(0, result) && result == 100 &&
        growTable.find(2, result) && result == 200;

    std::cout << "Test 21 - Records preserved: "
              << (preserved ? "PASS" : "FAIL")
              << '\n';

    // Test 22: Unsplit buckets share directory pointers
    bool sharedPointers =
        &growTable.getBucket(6) ==
        &growTable.getBucket(7);

    std::cout << "Test 22 - Shared bucket pointers: "
              << (sharedPointers ? "PASS" : "FAIL")
              << '\n';

    // Test 23: Split without further directory doubling
    int depthBefore = growTable.getGlobalDepth();

    bool moreInserts =
        growTable.insert({3, 300}) &&
        growTable.insert({-3, 400});

    std::cout << "Test 23 - Split without doubling: "
              << (moreInserts &&
                  growTable.getGlobalDepth() == depthBefore
                      ? "PASS" : "FAIL")
              << '\n';

    // Test 24: All records remain accessible
    bool allFound =
        growTable.find(0, result) && result == 100 &&
        growTable.find(2, result) && result == 200 &&
        growTable.find(3, result) && result == 300 &&
        growTable.find(-3, result) && result == 400;

    std::cout << "Test 24 - All lookups after splits: "
              << (allFound ? "PASS" : "FAIL")
              << '\n';

    // Test 25: Updating a key must not cause splitting
    int depthBeforeUpdate = growTable.getGlobalDepth();

    bool updatedRecord =
        growTable.insert({0, 999}) &&
        growTable.find(0, result) &&
        result == 999;

    std::cout << "Test 25 - Update without splitting: "
              << (updatedRecord &&
                  growTable.getGlobalDepth() == depthBeforeUpdate
                      ? "PASS" : "FAIL")
              << '\n';

    // Test 26: Reject zero bucket capacity
    bool rejectedCapacity = false;

    try {
        ExtendibleHash invalidTable(0);
    } catch (const std::invalid_argument&) {
        rejectedCapacity = true;
    }

    std::cout << "Test 26 - Reject zero capacity: "
              << (rejectedCapacity ? "PASS" : "FAIL")
              << '\n';

    // Test 27: Verify many insertions and lookups
    ExtendibleHash bulkTable(4);
    bool bulkCorrect = true;

    for (int i = 0; i < 1000; ++i) {
        if (!bulkTable.insert({i, i * 3})) {
            bulkCorrect = false;
            break;
        }
    }

    for (int i = 0; i < 1000 && bulkCorrect; ++i) {
        int stored = 0;

        if (!bulkTable.find(i, stored) || stored != i * 3) {
            bulkCorrect = false;
        }
    }

    std::cout << "Test 27 - Bulk insert and lookup: "
              << (bulkCorrect ? "PASS" : "FAIL")
              << '\n';

    // Test 28: Verify an actual split without directory growth
    bool correctSplit =
        growTable.getGlobalDepth() == 3 &&
        &growTable.getBucket(4) != &growTable.getBucket(5) &&
        growTable.getBucket(4).getLocalDepth() == 3 &&
        growTable.getBucket(5).getLocalDepth() == 3 &&
        &growTable.getBucket(6) == &growTable.getBucket(7) &&
        growTable.getBucket(6).getLocalDepth() == 2;

    std::cout << "Test 28 - Split without directory doubling: "
              << (correctSplit ? "PASS" : "FAIL") << '\n';

    // Test 29: Check directory sharing and bucket capacity
    bool structureOK =
        bulkTable.getDirectorySize() ==
        (std::size_t{1} << bulkTable.getGlobalDepth());

    std::unordered_map<const Bucket*, std::size_t> pointerCounts;
    std::unordered_map<const Bucket*, std::size_t> prefixes;

    for (std::size_t i = 0;
         i < bulkTable.getDirectorySize() && structureOK; ++i) {

        const Bucket* b = &bulkTable.getBucket(i);
        const int depth = b->getLocalDepth();

        if (depth < 1 || depth > bulkTable.getGlobalDepth() ||
            b->getRecordCount() > 4) {
            structureOK = false;
            break;
        }

        const std::size_t prefix =
            i >> (bulkTable.getGlobalDepth() - depth);

        const auto inserted = prefixes.emplace(b, prefix);

        if (!inserted.second && inserted.first->second != prefix) {
            structureOK = false;
        }

        ++pointerCounts[b];
    }

    for (const auto& entry : pointerCounts) {
        const Bucket* b = entry.first;

        const std::size_t expectedReferences =
            std::size_t{1} <<
            (bulkTable.getGlobalDepth() - b->getLocalDepth());

        if (entry.second != expectedReferences) {
            structureOK = false;
        }
    }

    std::cout << "Test 29 - Directory and bucket invariants: "
              << (structureOK ? "PASS" : "FAIL") << '\n';

    // Test 30: Randomized insertion order with negative keys
    std::vector<int> keys;

    for (int key = -1500; key < 1500; ++key) {
        keys.push_back(key);
    }

    std::mt19937 rng(4660);
    std::shuffle(keys.begin(), keys.end(), rng);

    ExtendibleHash randomizedTable(3);
    bool randomOK = true;

    for (int key : keys) {
        if (!randomizedTable.insert({key, key * 3})) {
            randomOK = false;
        }
    }

    for (int key : keys) {
        int actual = 0;

        if (!randomizedTable.find(key, actual) ||
            actual != key * 3) {
            randomOK = false;
        }
    }

    std::cout << "Test 30 - Randomized insertion and lookup: "
              << (randomOK ? "PASS" : "FAIL") << '\n';

    // Test 31: Updates must not create duplicates or trigger splits
    const int previousDepth = randomizedTable.getGlobalDepth();
    const std::size_t previousSize =
        randomizedTable.getDirectorySize();

    bool updateOK = true;

    for (int key : keys) {
        if (!randomizedTable.insert({key, key * 5})) {
            updateOK = false;
        }
    }

    for (int key : keys) {
        int actual = 0;

        if (!randomizedTable.find(key, actual) ||
            actual != key * 5) {
            updateOK = false;
        }
    }

    std::unordered_set<const Bucket*> seenBuckets;
    std::size_t totalRecords = 0;

    for (std::size_t i = 0;
         i < randomizedTable.getDirectorySize(); ++i) {

        const Bucket* b = &randomizedTable.getBucket(i);

        if (seenBuckets.insert(b).second) {
            totalRecords += b->getRecordCount();
        }
    }

    updateOK = updateOK &&
               randomizedTable.getGlobalDepth() == previousDepth &&
               randomizedTable.getDirectorySize() == previousSize &&
               totalRecords == keys.size();

    std::cout << "Test 31 - Updates preserve structure and count: "
              << (updateOK ? "PASS" : "FAIL") << '\n';

    // Test 32: Integer boundary keys
    ExtendibleHash boundaryTable(2);

    const std::vector<int> specialKeys = {
        std::numeric_limits<int>::min(),
        std::numeric_limits<int>::max(),
        -1, 0, 1
    };

    bool boundaryOK = true;

    for (int key : specialKeys) {
        if (!boundaryTable.insert({key, key})) {
            boundaryOK = false;
        }
    }

    for (int key : specialKeys) {
        int actual = 0;

        if (!boundaryTable.find(key, actual) || actual != key) {
            boundaryOK = false;
        }
    }

    std::cout << "Test 32 - Integer boundary keys: "
              << (boundaryOK ? "PASS" : "FAIL") << '\n';

    return 0;
}