#include <iostream>
#include "extendible_hash.h"
#include <initializer_list>

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


    return 0;
}