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
        pair<int,int> getAntiNode(Antenna b);
};

class AntennaCollection {
    private:
        unordered_map<char,vector<Antenna>> antennas;
    public:
        vector<pair<int,int>> getAntinodes();
        void addAntenna(Antenna a);
};

int main(int argc, char* argv[])
{
    cout << "--\nAoC Day 3\n--\n";
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
    auto nodes = ac.getAntinodes();
    cout << "Found " << nodes.size() << " antinodes\n";
    // Both these variables should still exist...
    int maxY = lineNr;
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

vector<pair<int,int>> AntennaCollection::getAntinodes()
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
                    nodes.push_back(aVec[i].getAntiNode(aVec[j]));
                }
            }
        }
    }

    return nodes;
    //throw runtime_error("Not implemented");
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
pair<int,int> Antenna::getAntiNode(Antenna b)
{
    // This occurs at the same distance used travelling from a -> b, but again from antenna b
    int deltaX = b.x - x;
    int deltaY = b.y - y;
    pair<int,int> p = {b.x + deltaX, b.y + deltaY};
    return p;
}


