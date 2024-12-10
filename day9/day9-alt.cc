#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <sstream>
#include <stack>

using namespace std;

class File {
    public:
        long id;
        long startingPos;
        long length;
        long checksum();
};

class FileSystem {
    public:
        vector<File> files;
        long startNextFile;
        void addFile(long id, long length);
        void addEmptySpace(long length);
        long findFirstEmptyBlock(long length, long beforeBlockPos);
        void moveFile(File f, long newStartBlockPos);
};

int main(int argc, char* argv[])
{
    cout << "--\nAoC Day 9\n--\n";
    // For testing
    int debuglimit = 1;
    long debug = 0;
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
        long pos = 0;
        long len;
        long id = 0; // ID of file starting at 0, incrementing
        while (pos < line.length()) {
            // Read file length
            len = stol(line.substr(pos,1));
            // Add file 'ID' to memory 'len' times
            for (long i = 0; i < len; i++) {
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
                len = stoi(line.substr(pos,1));
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
    stack<File> fileStackP1;
    // Duplicate into a stack just so we can easily go backwards over each file exactly once
    // Even if we're moving them around in the actual FileSystem obj.
    for (auto f : fsP1.files) {
        fileStackP1.push(f);
    }
    while (!fileStackP1.empty()) {
        File f = fileStackP1.top();
        fileStackP1.pop();
        if (fileStackP1.size() % 1000 == 0 && fileStackP1.size() != 0) {
            cout << "Computing possible file fragment moves, " << fileStackP1.size() << " files remaining...\n";
        }
        // Run through backwards
        if (debugapply) {
            cout << "Working on file id " << f.id << ", len " << f.length << "\n";
        }
        // Find first available space big enough
        long blockPos = fsP1.findFirstEmptyBlock(f.length, f.startingPos);
        if (debugapply) {
            cout << "First possible space large enough (if any) at pos " << blockPos << endl;
        }
        // Is it better than our current space? Update if so
        if (blockPos != -1 && blockPos < f.startingPos) {
            if (debugapply) { cout << "Updating position: YES\n"; }
            fsP1.moveFile(f, blockPos);
        }
        else {
            if (debugapply) { cout << "Updating position: NO\n"; }
        }
    }
    // Checksum P1
    long sumP1 = 0;
    for (auto f : fsP1.files) {
        sumP1 += f.checksum();
    }

    cout << "== Part 2 ==\n";

    // Processing P2
    // This time we want to consider each file in its entirety, starting from the end
    // and working backwards
    // Only try to move a file once before moving on
    // If there are no spaces large enough to accomodate it in its entirety it does not move
    stack<File> fileStack;
    // Duplicate into a stack just so we can easily go backwards over each file exactly once
    // Even if we're moving them around in the actual FileSystem obj.
    for (auto f : fsP2.files) {
        fileStack.push(f);
    }
    while (!fileStack.empty()) {
        File f = fileStack.top();
        fileStack.pop();
        if (fileStack.size() % 1000 == 0 && fileStack.size() != 0) {
            cout << "Computing possible file fragment moves, " << fileStack.size() << " files remaining...\n";
        }
        // Run through backwards
        if (debugapply) {
            cout << "Working on file id " << f.id << ", len " << f.length << "\n";
        }
        // Find first available space big enough
        long blockPos = fsP2.findFirstEmptyBlock(f.length, f.startingPos);
        if (debugapply) {
            cout << "First possible space large enough (if any) at pos " << blockPos << endl;
        }
        // Is it better than our current space? Update if so
        if (blockPos != -1 && blockPos < f.startingPos) {
            if (debugapply) { cout << "Updating position: YES\n"; }
            fsP2.moveFile(f, blockPos);
        }
        else {
            if (debugapply) { cout << "Updating position: NO\n"; }
        }
    }
    // Checksum P2
    long sumP2 = 0;
    for (auto f : fsP2.files) {
        sumP2 += f.checksum();
    }

    // Output
    cout << "--\n";
    cout << "P1 Checksum = " << sumP1 << endl;
    cout << "P2 Checksum = " << sumP2 << endl;

    cout << "--\nEnd.\n";
    return 0;
}

void FileSystem::addFile(long id, long length)
{
    File f;
    f.startingPos = startNextFile;
    f.length = length;
    f.id = id;
    files.push_back(f);
    startNextFile += length;
}

void FileSystem::addEmptySpace(long length) {
    startNextFile += length;
}

long FileSystem::findFirstEmptyBlock(long length, long beforeBlockPos)
{
    // Special case, if the filesystem starts with a gap!
    // ...0.11.2
    if (files[0].startingPos >= length) {
        cout << "DEBUG: Gap big enough for " << length << " before file 0!";
        return 0;
    }

    int endPrevFile = files[0].startingPos + files[0].length;
    for (int i = 1; endPrevFile < beforeBlockPos && i < files.size(); i++) {
        int gap = files[i].startingPos - endPrevFile;
        //cout << "Previous file ended at " << endPrevFile << " and this file id " << files[i].id << " starts at " << files[i].startingPos << " leaving a gap of " << gap << " before this file\n";
        if (gap >= length) {
            return endPrevFile;
            //cout << "Gap found at pos. " << endPrevFile << endl;
        }
        endPrevFile = files[i].startingPos + files[i].length;
    }
    //cout << "No gap found, returning -1\n";
    return -1;
}
    
void FileSystem::moveFile(File f, long newStartBlockPos) {
    // Must find and erase file f from the file list
    // And insert it again at the new position
    // Be careful not to mix up block positions and filesystem index positions
    //cout << "moveFile: Received move command, file id " << f.id << " to new filesystem pos " << newPos << endl;
    bool found = false;
    long oldIndex;
    for (int i = files.size() - 1; !found && i >= 0; i--) {
        if (files[i].id == f.id) {
            found = true;
            oldIndex = i;
            //cout << "moveFile: found old vector index " << oldIndex << endl;
        }
    }
    if (found) {
        // Remove it
        files.erase(files.begin()+oldIndex);
    }
    else {
        ostringstream os;
        os << "Didn't find file id " << f.id << " in FileSystem files";
        throw runtime_error(os.str());
    }
    // Insert at new location
    // First find new index
    long newIndex;
    found = false;
    for (int i = 0; !found && i < files.size(); i++) {
        if (files[i].startingPos > newStartBlockPos) {
            // The moved file needs to displace this one in the list,
            // i.e. be inserted at this index
            newIndex = i;
            //cout << "Found insertion index for file at " << newIndex << endl;
            //cout << "== because newPos = " << newPos << " but file index " << i << " id " << files[i].id << " fs pos is " << files[i].startingPos << endl;
            found = true;
        }
    }
    f.startingPos = newStartBlockPos;
    files.insert(files.begin()+newIndex, f);
    //cout << "  Contents of files vector after move command\n";
    //for (auto f : files) {
    //    cout << "    Vector element : file id " << f.id << endl;
    //}
}

long File::checksum() {
    // Checksum is the ID multiplied by the position of each block on the filesystem
    // So if file ID 8 len 3 starts at pos 2 like so ..888..
    // We need to do 2*8 + 3*8 + 4*8
    int cksum = 0;
    for (int i = 0; i < length; i++) {
        cksum += id * (startingPos + i);
    }
    return cksum;
}

