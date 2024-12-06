#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <sstream>
#include <queue>

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
        void updateChar(int x, int y, char newval);
        int getSizeX();
        int getSizeY();
};
class Guard {
    public:
        int xpos;
        int ypos;
        enum Direction dir;
        pair<int,int> getNextPos(int multiplier);
        void turnRight();
        void move();
        // For part 2
        vector<pair<int,int>> infiniteLoopPoints;
        bool atPossibleInfiniteLoop(Lab lab, char marker);
        Direction getTravelDirectionOfPoint();
        Direction getTravelDirectionOfPoint(int x, int y);
        void moveBackwards();
    private:
        unordered_map<string,Direction> travelDirections;
        void setTravelDirectionOfPoint();
};

string getFileCoordinatesString(int x, int y);
bool runGuardInfiniteLoopCheck(Guard g, Lab l, int xPosObstacle, int yPosObstacle);

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
    int infiniteLoopPoints = 0;

    // Read input into our data structures
    string line;
    int lineNr = 0;
    Lab lab;
    Guard guard;
    Guard guardP2;
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
            // Part 2 guard here also
            guardP2.xpos = pos;
            guardP2.ypos = lineNr;
            guardP2.dir = UP;
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
    // For part 2
    // Just log every single cell we pass over so we can brute force test them later
    queue<pair<int,int>> potentialObstacles;
    // Loop
    int steps = 0; int turns = 0;
    bool escaped = false;
    while (!escaped)
    {
        pair<int,int> nextPos = guard.getNextPos(1);
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
                bool newplace = (nextChar != marker);
                if (newplace) {
                    // Not been here before, count it
                    sum++;
                    lab.updateChar(xNext, yNext, marker);
                    // Log it for P2
                    potentialObstacles.push({xNext, yNext});
                }
                // Move guard, this also logs the previous location's direction of travel
                guard.move();
                steps++;
                if (!newplace && guard.atPossibleInfiniteLoop(lab, marker)) {
                    //cout << "Guard position " << getFileCoordinatesString(guard.xpos,guard.ypos) << " is an infinite loop point\n";
                    infiniteLoopPoints++;
                }
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

    // Part 2:
    // Brute force approach
    // Re-run the above, but this time, pop the first obstacle first
    // Just try every point along the path...
    int sumP2 = 0;
    int p2Runs = 0;
    while (potentialObstacles.size() != 0) {
        p2Runs++;
        pair<int,int> posNewObstacle = potentialObstacles.front();
        potentialObstacles.pop();
        int xPosNewObstacle = posNewObstacle.first;
        int yPosNewObstacle = posNewObstacle.second;

        cout << "Simulating P2 guard " << p2Runs << "/" << potentialObstacles.size ()+ p2Runs;
        cout << " with obstacle at " << getFileCoordinatesString(xPosNewObstacle, yPosNewObstacle) << "\n";

        // Guard is copied by function call so we can re-use it
        bool infiniteLoop = runGuardInfiniteLoopCheck(guardP2, lab, xPosNewObstacle, yPosNewObstacle);
        if (infiniteLoop) {
            cout << "-- Guard " << p2Runs << " found an infinite loop! --\n";
            sumP2++;
        }
    }

    // Output
    cout << "--\n";
    cout << "Sum unique spots visited = " << sum << endl;
    cout << "Sum possible infinite loop points = " << infiniteLoopPoints << endl;
    cout << "Sum P2 = " << sumP2 << endl;

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

void Lab::updateChar(int x, int y, char newval)
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

pair<int,int> Guard::getNextPos(int multiplier)
{
    // Be careful, y position actually increases as we go _down_ the lines!
    pair<int,int> next;
    switch (dir)
    {
        case UP:
            next.first = xpos;
            next.second = ypos - multiplier;
            break;
        case RIGHT:
            next.first = xpos + multiplier;
            next.second = ypos;
            break;
        case DOWN:
            next.first = xpos;
            next.second = ypos + multiplier;
            break;
        case LEFT:
            next.first = xpos - multiplier;
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

void Guard::move() {

    // Before we move log the direction we moved on our previous spot for Part 2
    setTravelDirectionOfPoint();
    // Now move
    pair<int,int> nextPos = getNextPos(1);
    xpos = nextPos.first;
    ypos = nextPos.second;
}

void Guard::moveBackwards() {

    // For part 2
    pair<int,int> nextPos = getNextPos(-1);
    xpos = nextPos.first;
    ypos = nextPos.second;
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

bool Guard::atPossibleInfiniteLoop(Lab lab, char marker)
{
    //cout << "Checking previously travelled point " << getFileCoordinatesString(xpos, ypos) << " for infinite loop potential\n";
    // Part 2 stuff:
    // Every time we cross a point we have already been to it could be a possible loop point
    // It depends on the direction we were travelling when we most recently crossed that point

    // We been here before
    Direction prevDirection = getTravelDirectionOfPoint();

    // If we were previously travelling one right turn clockwise of our current
    // direction of travel, we can turn again here and join an infinite loop of our past self
    bool possible;
    switch (prevDirection)
    {
        case UP:
            possible = (dir == LEFT);
            break;
        case RIGHT:
            possible = (dir == UP);
            break;
        case DOWN:
            possible = (dir == RIGHT);
            break;
        case LEFT:
            possible = (dir == DOWN);
            break;
        default:
            possible = false;
            break;
    }

    if (!possible) {
        //cout << "Point " << getFileCoordinatesString(xpos, ypos) << " found false after direction check\n";
        return false;
    }

    // We now know if we can make an infinite loop
    // However the place we want to block may have already been taken by a past path
    // or be out of bounds as we are on the edge of the map already
    // So check this too
    pair<int,int> blockingLoopPoint = getNextPos(1);
    int xBlocking = blockingLoopPoint.first;
    int yBlocking = blockingLoopPoint.second;
    if (xBlocking == -1 || xBlocking == lab.getSizeX()) {
        // Blocking point would be out of bounds
        possible = false;
    }
    else if (yBlocking == -1 || yBlocking == lab.getSizeY()) {
        // Blocking point would be out of bounds
        possible = false;
    }

    if (!possible) {
        //cout << "Requires point of obstacle " << getFileCoordinatesString(xBlocking, yBlocking) << " found false after out of bounds checks\n";
        return false;
    }
    char nextChar = lab.getChar(xBlocking, yBlocking);
    if (nextChar == marker) {
        possible = false;
        //cout << "Requires point of obstacle " << getFileCoordinatesString(xBlocking, yBlocking) << " found false as previously travelled\n";
    }

    return possible;
}

string getFileCoordinatesString(int x, int y)
{
    ostringstream out;
    out << "[" << x << "," << y << "] (l:" << y+1 << ",c:" << x+1 << ")";
    return out.str();
}

// Log direction of travel for every point passed through for Part 2
// If we pass through it again, depending on direction we travelled through it could
// be the setup for an infinite loop
void Guard::setTravelDirectionOfPoint()
{
    ostringstream os;
    os << xpos << "x" << ypos;
    string key = os.str();

    // Set
    travelDirections[key] = dir;
}

Direction Guard::getTravelDirectionOfPoint()
{
    return getTravelDirectionOfPoint(xpos, ypos);
}

Direction Guard::getTravelDirectionOfPoint(int x, int y)
{
    ostringstream os;
    os << x << "x" << y;
    string key = os.str();

    if (!travelDirections.contains(key))
    {
        ostringstream err;
        err << "Asked for key '" << key << "' that I do not have";
        throw runtime_error(err.str());
    }

    // Get
    return travelDirections[key];
}

bool runGuardInfiniteLoopCheck(Guard guard, Lab lab, int xPosObstacle, int yPosObstacle)
{
    // Here we run a subset of the P1 loop where we are only interested in
    // tracking whether we enter an infinite loop or not
    bool escaped = false;
    pair<int,int> nextPos;
    int xNext;
    int yNext;
    char nextChar;
    char marker;
    char notVisited = '.';
    unordered_map<string,int> obstacleHits;
    while (!escaped)
    {
        nextPos = guard.getNextPos(1);
        xNext = nextPos.first;
        yNext = nextPos.second;
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
            nextChar = lab.getChar(xNext, yNext);
            // We could also hit our new obstacle again!
            if (nextChar == '#' || ( xPosObstacle == xNext && yPosObstacle == yNext )) {
                // Hit an obstacle, so turn right instead
                // First log it so we know which direction we hit from
                ostringstream os;
                string dirString;
                switch (guard.dir)
                {
                    case UP:
                        dirString = "up";
                        break;
                    case RIGHT:
                        dirString = "ri";
                        break;
                    case DOWN:
                        dirString = "do";
                        break;
                    case LEFT:
                        dirString = "le";
                        break;
                    default:
                        break;
                }
                os << xNext << "x" << yNext << dirString;
                if (obstacleHits.contains(os.str()) && obstacleHits[os.str()] == 1) {
                    return true;
                }
                else {
                    obstacleHits[os.str()] = 1;
                }
                guard.turnRight();
            }
            else
            {
                // Move guard
                guard.move();
            }
        }
    }
    return false;
}

