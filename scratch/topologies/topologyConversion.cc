#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <algorithm>

int main() {
    // Open the input file
    std::ifstream inputFile("input.txt");

    // Open the output file
    std::ofstream outputFile("output.txt");

    // Check if the input file is open
    if (!inputFile.is_open()) {
        std::cerr << "Error: Unable to open input file." << std::endl;
        return 1;
    }

    // Check if the output file is open
    if (!outputFile.is_open()) {
        std::cerr << "Error: Unable to open output file." << std::endl;
        return 1;
    }

    // Set to store unique node IDs
    std::set<int> uniqueNodeIds;

    // Process each line in the input file
    std::string line;
    while (std::getline(inputFile, line)) {
        // Skip lines starting with '#'
        if (!line.empty() && (line[0] == '#' || line[0] == 'T' || line[0] == 'M')) {
            continue;
        }

        std::istringstream tokenStream(line);
        std::vector<std::string> tokens;
        std::string token;
        while (std::getline(tokenStream, token, '\t')) {
            tokens.push_back(token);
        }

        // Extract node IDs from the link information and add them to the set
        int node1 = std::stoi(tokens[1]);
        int node2 = std::stoi(tokens[2]);
        uniqueNodeIds.insert(node1);
        uniqueNodeIds.insert(node2);
    }

    // Write the header line with the number of nodes and links to the output file
    outputFile << uniqueNodeIds.size() << " " << std::max(0, static_cast<int>(uniqueNodeIds.size()) - 1) << std::endl;

    // Reset the input file's position to the beginning
    inputFile.clear();
    inputFile.seekg(0, std::ios::beg);

    // Process each line in the input file
    while (std::getline(inputFile, line)) {
        // Skip lines starting with '#'
        if (!line.empty() && (line[0] == '#' || line[0] == 'T' || line[0] == 'M')) {
            continue;
        }

        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(line);
        while (std::getline(tokenStream, token, '\t')) {
            tokens.push_back(token);
            // outputFile << token << " ";
        }
        outputFile << tokens[1] << " " << tokens[2] << std::endl;
    }

    // Close the input and output files
    inputFile.close();
    outputFile.close();

    std::cout << "Conversion completed successfully." << std::endl;

    return 0;
}
