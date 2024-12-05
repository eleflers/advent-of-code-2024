#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>

using namespace std;

int countWordAtPosX(int posX, int posY, vector<string> linearray, string word);
int searchWordInDirection(int posX, int posY, vector<string> linearray, string word, int deltaX, int deltaY, string direction);

int main(int argc, char* argv[])
{
    cout << "--\nAoC Day 4\n--\n";
    // For testing
    int debuglimit = 5;
    int debug = 0;
    bool debugapply = false;
    if (debugapply) { cout << "DEBUG MODE : ON\nLINE LIMIT : " << debuglimit << "\n--" << endl; }

    // TODO: Add debug flag detection from CLI, and check whether file exists
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
    int sumP1 = 0;
    int sumP2 = 0;

    // Read file and get numbers
    string line;
    vector<string> linearray;
    while (getline(input, line) && (!debugapply || debug < debuglimit))
    {
        debug++;
        if (debugapply) {
            //cout << line << endl;
        }

        linearray.push_back(line);
    }
    // Finished with input file
    input.close();

    // Iterate over array
    int sizeX = linearray[0].length();
    int sizeY = linearray.size();
    string searchp1 = "XMAS";
    for (size_t row = 0; row < sizeY; row++) {
        // Trying to find any word XMAS written in any direction
        // Look for 'X' to start
        int pos;
        int offset = 0;
        line = linearray[row];
        //cout << "Processing line " << row << ": '" << line << "'\n";
        pos = line.find(searchp1[0]);
        // Loop over every X in the line
        while (pos != string::npos) {
            // Found an X, process it
            sumP1 += countWordAtPosX(pos, row, linearray, searchp1);

            // Set up next cycle
            offset = pos+1;
            pos = line.find(searchp1[0], offset);
        }
    }
    for (size_t row = 0; row < sizeY; row++) {
        // Part 2:
        // Now looking for MAS in a cross with A in centre like
        //
        // ..M.S..
        // ...A...
        // ..M.S..
        //
        // So search for A's, then look for MAS and SAM from top 2 corners
        int pos;
        int offset = 0;
        line = linearray[row];
        pos = line.find("A");
        // Loop over every A in the line
        while (pos != string::npos) {
            // Found an A, process it
            int direction1 = 0;
            direction1 += searchWordInDirection(pos-1, row-1, linearray, "MAS", 1, 1, "↘");
            direction1 += searchWordInDirection(pos+1, row+1, linearray, "MAS", -1, -1, "↖");
            int direction2 = 0;
            direction2 += searchWordInDirection(pos-1, row+1, linearray, "MAS", 1, -1, "↗");
            direction2 += searchWordInDirection(pos+1, row-1, linearray, "MAS", -1, 1, "↙");

            if (direction1 == 1 && direction2 == 1) {
                // Both elements of cross are present, so record it
                sumP2++;
            }

            // Set up next cycle
            offset = pos+1;
            pos = line.find("A", offset);
        }
    }

    // Output
    cout << "--\n";
    cout << "Sum P1 = " << sumP1 << endl;
    cout << "Sum P2 = " << sumP2 << endl;
    cout << "--\nEnd.\n";
    return 0;
}

int countWordAtPosX(int posX, int posY, vector<string> linearray, string word) {
    //cout << "Given '" << word[0] << "' to process at line " << posY << " column " << posX << endl;
    int result = 0;
    result += searchWordInDirection(posX, posY, linearray, word,  1,  1, "↘");
    result += searchWordInDirection(posX, posY, linearray, word,  1,  0, "→");
    result += searchWordInDirection(posX, posY, linearray, word,  1, -1, "↗");
    result += searchWordInDirection(posX, posY, linearray, word,  0,  1, "↓");
    result += searchWordInDirection(posX, posY, linearray, word,  0, -1, "↑");
    result += searchWordInDirection(posX, posY, linearray, word, -1,  1, "↙");
    result += searchWordInDirection(posX, posY, linearray, word, -1,  0, "←");
    result += searchWordInDirection(posX, posY, linearray, word, -1, -1, "↖");
    return result;
}

int searchWordInDirection(int posX, int posY, vector<string> linearray, string word, int deltaX, int deltaY, string direction) {
    int sizeX = linearray[0].length();
    int sizeY = linearray.size();
    // Check if starting position is out of bounds, since for part 2 we are finding the middle of a word
    if (posX < 0 || posX >= sizeX) {
        //cout << "WARN: Word search at pos [" << posX << "," << posY << "] out of bounds in X direction\n";
        return 0;
    }
    if (posY < 0 || posY >= sizeY) {
        //cout << "WARN: Word search at pos [" << posX << "," << posY << "] out of bounds in Y direction\n";
        return 0;
    }
    // Check if search goes out of bounds
    // E.g. if word is "XMAS" we jump 4-1=3 times in any direction
    int wordSize = word.length();
    if ( (posX + (wordSize - 1) * deltaX) < 0 || (posX + (wordSize - 1) * deltaX) >= sizeX ) {
        //cout << "WARN: Word search at pos [" << posX << "," << posY << "] delta [" << deltaX << "," << deltaY << "] (" << direction << ") out of bounds in X direction\n";
        return 0;
    }
    if ( (posY + (wordSize - 1) * deltaY) < 0 || (posY + (wordSize - 1) * deltaY) >= sizeY ) {
        //cout << "WARN: Word search at pos [" << posX << "," << posY << "] delta [" << deltaX << "," << deltaY << "] (" << direction << ") out of bounds in Y direction\n";
        return 0;
    }

    for (int i = 0; i < wordSize; i++) {
        int x = posX + (i * deltaX);
        int y = posY + (i * deltaY);
        if (linearray[y][x] != word[i]) {
            return 0;
        }
    }
    //cout << "Found word '" << word << "' at position [" << posX << "," << posY << "] delta [" << deltaX << "," << deltaY << "] (" << direction << ")\n";
    return 1;
}

