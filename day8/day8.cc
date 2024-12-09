#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <set>

using namespace std;

class Antenna {
    public:
        char frequency;
        int x;
        int y;
        vector<pair<int,int>> getAntiNodes(Antenna b, int maxX, int maxY);
};

class AntennaCollection {
    private:
        unordered_map<char,vector<Antenna>> antennas;
    public:
        vector<pair<int,int>> getAntinodes(int maxX, int maxY);
        void addAntenna(Antenna a);
};

int main(int argc, char* argv[])
{
    cout << "--\nAoC Day 8\n--\n";
    // For testing
    int debuglimit = 12;
    int debug = 0;
    bool debugapply = false;
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
    int lineNr = 0;
    AntennaCollection ac;
    bool first;
    int maxX;
    while (getline(input, line) && (!debugapply || debug < debuglimit))
    {
        debug++;
        if (debugapply) {
            cout << line << endl;
        }

        // Scan each line, keep anything but a '.'
        int pos = line.find_first_not_of(".");
        while (pos != string::npos) {
            // Log match
            Antenna a;
            a.frequency = line[pos];
            a.x = pos;
            a.y = lineNr;
            ac.addAntenna(a);

            // Pos for next loop, any remaining antennas on this line
            if (pos+1 < line.length()) {
                pos = line.find_first_not_of(".", pos+1);
            }
            else {
                pos = string::npos;
            }
        }
        lineNr++;
        maxX = line.length();
    }

    // Finished with input file
    input.close();

    // Processing
    int maxY = lineNr;
    auto nodes = ac.getAntinodes(maxX, maxY);
    cout << "Found " << nodes.size() << " antinodes\n";
    unordered_map<int,set<int>> nodeXYPos;
    vector<pair<int,int>> uniqueValidNodes;
    for (auto n : nodes) {
        // Check in bounds
        int x = n.first;
        int y = n.second;
        if (x >= 0 && x < maxX && y >= 0 && y < maxY) {
            // Valid placement, check for uniqueness
            if (!nodeXYPos[x].contains(y)) {
                nodeXYPos[x].insert(y);
                uniqueValidNodes.push_back({x,y});
            }
            else {
                //cout << "Discarding duplicate node at pos " << x << "," << y << endl;
            }
        }
        else {
            //cout << "Discarding node out of bounds at pos " << x << "," << y << " because maxX maxY is " << maxX << "," << maxY << endl;
        }
    }

    // Output
    cout << "--\n";
    cout << "Found " << uniqueValidNodes.size() << " unique antinodes in bounds\n";

    cout << "--\nEnd.\n";
    return 0;
}

vector<pair<int,int>> AntennaCollection::getAntinodes(int maxX, int maxY)
{
    vector<pair<int,int>> nodes;
    cout << "Found " << antennas.size() << " unique frequencies of antennas\n";
    for (auto it = antennas.begin(); it != antennas.end(); it++) {
        // One frequency at a time in for loop
        char f = it->first;
        vector<Antenna> aVec = it->second;
        cout << "Frequency '" << f << "' count " << aVec.size() << " antennas...\n";

        // We need every pairing of antenna in the aVec for this freq.
        // Not unique pairs because antinodes are generated in both directions
        // We could easily make it unique by having j = i to start loop 2 but then
        // the antinode function would need to find both antinodes instead of just 1 per pairing
        for (int i = 0; i < aVec.size(); i++) {
            for (int j = 0; j < aVec.size(); j++) {
                if (i != j) {
                    vector<pair<int,int>> n = aVec[i].getAntiNodes(aVec[j], maxX, maxY);
                    for (auto x : n) {
                        nodes.push_back(x);
                    }
                }
            }
        }
    }

    return nodes;
}

void AntennaCollection::addAntenna(Antenna a)
{
    if (antennas.find(a.frequency) == antennas.end()) {
        // Doesn't exist yet, make an empty vector
        vector<Antenna> aVec;
        antennas[a.frequency] = aVec;
    }
    // Add new antenna to element
    antennas[a.frequency].push_back(a);
}

// Get the coord. of the antinode produced by this Antenna in the direction of Antenna b
// As of Part 2 this is now multiple nodes repeating until we go out of bounds
vector<pair<int,int>> Antenna::getAntiNodes(Antenna b, int maxX, int maxY)
{
    // P1: This occurs at the same distance used travelling from a -> b, but again from antenna b
    // P2: However since Part2 this now starts at the position of b itself, and repeats as before until
    // we go out of bounds
    vector<pair<int,int>> an;
    int deltaX = b.x - x;
    int deltaY = b.y - y;

    // First antinode (an) pos. is just the pos. of Antenna b
    int anx = b.x;
    int any = b.y;
    // Keep going as long as we are in bounds
    while (anx >= 0 && anx < maxX && any >= 0 && any < maxY) {
        an.push_back({anx, any});
        // Next antinode
        anx += deltaX;
        any += deltaY;
    }
    return an;
}


