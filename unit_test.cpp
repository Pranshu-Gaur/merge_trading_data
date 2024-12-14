#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <filesystem>
#include <random>
#include <cmath>
#include <sstream>

namespace fs = std::filesystem;

int main() {
    std::string dataDir = "data";
    std::string outputFile = "MultiPlexedFile.txt";
    std::string analyticsFile = "analytics.txt";
    int numFiles = 10000;
    int linesPerFile = 1000;

    // Cleaning up old data
    fs::remove_all(dataDir);
    fs::create_directory(dataDir);
    fs::remove(outputFile);
    fs::remove(analyticsFile);

    // Generating dummy input files
    std::cout << "Generating " << numFiles << " dummy input files in " << dataDir << "...\n";
    {
        std::mt19937_64 rng(std::random_device{}());
        std::uniform_real_distribution<double> priceDist(20.0, 30.0);
        std::uniform_int_distribution<int> sizeDist(1, 100);

        for (int i = 1; i <= numFiles; ++i) {
            std::string fileName = dataDir + "/symbol" + std::to_string(i) + ".txt";
            std::ofstream ofs(fileName);
            ofs << "Timestamp, Price, Size, Exchange, Type\n";
            for (int j = 1; j <= linesPerFile; ++j) {
                std::stringstream line;
                line << "2021-03-05 10:00:00." << std::setw(3) << std::setfill('0') << j << ", ";
                double price = priceDist(rng);
                int sizeVal = sizeDist(rng);
                line << price << ", " << sizeVal << ", NYSE, Ask\n";
                ofs << line.str();
            }
        }
    }
    std::cout << "Data generation complete.\n";

    // Running market_data_merger and measuring time
    std::cout << "Running market_data_merger...\n";
    auto start = std::chrono::high_resolution_clock::now();
    int ret = std::system("market_data_merger");
    auto end = std::chrono::high_resolution_clock::now();

    if (ret != 0) {
        std::cerr << "market_data_merger execution failed with code " << ret << "\n";
        return 1;
    }
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();

    std::cout << "Collecting analytics...\n";

    // Total Input Lines
    int totalInputLines = numFiles * (linesPerFile + 1);
    
    uintmax_t totalInputSize = 0;

    // Getting total input size
    for (auto &entry : fs::recursive_directory_iterator(dataDir)) {
        if (entry.is_regular_file()) {
            totalInputSize += entry.file_size();
        }
    }

    std::string outputSizeStr = "N/A";
    int outputLines = numFiles * linesPerFile + 1;
    if (fs::exists(outputFile)) {
        auto sz = fs::file_size(outputFile);
        
        // Converting to readable format
        double sizeD = (double)sz;
        const char* units[] = {"B","K","M","G","T"};
        int unitIdx = 0;
        while (sizeD > 1024.0 && unitIdx < 4) {
            sizeD /= 1024.0;
            unitIdx++;
        }
        std::ostringstream oss;
        oss << std::round(sizeD*100)/100 << units[unitIdx];
        outputSizeStr = oss.str();
    }

    // Writing Analytics
    {
        std::ofstream analytics(analyticsFile);
        analytics << "Analytics Report\n";
        analytics << "================\n";
        analytics << "Number of input files: " << numFiles << "\n";
        analytics << "Lines per input file: " << linesPerFile << "\n";
        analytics << "Total input lines: " << totalInputLines << "\n";

        // Converting totalInputSize to readable format:
        double sizeD = (double)totalInputSize;
        const char* units[] = {"B","K","M","G","T"};
        int unitIdx = 0;
        while (sizeD > 1024.0 && unitIdx < 4) {
            sizeD /= 1024.0;
            unitIdx++;
        }
        std::ostringstream sizeOss;
        sizeOss << std::round(sizeD*100)/100 << units[unitIdx];
        std::string totalInputSizeStr = sizeOss.str();

        analytics << "Total input size on disk: " << totalInputSizeStr << "\n\n";
        analytics << "Execution time: " << elapsed << " seconds\n";
        analytics << "Output file: " << outputFile << "\n";
        analytics << "Output file size: " << outputSizeStr << "\n";
        analytics << "Output file line count: " << outputLines << "\n";
    }

    std::cout << "Analytics written to " << analyticsFile << "\n";

    return 0;
}
