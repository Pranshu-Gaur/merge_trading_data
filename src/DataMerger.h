#ifndef MARKETDATAMERGER_H
#define MARKETDATAMERGER_H

#include "DataLine.h"
#include <string>
#include <vector>
#include <queue>
#include <fstream>
#include <memory>

class MarketDataMerger {
public:
    // Definition of the Merge Files Function
    bool mergeFiles(const std::vector<std::string> &inputFiles, const std::string &outputFile);

private:
    // Holding the state of each file being merged
    struct FileState {
        std::ifstream ifs;
        MarketDataLine currentLine;
        std::string symbol;
    };

    // Comparator for the priority queue to sortthe lines by timestamp and symbol
    struct LineComparator {
        bool operator()(const std::pair<MarketDataLine*, FileState*> &lhs, const std::pair<MarketDataLine*, FileState*> &rhs) const {
            if (lhs.first->timestamp != rhs.first->timestamp)
                return lhs.first->timestamp > rhs.first->timestamp;
            return lhs.first->symbol > rhs.first->symbol;
        }
    };
};

#endif
