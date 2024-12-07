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
        int getSizeX();
        int getSizeY();
};
class Guard {
    public:
        int xpos;
        int ypos;
        enum Direction dir;
        pair<int,int> getNextPos();
        void turnRight();
};

int main(int argc, char* argv[])
{
    cout << "--\nAoC Day 6\n--\n";
    // For testing
    int debuglimit = 10;
    int debug = 0;
    bool debugapply = false;
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

        // Build vector
        lab.grid.push_back(line);
        // Look for the guard's starting spot
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
    char marker = 'X';
    // Log starting position before moving
    lab.updateChar(guard.xpos, guard.ypos, marker);
    sum++;
    // Loop
    int steps = 0; int turns = 0;
    bool escaped = false;
    while (!escaped)
    {
        pair<int,int> nextPos = guard.getNextPos();
        int xNext = nextPos.first;
        int yNext = nextPos.second;
        // Are we at the edge
        if (xNext == -1 || xNext == lab.getSizeX()) {
            // Escaped horizontally
            escaped = true;
        }
        else if (yNext == -1 || yNext == lab.getSizeY()) {
            // Escaped vertically
            escaped = true;
        }
        else {
            // Process next step
            char nextChar = lab.getChar(xNext, yNext);
            if (nextChar == '#') {
                // Hit an obstacle, so turn right instead
                guard.turnRight();
                turns++;
            }
            else {
                if (nextChar != marker) {
                    // Not been here before, mark and count it
                    lab.updateChar(xNext, yNext, marker);
                    sum++;
                }
                // Move guard
                guard.xpos = xNext;
                guard.ypos = yNext;
                steps++;
            }
        }
    }
    cout << "Escaped after " << steps << " steps and " << turns << " turns!\n";
    if (debugapply) {
        cout << "Printing map..." << endl;
        for (auto line : lab.grid) {
            cout << line << endl;
        }
    }

    // Output
    cout << "--\n";
    cout << "Sum unique spots visited = " << sum << endl;

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
        throw std::runtime_error(os.str());
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
        throw std::runtime_error(os.str());
    }
    if (x < 0 || x >= grid[0].length()) {
        ostringstream os;
        os << "Position [" << x << "," << y << "] is out of bounds, in x direction. (grid[0].length() = " << grid[0].length() << ")\n";
        throw std::runtime_error(os.str());
    }
    grid[y][x] = newval;
}

pair<int,int> Guard::getNextPos()
{
    // Be careful, y position actually increases as we go _down_ the lines!
    pair<int,int> next;
    switch (dir)
    {
        case UP:
            next.first = xpos;
            next.second = ypos - 1;
            break;
        case RIGHT:
            next.first = xpos + 1;
            next.second = ypos;
            break;
        case DOWN:
            next.first = xpos;
            next.second = ypos + 1;
            break;
        case LEFT:
            next.first = xpos - 1;
            next.second = ypos;
            break;
        default:
            break;
    }
    return next;
}

int Lab::getSizeX()
{
    // Line width
    return grid[0].length();
}

int Lab::getSizeY()
{
    // Line count
    return grid.size();
}

void Guard::turnRight()
{
    // For some reason there's no easy way to simply +1 to an enum or cycle through it?
    // Or even just get the digit value out
    // Let's just use a switch statement instead...
    switch (dir)
    {
        case UP:
            dir = RIGHT;
            break;
        case RIGHT:
            dir = DOWN;
            break;
        case DOWN:
            dir = LEFT;
            break;
        case LEFT:
            dir = UP;
            break;
        default:
            break;
    }
}

