#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <sstream>
#include <stack>
#include <map>
#include <queue>

using namespace std;

class File {
    public:
        long id;
        long startingPos;
        int length;
        long checksum();
};

class FileSystem {
    public:
        vector<File> files;
        map<long,int> gaps; // Map of starting pos. to length of gap
        queue<long> gapQueue; // Map of individual gap positions for P1 optimisation
        long startNextFile;
        void addFile(long id, int length);
        void addEmptySpace(int length);
        long findAndUpdateFirstEmptyBlock(int length, long beforeBlockPos);
        void moveFile(File& f, long newStartBlockPos, bool debugprints);
        long processFileSystem(bool p1_optimised, bool debugapply);
};

int main(int argc, char* argv[])
{
    cout << "--\nAoC Day 9\n--\n";
    // For testing
    int debuglimit = 1;
    long debug = 0;
    bool debugapply = false;

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
        long pos = 0;
        int len;
        long id = 0; // ID of file starting at 0, incrementing
        //cout << "Line length " << line.length() << " char\n";
        while (pos < line.length()) {
            if (pos != 0 && pos % 200000 == 0) {
                cout << "Read " << pos << "/" << line.length() << " char...\n";
            }
            // Read file length
            len = stol(line.substr(pos,1));
            // Add file 'ID' to memory 'len' times
            for (int i = 0; i < len; i++) {
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
                len = stol(line.substr(pos,1));
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
    long sumP1 = fsP1.processFileSystem(true, debugapply);

    cout << "== Part 2 ==\n";
    long sumP2 = fsP2.processFileSystem(false, debugapply);

    // Output
    cout << "--\n";
    cout << "P1 Checksum = " << sumP1 << endl;
    cout << "P2 Checksum = " << sumP2 << endl;

    // Checks
    if (sumP1 == 1928 || sumP1 == 6323641412437)
    {
        cout << "P1 Answer looks correct\n";
    }
    else
    {
        cout << "P1 Answer looks wrong\n";
    }
    if (sumP2 == 2858 || sumP2 == 6351801932670)
    {
        cout << "P2 Answer looks correct\n";
    }
    else
    {
        cout << "P2 Answer looks wrong\n";
    }

    cout << "--\nEnd.\n";
    return 0;
}

void FileSystem::addFile(long id, int length)
{
    if (length == 0) {
        // Don't be fooled by 0 length files!
        return;
    }
    File f;
    f.startingPos = startNextFile;
    f.length = length;
    f.id = id;
    files.push_back(f);
    startNextFile += length;
}

void FileSystem::addEmptySpace(int length) {

    // Map for general solution
    gaps[startNextFile] = length;
    // Queue for P1 optimisations
    for (int i = 0; i < length; i++) {
        gapQueue.push(startNextFile + i);
    }
    // Keep track where we are
    startNextFile += length;
}

// Returns starting block pos. of the empty filesystem gap that was reduced by 'length', if any
// So a file of 'length' can now be inserted there instead
// Returns -1 if no such large enough empty block span can be found before beforeBlockPos
long FileSystem::findAndUpdateFirstEmptyBlock(int length, long beforeBlockPos)
{
    long gapStartBlock;
    int gapLength;
    for (auto it = gaps.begin(); it != gaps.end(); it++) {
        // Ordered map so we should iterate from earliest starting pos
        gapStartBlock = it->first;
        gapLength = it->second;
        if (gapLength >= length) {
            // Use this one!
            // Update it before returning
            // Remove it entirely if remaining length is 0, otherwise there is a big performance penalty trying lots of 0 length gaps at the beginning
            // But vector erase performance penalty is worse! Changing back to ordered map
            // The old one must be erased either way
            gaps.erase(it);
            if (gapLength - length != 0) {
                // Reduce remaining gap length and move up starting position by the amount we have been asked to clear
                gaps[gapStartBlock + length] = gapLength - length;
            }
            return gapStartBlock;
        }
        else if (gapStartBlock > beforeBlockPos) {
            return -1;
        }
    }
    //cout << "No gap found, returning -1\n";
    return -1;
}

void FileSystem::moveFile(File& f, long newStartBlockPos, bool debugprints) {
    // Be careful not to mix up block positions and filesystem index positions
    if (debugprints) {
        cout << "moveFile: Received move command, file id " << f.id << " from filesystem pos " << f.startingPos << " to " << newStartBlockPos << endl;
    }

    // Update location
    f.startingPos = newStartBlockPos;

    if (debugprints) {
        cout << "  Contents of gaps map after move command:\n";
        for (auto it = gaps.begin(); it != gaps.end(); it++) {
            cout << "    Gap pos. " << it->first << " length " << it->second << endl;
        }
    }
}

long File::checksum() {
    // Checksum is the ID multiplied by the position of each block on the filesystem
    // So if file ID 8 len 3 starts at pos 2 like so ..888..
    // We need to do 2*8 + 3*8 + 4*8
    long cksum = 0;
    for (int i = 0; i < length; i++) {
        cksum += id * (startingPos + i);
    }
    return cksum;
}

long FileSystem::processFileSystem(bool p1_optimised, bool debugapply) {
    long remaining = files.size();
    long blockPos;
    File* f;
    for (long i = files.size() - 1; i >= 0; i--)
    {
        f = &files[i];
        if (remaining % 10000 == 0 && remaining != 0) {
            cout << "Computing possible file fragment moves, " << remaining << " files remaining...\n";
        }
        remaining--;
        // Run through backwards
        if (debugapply) {
            cout << "Working on file id " << f->id << ", len " << f->length << "\n";
        }
        if (p1_optimised) {
            // Part 1 optimisations can be used
            // Files and gaps always guaranteed to be size 1
            // Still using the original vector, but don't bother checking lengths
            // Just pop and use the first queue element
            if (!gapQueue.empty())
            {
                blockPos = gapQueue.front();
                gapQueue.pop();
                if (debugapply) {
                    cout << "Popped block position " << blockPos << " from gap queue, there are " << gapQueue.size() << " queue positions left to test\n";
                }
                if (blockPos < f->startingPos)
                {
                    if (debugapply) { cout << "Updating position: YES\n"; }
                    moveFile(*f, blockPos, debugapply);
                }
                else
                {
                    if (debugapply) {
                        cout << "Updating position: NO\n";
                        cout << "Breaking loop due to gap / file crossover\n";
                    }
                    // Since every gap is guaranteed to be filled and we are working backwards
                    // through the files, we also know that if a file is not moved then
                    // we have reached the crossing point and can exit the for loop early
                    break;
                }
            }
            else {
                // Ran out of gaps
                if (debugapply) {
                    cout << "There are no more available gaps in the gap queue to query, breaking loop\n";
                }
                break;
            }
        }
        else {
            // General solution
            // Find first available space big enough
            blockPos = findAndUpdateFirstEmptyBlock(f->length, f->startingPos);
            if (debugapply) {
                cout << "First possible space large enough (if any) at pos " << blockPos << endl;
            }
            // Is it better than our current space? Update if so
            if (blockPos != -1 && blockPos < f->startingPos) {
                if (debugapply) { cout << "Updating position: YES\n"; }
                moveFile(*f, blockPos, debugapply);
            }
            else {
                if (debugapply) { cout << "Updating position: NO\n"; }
            }
        }
    }
    // Checksum
    long sum = 0;
    for (auto file : files) {
        sum += file.checksum();
    }
    return sum;
}

