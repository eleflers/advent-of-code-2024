#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>

using namespace std;

int main(int argc, char* argv[])
{
    cout << "--\nAoC Day 3\n--\n";
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
    while (getline(input, line) && (!debugapply || debug < debuglimit))
    {
        debug++;
        if (debugapply) {
            cout << line << endl;
        }
    }

    // Finished with input file
    input.close();

    // Output
    cout << "--\n";
    cout << "Sum = " << sum << endl;

    cout << "--\nEnd.\n";
    return 0;
}

