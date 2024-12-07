#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <sstream>

using namespace std;

enum Direction {
    UP = 0,
    RIGHT,
    DOWN,
    LEFT
};

class Lab {
    public:
        vector<string> grid;
        char getChar(int x, int y);
        char updateChar(int x, int y, char newval);
};
class Guard {
    public:
        int xpos;
        int ypos;
        enum Direction dir;
};

int main(int argc, char* argv[])
{
    cout << "--\nAoC Day 6\n--\n";
    // For testing
    int debuglimit = 10;
    int debug = 0;
    bool debugapply = true;
    if (debugapply) { cout << "DEBUG MODE : ON\nLINE LIMIT : " << debuglimit << "\n--" << endl; }

    // Read input args if any
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

    // Read input into our data structures
    string line;
    int lineNr = 0;
    Lab lab;
    Guard guard;
    while (getline(input, line) && (!debugapply || debug < debuglimit))
    {
        debug++;
        if (debugapply) {
            cout << "Lab object contains " << lab.grid.size() << " rows\n";
            cout << "Line " << debug << ": " << line << endl;
        }

        lab.grid.push_back(line);
        auto pos = line.find("^");
        if (pos != string::npos) {
            cout << "Found guard in starting position [" << pos << "," << lineNr << "]\n";
            guard.xpos = pos;
            guard.ypos = lineNr;
            guard.dir = UP;
        }
        lineNr++;
    }
    // Finished with input file
    input.close();

    // Processing
    cout << "Lab object contains " << lab.grid.size() << " rows\n";
    cout << "Guard obj. location [" << guard.xpos << "," << guard.ypos << "]\n";
    cout << "Char at guard location before: " << lab.getChar(guard.xpos, guard.ypos) << endl;
    lab.updateChar(guard.xpos, guard.ypos, 'X');
    cout << "Guard location after: " << lab.getChar(guard.xpos, guard.ypos) << endl;

    // Output
    cout << "--\n";
    cout << "Sum = " << sum << endl;

    cout << "--\nEnd.\n";
    return 0;
}

char Lab::getChar(int x, int y)
{
    // Return char. at x,y pos. so we don't have to remember the vector<string> line structure
    if (y < 0 || y >= grid.size()) {
        ostringstream os;
        os << "Position [" << x << "," << y << "] is out of bounds, in y direction. (grid.size() = " << grid.size() << ")\n";
        throw std::runtime_error(os.str()); // TODO: Make our own Exception class
    }
    if (x < 0 || x >= grid[0].length()) {
        ostringstream os;
        os << "Position [" << x << "," << y << "] is out of bounds, in x direction. (grid[0].length() = " << grid[0].length() << ")\n";
        throw std::runtime_error(os.str()); // TODO: Make our own Exception class
    }
    string line = grid[y];
    return line[x];
}

char Lab::updateChar(int x, int y, char newval)
{
    // Return char. at x,y pos. so we don't have to remember the vector<string> line structure
    if (y < 0 || y >= grid.size()) {
        ostringstream os;
        os << "Position [" << x << "," << y << "] is out of bounds, in y direction. (grid.size() = " << grid.size() << ")\n";
        throw std::runtime_error(os.str()); // TODO: Make our own Exception class
    }
    if (x < 0 || x >= grid[0].length()) {
        ostringstream os;
        os << "Position [" << x << "," << y << "] is out of bounds, in x direction. (grid[0].length() = " << grid[0].length() << ")\n";
        throw std::runtime_error(os.str()); // TODO: Make our own Exception class
    }
    grid[y][x] = newval;
}

