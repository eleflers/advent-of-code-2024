#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <sstream>

using namespace std;

class File {
    public:
        long long id;
        long long startingPos;
        long long length;
};

class FileSystem {
    public:
        vector<File> files;
        long long startNextFile;
        void addFile(long long id, long long length);
        void addEmptySpace(long long length);
        long long findNextEmptySpace(long long length);
        void moveFile();
};

int main(int argc, char* argv[])
{
    cout << "--\nAoC Day 9\n--\n";
    // For testing
    int debuglimit = 1;
    long long debug = 0;
    bool debugapply = false;

    // Part 1: 6323378990915 - incorrect, too small
    // Part 1: 6323641412437 - correct

    // User args
    string filename = "input";
    if (argc == 1) {
        cout << "Assume default input file '" << filename << "'\n";
    }
    else if (argc > 1) {
        filename = argv[1];
        cout << "Taking CLI input file name '" << filename << "'\n";
    }
    if (argc > 2) {
        cout << "Reading 2nd input param\n    -d / --debug for debug printing\n";
        if (string{argv[2]} == "-d" || string{argv[2]} == "--debug") {
            debugapply = true;
        }
        else {
            debugapply = false;
        }
    }
    if (debugapply) {
        cout << "--\nDEBUG MODE : ON\nLINE LIMIT : " << debuglimit << "\n--" << endl;
    }

    // Input file
    ifstream input(filename);

    // Variables for output
    long long sum = 0;

    // Read file
    string line;
    vector<long long> memory;
    long long spacesToFill = 0;
    int emptyVal = -1;
    // P2 things
    FileSystem fs;
    fs.startNextFile = 0;
    while (getline(input, line) && (!debugapply || debug < debuglimit))
    {
        debug++;
        if (debugapply) {
            cout << line << endl;
        }
        // Input is a single line (disk map), a series of individual blocks
        // We need to iterate over the line one by one
        // 1st digit is a file length
        // 2nd digit is a free space length
        // Repeats until end of line
        // Each file has a unique ID starting with 0 (these are unrelated to the block size! Always 1)
        long long pos = 0;
        long long len;
        long long id = 0; // ID of file starting at 0, incrementing
        while (pos < line.length()) {
            // Read file length
            len = stol(line.substr(pos,1));
            // Add file 'ID' to memory 'len' times
            for (long long i = 0; i < len; i++) {
                memory.push_back(id);
            }
            // Filesystem for P2 also
            fs.addFile(id, len);
            // Incremenent ID
            id++;
            //cout << "ID incremented to " << id << endl;
            pos++;
            
            // Read free space
            if (pos < line.length()) {
                len = stoi(line.substr(pos,1));
                // Add free space '.' 'len' times
                for (long long i = 0; i < len; i++) {
                    memory.push_back(emptyVal);
                    spacesToFill++;
                }
                fs.addEmptySpace(len);
                pos++;
            }
        }
        if (debugapply) {
            cout << "Resulting memory:\n";
            for (long long x : memory) {
                if (x == emptyVal) {
                    cout << " . ";
                }
                else {
                    cout << "{" << x << "}";
                }
            }
            cout << endl;
        }
    }
    // Finished with input file
    input.close();

    // Processing
    // Kept count of how many free space gaps we have to fill
    // Iterate over those, and take file from back of string and put in first available space
    // until no more spaces
    // Now we can start processing while keeping track of what we drop
    while (spacesToFill > 0) {
        // Peek at last ID
        long long last = memory.back();
        if (debugapply) { cout << "Last element = " << last << endl; }
        // Not interested in empty trailing memory, drop them all
        // Keep count though! These would have been counted in spacesToFill but are no longer needed for swaps
        // as nothing is behind them or was already moved ahead
        while (last == emptyVal) {
            if (debugapply) { cout << "Dropping extra empty space...\n"; }
            memory.pop_back();
            last = memory.back();
            spacesToFill--;
        }
        if (spacesToFill == 0) {
            // We must have just removed the last remaining empty space in the while loop
            cout << "Dropped last empty space, no more gaps in data to fill, returning\n";
        }
        else {
            // Now have value of last file ID to backfill
            // Find first empty space to put it in
            long long pos = 0;
            while (memory[pos] != emptyVal) {
                pos++;
            }
            if (debugapply) { cout << "First empty space at index: " << pos << ", updating with value: " << last << endl; }
            memory[pos] = last;
            memory.pop_back();
            if (debugapply) { cout << "Dropping last element...\n"; }

            spacesToFill--;
            if (debugapply) { cout << "There are " << spacesToFill << " remaining empty spaces to fill before the last element" << endl; }
            if (spacesToFill % 1000 == 0) {
                cout << spacesToFill << " remaining spaces left for processing...\n";
            }
        }
    }
    if (debugapply) {
        cout << "Resulting memory, after space filling:\n";
        for (long long x : memory) {
            if (x == emptyVal) {
                cout << " . ";
            }
            else {
                cout << "{" << x << "}";
            }
        }
        cout << endl;
    }
    // P1: Calculate 'checksum'
    long long i = 0;
    for (auto x : memory) {
        // 'Checksum' = Positional index * value
        if (x != emptyVal) {
            sum += x * i++;
        }
    }

    // Processing P2
    // This time we want to consider each file in its entirety, starting from the end
    // and working backwards
    // Only try to move a file once before moving on
    // If there are no spaces large enough to accomodate it in its entirety it does not move
    for (int i = fs.files.size() - 1; i >= 0; i--) {
        // Run through backwards
        //fs.files[i];
        if (debugapply) {
            cout << "Working on file " << i << ", id " << fs.files[i].id << ", len " << fs.files[i].length << "\n";
        }
        // Find first available space big enough
        long long pos = fs.findNextEmptySpace(fs.files[i].length);
        if (debugapply) {
            cout << "First possible space large enough (if any) at pos " << pos << endl;
        }
        // Is it better than our current space? Update if so
        if (pos != -1 && pos < fs.files[i].startingPos) {
            if (debugapply) { cout << "Updating position: YES\n"; }
            fs.files[i].startingPos = pos;
        }
        else {
            if (debugapply) { cout << "Updating position: NO\n"; }
        }
    }
    // TODO: Checksum P2

    // Output
    cout << "--\n";
    cout << "Checksum = " << sum << endl;

    cout << "--\nEnd.\n";
    return 0;
}

void FileSystem::addFile(long long id, long long length)
{
    File f;
    f.startingPos = startNextFile;
    f.length = length;
    f.id = id;
    files.push_back(f);
    startNextFile += length;
}

void FileSystem::addEmptySpace(long long length) {
    startNextFile += length;
}

long long FileSystem::findNextEmptySpace(long long length)
{
    // Special case, if the filesystem starts with a gap!
    // ...0.11.2
    if (files[0].startingPos >= length) {
        return 0;
    }

    int endLastFile = files[0].startingPos + files[0].length;
    for (auto f : files) {
        int gap = f.startingPos - endLastFile;
        if (gap >= length) {
            return endLastFile;
        }
        endLastFile = f.startingPos + f.length;
    }
    return -1;
}

