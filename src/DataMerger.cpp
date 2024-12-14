#include "DataMerger.h"
#include <iostream>
#include <algorithm>

bool MarketDataMerger::mergeFiles(const std::vector<std::string> &inputFiles, const std::string &outputFile) {
    std::ofstream ofs(outputFile);
    if (!ofs) {
        std::cerr << "Failed to open output file: " << outputFile << "\n";
        return false;
    }
    ofs << "Symbol, Timestamp, Price, Size, Exchange, Type\n";

    std::vector<std::unique_ptr<FileState>> fileStates;
    fileStates.reserve(inputFiles.size());

    // Initializing the file state for each input file
    for (const auto &f : inputFiles) {
        auto fs = std::make_unique<FileState>();
        fs->ifs.open(f);
        if (!fs->ifs) {
            std::cerr << "Failed to open input file: " << f << "\n";
            continue;
        }

        // Extracting symbol from file name
        std::string symbol = f;
        size_t pos = symbol.find_last_of("/\\");
        size_t start = (pos != std::string::npos) ? pos + 1 : 0;
        pos = symbol.find('.', start);
        if (pos != std::string::npos) {
            symbol = symbol.substr(start, pos - start);
        } else {
            symbol = symbol.substr(start);
        }
        fs->symbol = symbol;
        fileStates.push_back(std::move(fs));
    }

    // Defining priority queue with custom comparator
    std::priority_queue<
        std::pair<MarketDataLine*, FileState*>,
        std::vector<std::pair<MarketDataLine*, FileState*>>,
        LineComparator
    > pq;

    // Initializing the priority queue with the first line from each file
    for (auto &fs : fileStates) {
        std::string line;
        if (std::getline(fs->ifs, line)) {
            // Skipping the header
            if (line.find("Timestamp") != std::string::npos && !std::getline(fs->ifs, line)) {
                continue;
            }
            if (fs->currentLine.parseFromLine(line)) {
                if (fs->currentLine.symbol.empty()) {
                    fs->currentLine.symbol = fs->symbol;
                }
                pq.emplace(&fs->currentLine, fs.get());
            }
        }
    }

    // Merging process
    while (!pq.empty()) {
        auto [line, state] = pq.top();
        pq.pop();
        ofs << line->toOutputString() << "\n";

        std::string nextLine;
        if (std::getline(state->ifs, nextLine)) {
            if (state->currentLine.parseFromLine(nextLine)) {
                if (state->currentLine.symbol.empty()) {
                    state->currentLine.symbol = state->symbol;
                }
                pq.emplace(&state->currentLine, state);
            }
        }
    }

    return true;
}
