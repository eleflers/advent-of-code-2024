#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <sstream>
#include <stack>
#include <map>

using namespace std;

class File {
    public:
        long long id;
        long long startingPos;
        long long length;
        long long checksum();
};

class FileSystem {
    public:
        map<long long,File> files; // Map starting pos. to File
        map<long long,long long> gaps; // Map of starting pos. to length of gap
        long long startNextFile;
        void addFile(long long id, long long length);
        void addEmptySpace(long long length);
        long long findFirstEmptyBlock(long long length, long long beforeBlockPos);
        void moveFile(File f, long long newStartBlockPos, bool debugprints);
};

long long processFileSystem(FileSystem fs, bool debugapply);

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
    //long sum = 0;

    // Read file
    string line;
    int emptyVal = -1;
    // Object approach
    FileSystem fsP1;
    fsP1.startNextFile = 0;
    FileSystem fsP2;
    fsP2.startNextFile = 0;
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
            len = stoll(line.substr(pos,1));
            // Add file 'ID' to memory 'len' times
            for (long long i = 0; i < len; i++) {
                // Use P2 code to solve P1 also, just add 'len' files of length 1 instead of 1 file length 'len'
                fsP1.addFile(id, 1);
            }
            // Filesystem for P2 also, single file length 'len'
            fsP2.addFile(id, len);
            // Incremenent ID
            id++;
            //cout << "ID incremented to " << id << endl;
            pos++;
            
            // Read free space
            if (pos < line.length()) {
                len = stoll(line.substr(pos,1));
                // Add free space '.' 'len' times
                fsP1.addEmptySpace(len);
                fsP2.addEmptySpace(len);
                pos++;
            }
        }
    }
    // Finished with input file
    input.close();

    cout << "== Part 1 ==\n";

    // Processing P1
    // Refactored to use same code as P2, each file is just broken up into files of blocklength 1 with the same ID
    // Only try to move a file once before moving on
    // Duplicate into a stack just so we can easily go backwards over each file exactly once
    // Even if we're moving them around in the actual FileSystem obj.
    long long sumP1 = processFileSystem(fsP1, debugapply);

    cout << "== Part 2 ==\n";
    long long sumP2 = processFileSystem(fsP2, debugapply);

    // Output
    cout << "--\n";
    cout << "P1 Checksum = " << sumP1 << endl;
    cout << "P2 Checksum = " << sumP2 << endl;

    cout << "--\nEnd.\n";
    return 0;
}

void FileSystem::addFile(long long id, long long length)
{
    File f;
    f.startingPos = startNextFile;
    f.length = length;
    f.id = id;
    files[startNextFile] = f;
    startNextFile += length;
}

void FileSystem::addEmptySpace(long long length) {

    gaps[startNextFile] = length;
    startNextFile += length;
}

long long FileSystem::findFirstEmptyBlock(long long length, long long beforeBlockPos)
{
    for (auto it = gaps.begin(); it != gaps.end(); it++) {
        // Ordered map so we should iterate from earliest starting pos
        long long gapStartBlock = it->first;
        long long gapLength = it->second;
        if (gapLength >= length) {
            // Use this one!
            // Previous gap at X is now 0, but whatever is left is a new gap starting X + length_of_file
            return gapStartBlock;
        }
    }
    //cout << "No gap found, returning -1\n";
    return -1;
}
    
void FileSystem::moveFile(File f, long long newStartBlockPos, bool debugprints) {
    // Be careful not to mix up block positions and filesystem index positions
    if (debugprints) {
        cout << "moveFile: Received move command, file id " << f.id << " from filesystem pos " << f.startingPos << " to " << newStartBlockPos << endl;
    }

    // Update location
    long long oldStartingPos = f.startingPos;
    f.startingPos = newStartBlockPos;
    files[newStartBlockPos] = f;
    files.erase(oldStartingPos);

    // Update gap we just filled
    long long gapLength = gaps[newStartBlockPos];
    gaps.erase(newStartBlockPos);
    if (gapLength - f.length > 0) {
        // Gap not filled, so make new gap in remainder of space
        gaps[newStartBlockPos + f.length] = gapLength - f.length;
    }

    if (debugprints) {
        cout << "  Contents of gaps map after move command:\n";
        for (auto it = gaps.begin(); it != gaps.end(); it++) {
            cout << "    Gap pos. " << it->first << " length " << it->second << endl;
        }
    }
}

long long File::checksum() {
    // Checksum is the ID multiplied by the position of each block on the filesystem
    // So if file ID 8 len 3 starts at pos 2 like so ..888..
    // We need to do 2*8 + 3*8 + 4*8
    long long cksum = 0;
    for (long long i = 0; i < length; i++) {
        cksum += id * (startingPos + i);
    }
    return cksum;
}

long long processFileSystem(FileSystem fs, bool debugapply) {
    int remaining = fs.files.size();
    stack<long long> posStack;
    for (auto iter = fs.files.begin(); iter != fs.files.end(); iter++) {
        posStack.push(iter->first);
    }
    while (posStack.size() > 0) {
        long long pos = posStack.top();
        posStack.pop();
        File f = fs.files[pos];
        if (remaining % 1000 == 0 && remaining != 0) {
            cout << "Computing possible file fragment moves, " << remaining << " files remaining...\n";
        }
        remaining--;
        // Run through backwards
        if (debugapply) {
            cout << "Working on file id " << f.id << ", len " << f.length << "\n";
        }
        // Find first available space big enough
        long long blockPos = fs.findFirstEmptyBlock(f.length, f.startingPos);
        if (debugapply) {
            cout << "First possible space large enough (if any) at pos " << blockPos << endl;
        }
        // Is it better than our current space? Update if so
        if (blockPos != -1 && blockPos < f.startingPos) {
            if (debugapply) { cout << "Updating position: YES\n"; }
            fs.moveFile(f, blockPos, debugapply);
        }
        else {
            if (debugapply) { cout << "Updating position: NO\n"; }
        }
    }
    // Checksum
    long long sum = 0;
    for (auto f : fs.files) {
        sum += f.second.checksum();
    }
    return sum;
}

