#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <sstream>
#include <stack>

using namespace std;

int main(int argc, char* argv[])
{
    cout << "--\nAoC Day 9\n--\n";
    // For testing
    int debuglimit = 1;
    int debug = 0;
    bool debugapply = true;
    if (debugapply) { cout << "DEBUG MODE : ON\nLINE LIMIT : " << debuglimit << "\n--" << endl; }

    // User args
    string filename = "input";
    if (argc == 1) {
        cout << "Assume default input file '" << filename << "'\n";
    }
    else if (argc > 1) {
        filename = argv[1];
        cout << "Taking CLI input file name '" << filename << "'\n";
    }
    // Input file
    ifstream input(filename);

    // Variables for output
    int sum = 0;

    // Read file
    string line;
    ostringstream memorystringbuilder;
    string memory;
    int spacesToFill = 0;
    while (getline(input, line) && (!debugapply || debug < debuglimit))
    {
        debug++;
        if (debugapply) {
            cout << line << endl;
        }
        // Input is a single line (disk map), a series of individual digits
        // We need to iterate over the line one by one
        // 1st digit is a file length
        // 2nd digit is a free space length
        // Repeats until end of line
        // Each file has a unique ID starting with 0
        long pos = 0;
        int len;
        int id = 0; // Rotating ID of file starting at 0
        while (pos < line.length()) {
            // Read file length
            len = stoi(line.substr(pos,1));
            // Add file 'ID' to memory 'len' times
            for (int i = 0; i < len; i++) {
                memorystringbuilder << id;
            }
            // Incremenent ID, wraps around to 0 again from 9? (Not stated in task)
            id++;
            id = id % 10;
            cout << "ID incremented to " << id << endl;
            pos++;
            
            // Read free space
            if (pos < line.length()) {
                len = stoi(line.substr(pos,1));
                // Add free space '.' 'len' times
                for (int i = 0; i < len; i++) {
                    memorystringbuilder << ".";
                    spacesToFill++;
                }
                pos++;
            }
        }
        memory = memorystringbuilder.str();
        cout << "Resulting memory:\n" << memory << endl;
    }
    // Finished with input file
    input.close();

    // Processing
    // Kept count of how many free space gaps we have to fill
    // Iterate over those, and take file from back of string and put in first available space
    // until no more spaces
    while (spacesToFill-- > 0) {
        auto pos = memory.find(".");
        int gap = pos;
        pos = memory.find_last_not_of("."); // last digit to move
        // Move it
        memory[gap] = memory[pos];
        memory[pos] = '.';
    }
    cout << "Resulting memory, after space filling:\n" << memory << endl;
    // Calculate 'checksum'
    int i = 0;
    while (i < memory.length() && isdigit(memory[i])) {
        // 'Checksum' = Positional index * value
        sum += i * stoi(memory.substr(i,1));
        i++;
    }

    // Output
    cout << "--\n";
    cout << "Checksum = " << sum << endl;

    cout << "--\nEnd.\n";
    return 0;
}

