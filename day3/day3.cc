#include <iostream>
#include <fstream>
//#include <algorithm>
#include <vector>
//#include <regex>
#include <queue>

using namespace std;

int process_match(string line);

int main(int argc, char* argv[])
{
    cout << "--\nAoC Day 3\n--\n";
    // Part 2: 111905416 - too high
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
    // TODO: Disable may cross line boundaries, be prepared to check status at the
    // end of this while loop and log it for the next iteration to start out disabled
    while (getline(input, line) && (!debugapply || debug < debuglimit))
    {
        debug++;
        //cout << line << "\n--" << endl;

        // Part 2
        // Look for "do()" and "don't()" strings in the text
        // A line starts out true by default, but after a "don't()" string
        // any mulitplications are ignored until a "do()" string occurs
        pos = line.find("don't()");
        queue<int> disablePos;
        queue<int> enablePos;
        if (pos != string::npos) {
            // Found a deactivation, so we need to log everything
            while (pos != string::npos) {
                disablePos.push(pos);
                //cout << "Logged a don't() string at line " << debug << " pos " << pos << endl;
                pos = line.find("don't()", pos+1);
            }
            pos = 0;
            do {
                enablePos.push(pos);
                //cout << "Logged a do() string at line " << debug << " pos " << pos << endl;
                pos = line.find("do()", pos+1);
            } while (pos != string::npos);
        }
        // Find disable zones
        queue<pair<int,int>> disableZones;
        while (!disablePos.empty()) {
            int start = disablePos.front();
            disablePos.pop();
            // We are now in a disable zone
            // Find end, if any
            int end = start;
            while (!enablePos.empty() && end <= start) {
                end = enablePos.front();
                enablePos.pop();
            }
            if (end == start) {
                // Didn't find an end, so set to end of line
                // TODO: This may not be the correct thing to do for the solution answer!
                end = line.length();
            }
            pair<int,int> zone = {start, end};
            disableZones.push(zone);
            //cout << "Found disable zone from start: " << start << " to end: " << end << endl;
            // Need to discard any following disables that are in this interval before continuing
            // as they happen in a region that is already disabled
            if (!disablePos.empty())
                start = disablePos.front();
            while (!disablePos.empty() && start < end) {
                // Discard it
                disablePos.pop();
                start = disablePos.front();
            }
        }

        // Search manually
        // For a starting 'm'
        //cout << "Looking for first 'm'" << endl;
        pos = line.find_first_of("m");
        int offset = 0; // Skip parts of the string we already searched
        while (pos != string::npos && pos != line.length())
        {
            // Matched 'm'
            //cout << "Found starting 'm' line " << debug << " at pos. " << pos << endl;
            int start = pos;
            pos = line.find_first_of(")", start);
            if (pos != string::npos) {
                // Part 2, we may be in an invalid region of the line
                // Check this first
                bool inDisabledZone;
                while (!disableZones.empty() && disableZones.front().second < pos) {
                    // Discard all zones that end before we start
                    //cout << "Discarding zone " << disableZones.front().first << ", " << disableZones.front().second << endl;
                    disableZones.pop();
                }
                if (disableZones.empty()) {
                    inDisabledZone = false;
                } else {
                    inDisabledZone = ( disableZones.front().first < pos );
                    // Don't need to check (disableZones.front().second > pos), because the while loop did it above
                }

                if (inDisabledZone) {
                    // Don't bother checking the string
                    // We can just jump straight to the end of the disabled zone before
                    // doing any more string searching
                    offset = disableZones.front().second;
                    //cout << pos << " is in disabled zone\n";
                }
                else {
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
                }
                if (offset != line.length()) {
                    pos = line.find_first_of("m", offset);
                }
                else {
                    pos = string::npos;
                }
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

