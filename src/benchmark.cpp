#include "extendible_hash.h"
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <numeric>
#include <random>
#include <stdexcept>
#include <unordered_set>
#include <vector>

int main() {
    using Clock = std::chrono::steady_clock;

    const std::vector<int> sizes = {1000, 10000, 50000, 100000};
    const std::vector<std::size_t> capacities = {4, 16, 64};

    std::ofstream csv("benchmark_results.csv");
    if (!csv) {
        std::cerr << "Could not create benchmark_results.csv\n";
        return 1;
    }

    const char* header =
        "records,capacity,insertion_ms,lookup_ms,splits,doublings,"
        "buckets,directory_size,occupancy_percent";
    csv << header << '\n';
    std::cout << header << '\n';

    for (int n : sizes) {
        std::vector<int> keys(static_cast<std::size_t>(n));
        std::iota(keys.begin(), keys.end(), -n / 2);
        std::mt19937 rng(4660);
        std::shuffle(keys.begin(), keys.end(), rng);

        for (std::size_t capacity : capacities) {
            ExtendibleHash table(capacity);

            auto start = Clock::now();
            for (int key : keys) {
                if (!table.insert({key, key * 3})) {
                    throw std::runtime_error("Insertion failed");
                }
            }
            auto afterInsert = Clock::now();

            for (int key : keys) {
                int value = 0;
                if (!table.find(key, value) || value != key * 3) {
                    throw std::runtime_error("Lookup gave wrong result");
                }
            }
            auto afterLookup = Clock::now();

            std::unordered_set<const Bucket*> uniqueBuckets;
            std::size_t recordsStored = 0;
            for (std::size_t i = 0; i < table.getDirectorySize(); ++i) {
                const Bucket* bucket = &table.getBucket(i);
                if (uniqueBuckets.insert(bucket).second) {
                    recordsStored += bucket->getRecordCount();
                }
            }
            if (recordsStored != keys.size()) {
                throw std::runtime_error("Incorrect record count");
            }

            const double insertionMs =
                std::chrono::duration<double, std::milli>(
                    afterInsert - start).count();
            const double lookupMs =
                std::chrono::duration<double, std::milli>(
                    afterLookup - afterInsert).count();
            const double occupancy =
                100.0 * static_cast<double>(recordsStored) /
                static_cast<double>(uniqueBuckets.size() * capacity);

            auto writeRow = [&](std::ostream& out) {
                out << n << ',' << capacity << ','
                    << insertionMs << ',' << lookupMs << ','
                    << table.getSplitCount() << ','
                    << table.getDoublingCount() << ','
                    << uniqueBuckets.size() << ','
                    << table.getDirectorySize() << ','
                    << occupancy << '\n';
            };
            writeRow(csv);
            writeRow(std::cout);
        }
    }

    std::cout << "Results saved to benchmark_results.csv\n";
    return 0;
}
