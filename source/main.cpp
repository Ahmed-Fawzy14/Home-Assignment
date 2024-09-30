#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <filesystem>

namespace fs = std::filesystem;
using namespace std;

// Struct to represent each file and its current value
struct FileEntry {
    ifstream file;      // Input file stream for reading numbers
    int currentValue;   // Current value read from the file
    string fileName;    // Name of the file

    // Constructor to open the file and read the first number
    FileEntry(const string& filePath, const string& name) : file(filePath), fileName(name) {
        if (!(file >> currentValue)) {
            // Throw an error if the file is empty
            throw runtime_error("File " + fileName + " is empty.");
        } else {
            // Log the initial value read from the file
            cout << "Initialized with first value " << currentValue << " from " << fileName << ".\n";
        }
    }
};

// Function to merge all sorted files into a single sorted output file
void mergeFiles(const string& directoryPath, const string& outputFilePath) {
    // LAmbda function for the priority queue (min-heap)
    auto compare = [](FileEntry* a, FileEntry* b) {
        return a->currentValue > b->currentValue;
    };

    // Min-heap to store FileEntry pointers based on currentValue
    priority_queue<FileEntry*, vector<FileEntry*>, decltype(compare)> minHeap(compare);

    cout << "Opening files and initializing min-heap...\n";

    // Iterate over all files in the specified directory
    for (const auto& entry : fs::directory_iterator(directoryPath)) {
        if (entry.is_regular_file()) {
            string filePath = entry.path().string();
            string fileName = entry.path().filename().string();
            try {
                // Create a FileEntry for each file and add it to the heap
                FileEntry* fe = new FileEntry(filePath, fileName);
                minHeap.push(fe);
                cout << "Added " << fileName << " to the heap with initial value " << fe->currentValue << ".\n";
            } catch (const exception& e) {
                // Handle empty files
                cout << e.what() << "\n";
            }
        }
    }

    // Open the output file for writing the merged result
    ofstream outputFile(outputFilePath);
    if (!outputFile.is_open()) {
        cerr << "Error opening output file.\n";
        return;
    }

    cout << "Starting merge process...\n";

    // Continue until all files have been fully processed
    while (!minHeap.empty()) {
        // Get the FileEntry with the smallest currentValue
        FileEntry* fe = minHeap.top();
        minHeap.pop();

        // Write the smallest value to the output file
        cout << "Writing " << fe->currentValue << " from " << fe->fileName << " to output file.\n";
        outputFile << fe->currentValue << '\n';

        int tempValue;
        // Read the next number from the same file
        if (fe->file >> tempValue) {
            // Update currentValue and re-add the FileEntry to the heap
            fe->currentValue = tempValue;
            minHeap.push(fe);
            cout << "Read next value " << fe->currentValue << " from " << fe->fileName << " and added back to heap.\n";
        } else {
            // Close the file and delete the FileEntry if no more numbers
            fe->file.close();
            cout << "No more data in " << fe->fileName << ". Closed file.\n";
            delete fe;
        }
    }

    // Close the output file after merging
    outputFile.close();

    cout << "Merge process completed. Output written to " << outputFilePath << ".\n";
}

int main() {
    string directoryPath = "TestFiles_AhmedAbdelkader";  // Directory containing the input files
    string outputFilePath = "merged_output.txt";         // Path for the output file

    mergeFiles(directoryPath, outputFilePath);

    return 0;
}
