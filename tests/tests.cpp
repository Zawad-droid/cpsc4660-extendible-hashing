
#include "extendible_hash.h"
#include <algorithm>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <unordered_map>
#include <vector>

// Fail the test if a required condition is false.
void require(bool ok, const char* message) {
    if (!ok) {
        throw std::runtime_error(message);
    }
}

// Verify directory size, bucket depths, sharing, and record counts.
void verifyStructure(const ExtendibleHash& table,
                     std::size_t capacity,
                     std::size_t expectedRecords) {

    const int globalDepth = table.getGlobalDepth();

    require(globalDepth >= 1 && globalDepth <= 32,
            "Invalid global depth");

    require(table.getDirectorySize() ==
            (std::size_t{1} << globalDepth),
            "Directory size does not match global depth");

    struct BucketInfo {
        std::size_t prefix;
        std::size_t references;
    };

    std::unordered_map<const Bucket*, BucketInfo> seen;
    std::size_t recordCount = 0;

    for (std::size_t i = 0;
         i < table.getDirectorySize(); ++i) {

        const Bucket& bucket = table.getBucket(i);
        const int depth = bucket.getLocalDepth();

        require(depth >= 1 && depth <= globalDepth,
                "Invalid local depth");

        require(bucket.getRecordCount() <= capacity,
                "Bucket over capacity");

        std::size_t prefix = i >> (globalDepth - depth);

        auto [it, first] = seen.emplace(
            &bucket, BucketInfo{prefix, 0}
        );

        if (first) {
            recordCount += bucket.getRecordCount();
        } else {
            require(it->second.prefix == prefix,
                    "Bucket prefix mismatch");
        }

        ++it->second.references;
    }

    for (const auto& [bucket, info] : seen) {
        std::size_t expectedReferences =
            std::size_t{1} <<
            (globalDepth - bucket->getLocalDepth());

        require(info.references == expectedReferences,
                "Incorrect number of directory pointers");
    }

    require(recordCount == expectedRecords,
            "Record count mismatch");
}

int main() {
    try {
        ExtendibleHash table(4);

        // Integration 1: Empty table
        int value = 12345;

        require(!table.find(5, value) && value == 12345,
                "Empty lookup should preserve output");

        verifyStructure(table, 4, 0);

        std::cout << "Integration 1 - Empty table: PASS\n";

        // Integration 2: Insert, split, grow, and lookup
        std::vector<int> keys;

        for (int key = -1000; key < 1000; ++key) {
            keys.push_back(key);
        }

        std::mt19937 rng(4660);
        std::shuffle(keys.begin(), keys.end(), rng);

        for (std::size_t i = 0; i < keys.size(); ++i) {
            require(table.insert({keys[i], keys[i] * 11}),
                    "Insert failed");

            if ((i + 1) % 257 == 0) {
                verifyStructure(table, 4, i + 1);
            }
        }

        for (int key : keys) {
            require(table.find(key, value) &&
                    value == key * 11,
                    "Inserted record missing or incorrect");
        }

        verifyStructure(table, 4, keys.size());

        require(table.getGlobalDepth() > 1,
                "Expected directory growth");

        std::cout
            << "Integration 2 - Insert, split, grow, lookup: PASS\n";

        // Integration 3: Updates without directory growth
        const int depthBefore = table.getGlobalDepth();
        const std::size_t sizeBefore =
            table.getDirectorySize();

        for (int key : keys) {
            if (key % 2 == 0) {
                require(table.insert({key, key * 17}),
                        "Update failed");
            }
        }

        for (int key : keys) {
            int expected =
                (key % 2 == 0) ? key * 17 : key * 11;

            require(table.find(key, value) &&
                    value == expected,
                    "Updated record incorrect");
        }

        require(table.getGlobalDepth() == depthBefore &&
                table.getDirectorySize() == sizeBefore,
                "Updates unexpectedly changed directory");

        verifyStructure(table, 4, keys.size());

        std::cout
            << "Integration 3 - Updates without growth: PASS\n";

        // Integration 4: Missing and boundary keys
        value = 321;

        require(!table.find(-10000, value) && value == 321,
                "Missing negative key returned incorrectly");

        require(!table.find(10000, value) && value == 321,
                "Missing positive key returned incorrectly");

        ExtendibleHash boundary(2);

        require(boundary.insert({
                    std::numeric_limits<int>::min(), 7
                }),
                "INT_MIN insertion failed");

        require(boundary.insert({
                    std::numeric_limits<int>::max(), 8
                }),
                "INT_MAX insertion failed");

        require(boundary.find(
                    std::numeric_limits<int>::min(), value
                ) && value == 7,
                "INT_MIN lookup failed");

        require(boundary.find(
                    std::numeric_limits<int>::max(), value
                ) && value == 8,
                "INT_MAX lookup failed");

        verifyStructure(boundary, 2, 2);

        std::cout
            << "Integration 4 - Missing and boundary keys: PASS\n";

        // Integration 5: Independent hash tables
        ExtendibleHash left(1);
        ExtendibleHash right(2);

        require(left.insert({42, 100}) &&
                right.insert({42, 200}),
                "Independent inserts failed");

        require(left.insert({42, 300}),
                "Independent update failed");

        require(left.find(42, value) && value == 300,
                "Left table incorrect");

        require(right.find(42, value) && value == 200,
                "Right table incorrectly modified");

        verifyStructure(left, 1, 1);
        verifyStructure(right, 2, 1);

        std::cout
            << "Integration 5 - Independent instances: PASS\n";

        std::cout << "All 5 integration tests passed.\n";
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Integration test FAILED: "
                  << e.what() << '\n';
        return 1;
    }
}
