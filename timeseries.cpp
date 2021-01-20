#include "timeseries.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <string>

TimeSeries::TimeSeries(const char *CSVfileName) {
    // Open the given CSV file
    ifstream csv_file(CSVfileName);

    // Creat a vector that will containing the CSV file data
    std::vector<std::vector<float>> table;

    // Insert the data to vector of vectors
    std::string line;
    std::vector<std::string> names;

    std::getline(csv_file, line);
    std::istringstream stream(line);
    std::string cell;

    // reading names
    while (std::getline(stream, cell, ',')) {
        names.push_back(cell);
    }
    
    // resize tables according to names
    table.resize(names.size());

    // reading data
    while (std::getline(csv_file, line)) {
        auto column = table.begin();
        std::istringstream stream(line);

        while (std::getline(stream, cell, ',')) {
            column->push_back(stof(cell));
            ++column;
        }
    }

    std::vector<std::vector<float>> result(table[0].size(),
                                           std::vector<float>(table.size()));
    for (std::vector<int>::size_type i = 0; i < table[0].size(); i++) {
        for (std::vector<int>::size_type j = 0; j < table.size(); j++) {
            result[i][j] = table[j][i];
        }
    }

    m_table = table;
    m_names = names;
}
