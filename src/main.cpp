#include "DataMerger.h"
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <algorithm>
#include <sstream>

int main() {
    const std::string dataDir = "data";
    const std::string finalOutput = "MultiPlexedFile.txt";
    size_t MAX_OPEN_FILES = 96;

    // Collecting all .txt files from the data directory
    std::vector<std::string> inputFiles;
    for (const auto& entry : std::filesystem::directory_iterator(dataDir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
            inputFiles.emplace_back(entry.path().string());
        }
    }

    if (inputFiles.empty()) {
        std::cerr << "No input .txt files found in directory: " << dataDir << "\n";
        return 1;
    }

    MarketDataMerger merger;

    // Function to split files into batches
    auto batchFiles = [&](const std::vector<std::string> &files, size_t batchSize) -> std::vector<std::vector<std::string>> {
        std::vector<std::vector<std::string>> batches;
        for (size_t i = 0; i < files.size(); i += batchSize) {
            size_t end = std::min(files.size(), i + batchSize);
            batches.emplace_back(files.begin() + i, files.begin() + end);
        }
        return batches;
    };

    // Directly merging if total files within the limit
    if (inputFiles.size() <= MAX_OPEN_FILES) {
        if (!merger.mergeFiles(inputFiles, finalOutput)) {
            std::cerr << "Merging failed.\n";
            return 1;
        }
        std::cout << "Merging completed successfully. \nOutput: " << finalOutput << "\n\n";
        return 0;
    }

    size_t pass = 0;
    std::vector<std::string> currentFiles = inputFiles;
    std::vector<std::string> previousPassFiles;

    while (currentFiles.size() > 1) {
        auto batches = batchFiles(currentFiles, MAX_OPEN_FILES);
        std::vector<std::string> nextFiles;

        size_t batchIndex = 0;
        for (const auto &batch : batches) {
            std::ostringstream ss;
            ss << "intermediate_pass" << pass << "_batch" << batchIndex++ << ".txt";
            std::string intermediateFile = ss.str();

            if (!merger.mergeFiles(batch, intermediateFile)) {
                std::cerr << "Merging batch " << batchIndex - 1 << " failed.\n";
                return 1;
            }

            nextFiles.emplace_back(std::move(intermediateFile));
        }
        previousPassFiles = currentFiles;
        currentFiles = nextFiles;
        // As files get larger in size, we reduce maximum number of files open
        MAX_OPEN_FILES < 24 ? MAX_OPEN_FILES = 4 :  MAX_OPEN_FILES /= 6; 

        // Deleting intermediate files from the previous pass
        if (pass > 0) {
            size_t files = 0;
            for (const auto &file : previousPassFiles) {
                if (file.find("_batch") != std::string::npos) {
                    try {
                        std::filesystem::remove(file);
                        files++;
                    } catch (const std::filesystem::filesystem_error &e) {
                        std::cerr << "Error deleting file " << file << ": " << e.what() << "\n";
                    }
                }
            }
            std::cout << "Deleted " << files << " out of " << previousPassFiles.size() << " intermediate files. " << "\n";
        }
        pass++;

        std::cout << "Completed pass " << pass << ", resulting in " << currentFiles.size() << " intermediate files.\n";
    }

    // Renaming the final merged file
    if (currentFiles.size() == 1) {
        try {
            std::filesystem::rename(currentFiles[0], finalOutput);
            std::cout << "Merging completed successfully. \nOutput: " << finalOutput << "\n";
        } catch (const std::filesystem::filesystem_error &e) {
            std::cerr << "\n" << "Error renaming file " << currentFiles[0] << " to " << finalOutput << ": " << e.what() << "\n";
            return 1;
        }
    } else {
        std::cerr << "\n" << "Unexpected error: more than one file remained after merging.\n";
        return 1;
    }

    return 0;
}
