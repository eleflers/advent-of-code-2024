#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>

using namespace std;

class Antenna {
    public:
        char frequency;
        int x;
        int y;
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
    }

    // Finished with input file
    input.close();

    // Processing
    auto nodes = ac.getAntinodes();

    // Output
    cout << "--\n";
    cout << "Found " << nodes.size() << " antinodes\n";

    cout << "--\nEnd.\n";
    return 0;
}

vector<pair<int,int>> AntennaCollection::getAntinodes()
{
    cout << "Not implemented yet, but I have " << antennas.size() << " unique frequencies of antennas to report\n";
    vector<pair<int,int>> nodes;
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

