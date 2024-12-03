#include <iostream>
#include <fstream>
//#include <algorithm>
#include <vector>
#include <regex>

using namespace std;

int process_match(string line);

int main(int argc, char* argv[])
{
    cout << "--\nAoC Day 3\n--\n";
    // For testing
    int debuglimit = 1;
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
    long sum = 0;

    string line;
    size_t pos;
    //regex searchpattern("mul\\(\\d+,\\d+\\)");
    //smatch match;
    while (getline(input, line) && (!debugapply || debug < debuglimit))
    {
        debug++;
        //cout << line << "\n--" << endl;

        // TODO: Try regex
        // Looking for exact pattern 'mul(X,Y)'
        // while (regex_search(line, match, searchpattern)) {
        //     for (auto x : match) {
        //         cout << "Regex found match '" << x << "'\n";
        //     }
        //     searchpattern = match.suffix().str();
        // }

        // Search manually
        // For a starting 'm'
        pos = line.find_first_of("m");
        int offset = 0; // Skip parts of the string we already searched
        while (pos != string::npos)
        {
            // Matched 'm'
            //cout << "Found starting 'm' line " << debug << " at pos. " << pos << endl;
            int start = pos;
            pos = line.find_first_of(")", start);
            if (pos != string::npos) {
                // Potential match
                // We don't care if it is valid or not, if invalid we just return 0
                //cout << "Found potential closing ')' line " << debug << " at pos. " << pos << endl;
                int end = pos + 1;
                int result = process_match(line.substr(start,end-start));
                sum += result;
                //line = line.substr(end); // For next cycle of loop

                // Next match?
                // Can do this here so it only happens if there's at least 1 ")" remaining in the line
                // to save on checking pointless "m"s
                if (result > 0)
                    offset = end;
                else
                    offset = start + 1;
                pos = line.find_first_of("m", offset);
            }
        }
    }

    // Output
    cout << "--\n";
    cout << "Sum = " << sum << endl;

    // Finished with input file
    input.close();

    cout << "--\nEnd.\n";
    return 0;
}

// Takes any string starting 'm' ending ')'. Returns 0 if invalid, else returns result of multiplication
int process_match(string line)
{
    // Here we check if the string we have been handed fits the pattern
    // and parse 2 numbers from it if so, return the multiplication
    //
    // Pattern = 'mul(XXX,YY)'
    //                ^  ^^
    //                |  ||
    //   posNum1 -----'  ||
    //   posDelim -------'|
    //   posNum2 ---------'

    //cout << "Given string '" << line << "' for analysis...\n";

    // We already know it starts in 'm' and ends with ')'
    size_t posNum1 = 4;
    size_t posDelim, posNum2;
    size_t pos;
    if (line.substr(0,posNum1) != "mul(") {
        return 0;
    }
    // Now we know we have "mul("
    // Look for a delimiting comma
    pos = line.find_first_of(",");
    if (pos == string::npos) {
        return 0;
    }
    // Also found a comma
    posDelim = pos;
    posNum2 = posDelim + 1;
    // Calculate lengths of the spaces left for numbers
    // Remember to account for nasty special cases like "mul(123,)" that may try to trick us
    // Last char. of string we know is ')' already, and it occurs at pos. line.length() - 1
    int lenNum1 = posDelim - posNum1;
    int lenNum2 = ( line.length() - 1 ) - posNum2;
    if (lenNum1 == 0 || lenNum2 == 0)
        return 0;
    // Test for digits
    // 1st
    for (int i = posNum1; i < posDelim; i++) {
        if (!isdigit(line[i]))
            return 0;
    }
    // 2nd
    for (int i = posNum2; i < line.length()-1; i++) {
        if (!isdigit(line[i]))
            return 0;
    }

    // We now know we have a valid string
    // Parse the numbers and multiply
    int num1 = stoi(line.substr(posNum1, posDelim));
    int num2 = stoi(line.substr(posNum2, line.length()-1));
    return num1 * num2;
}

