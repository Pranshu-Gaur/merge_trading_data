#ifndef MARKETDATALINE_H
#define MARKETDATALINE_H

#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

// Helper function to trim leading and trailing spaces
inline std::string trim(const std::string &s) {
    size_t start = s.find_first_not_of(" \t");
    if (start == std::string::npos)
        return "";
    size_t end = s.find_last_not_of(" \t");
    return s.substr(start, end - start + 1);
}

struct MarketDataLine {
    std::string symbol;
    std::string timestamp;
    double price;
    int size;
    std::string exchange;
    std::string type;

    // Parsing a CSV line and populating the struct fields
    bool parseFromLine(const std::string &line) {
        std::vector<std::string> parts;
        std::stringstream ss(line);
        std::string token;

        while (std::getline(ss, token, ',')) {
            parts.emplace_back(trim(token));
        }

        if (parts.size() == 6) {
            // Format: Symbol, Timestamp, Price, Size, Exchange, Type
            symbol = parts[0];
            timestamp = parts[1];
            price = std::stod(parts[2]);
            size = std::stoi(parts[3]);
            exchange = parts[4];
            type = parts[5];
        } else if (parts.size() == 5) {
            // Format: Timestamp, Price, Size, Exchange, Type
            symbol.clear(); // Symbol to be set externally
            timestamp = parts[0];
            price = std::stod(parts[1]);
            size = std::stoi(parts[2]);
            exchange = parts[3];
            type = parts[4];
        } else {
            return false; // Invalid format
        }

        return true;
    }

    // Formatting the struct fields into a CSV line
    std::string toOutputString() const {
        std::ostringstream oss;
        oss << symbol << ", "
            << timestamp << ", "
            << price << ", "
            << size << ", "
            << exchange << ", "
            << type;
        return oss.str();
    }
};

#endif
