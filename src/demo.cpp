
#include "extendible_hash.h"
#include <iostream>
#include <vector>

void printState(const ExtendibleHash& table,
                const std::vector<int>& insertedKeys) {
    std::cout << "\nGlobal depth: "
              << table.getGlobalDepth() << '\n';
    std::cout << "Directory size: "
              << table.getDirectorySize() << '\n';
    std::cout << "Bucket splits: "
              << table.getSplitCount() << '\n';
    std::cout << "Directory doublings: "
              << table.getDoublingCount() << '\n';

    for (std::size_t i = 0; i < table.getDirectorySize(); ++i) {
        const Bucket& bucket = table.getBucket(i);

        std::cout << "Directory[" << i << "]"
                  << " -> local depth " << bucket.getLocalDepth()
                  << ", keys: ";

        bool empty = true;

        for (int key : insertedKeys) {
            const Bucket& selected =
                table.getBucket(table.getDirectoryIndex(key));

            if (&selected == &bucket) {
                std::cout << key << " ";
                empty = false;
            }
        }

        if (empty) std::cout << "(empty)";
        std::cout << '\n';
    }
}

int main() {
    ExtendibleHash table(2);

    table.insert({0, 100});
    table.insert({2, 200});

    std::cout << "BEFORE INSERTING KEY 1\n";
    printState(table, {0, 2});

    table.insert({1, 300});

    std::cout << "\nAFTER INSERTING KEY 1\n";
    printState(table, {0, 2, 1});

    std::cout << "\nVERIFYING LOOKUPS\n";

    for (int key : {0, 2, 1}) {
        int value = 0;

        if (!table.find(key, value)) {
            std::cerr << "Lookup failed for key " << key << '\n';
            return 1;
        }

        std::cout << "Key " << key
                  << " -> Value " << value << '\n';
    }

    return 0;
}
