#include <iostream>
#include <fstream>
//#include <algorithm>
#include <vector>
//#include <regex>
#include <queue>

using namespace std;

int process_match(string line);
queue<int> find_all_substr_pos(string line, string search);
queue<pair<int,int>> find_disable_zones(queue<int> disablePos, queue<int> enablePos, bool startDisabled, int linelength);
bool is_pos_in_zone(int pos, queue<pair<int,int>>& zones);

int main(int argc, char* argv[])
{
    cout << "--\nAoC Day 3\n--\n";
    // Part 2: 111905416 - too high
    // Part 2: 143174327 - too high, a couple lines were disabled at end of line so carried over
    // Part 2: 94564792 - refactor into functions, now too low

    // For testing
    int debuglimit = 1;
    int debug = 0;
    bool debugapply = false;
    if (debugapply) { cout << "DEBUG MODE : ON\nLINE LIMIT : " << debuglimit << "\n--" << endl; }

    // Command line args
    // TODO: Add debug flag detection from CLI, and check whether file exists
    string filename = "input";
    if (argc == 1) {
        cout << "Assume default input file '" << filename << "'\n";
    }
    else if (argc > 1) {
        filename = argv[1];
        cout << "Taking CLI input file name '" << filename << "'\n";
    }

    // Variables for output
    long sum = 0;

    // Input file
    ifstream input(filename);
    string line;
    size_t pos;

    // Disable may cross line boundaries, be prepared to check status at the
    // end of this while loop and log it for the next iteration to start out disabled
    bool startDisabled = false;
    bool persistDisabledStateAcrossLines = true;
    while (getline(input, line) && (!debugapply || debug < debuglimit))
    {
        debug++;
        //cout << line << "\n--" << endl;

        // Part 2
        // Look for "do()" and "don't()" strings in the text
        // A line starts out true by default, but after a "don't()" string
        // any mulitplications are ignored until a "do()" string occurs
        queue<int> disablePos = find_all_substr_pos(line, "don't()");
        queue<int> enablePos = find_all_substr_pos(line, "do()");
        // Find disable zones
        queue<pair<int,int>> disableZones = find_disable_zones(disablePos, enablePos, startDisabled, line.length());

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
            // Look for a closing bracket for a potential match
            pos = line.find_first_of(")", start);
            if (pos != string::npos) {
                // Dealing with potential mul(X,Y)

                // Part 2, we may be in an invalid region of the line
                // Check this first
                bool inDisabledZone = is_pos_in_zone(pos, disableZones);

                if (inDisabledZone) {
                    // Don't bother checking the string
                    // We can just jump straight to the end of the disabled zone before
                    // doing any more string searching
                    offset = disableZones.front().second;
                    //cout << pos << " is in disabled zone\n";
                }
                else {
                    // Potential match & not in disable zone
                    // We don't care if it is valid or not, if invalid we just return 0
                    //cout << "Found potential closing ')' line " << debug << " at pos. " << pos << endl;
                    int length = (pos + 1) - start;
                    int result = process_match(line.substr(start, length));
                    sum += result;

                    // Update offset
                    // If valid match, we can start after the end of the entire match string
                    // Else we can only offset to the char. after the first 'm'
                    if (result > 0)
                        offset = start + length;
                    else
                        offset = start + 1;
                }
                // Find next 'm' for next loop iteration
                if (offset != line.length()) {
                    pos = line.find_first_of("m", offset);
                }
                else {
                    pos = string::npos;
                }
            }
        }

        // Finished with entire line
        // Carryover last status if enabled
        if (persistDisabledStateAcrossLines && !disableZones.empty() && disableZones.front().second == line.length()) {
            // Then there was an active disable zone in place at the end of the line
            cout << "Warning: Line " << debug << " was DISABLED at end of line, persisting across to next line\n";
            startDisabled = true;
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

queue<int> find_all_substr_pos(string line, string search)
{
    queue<int> result;
    size_t pos = line.find(search);
    while (pos != string::npos) {
        // Found something, so log and repeat until we run out
        result.push(pos);
        //cout << "Logged a '" << search << "' string at line " << debug << " pos " << pos << endl;
        pos = line.find(search, pos+1);
    }

    return result;
}

queue<pair<int,int>> find_disable_zones(queue<int> disablePos, queue<int> enablePos, bool startDisabled, int linelength)
{
    queue<pair<int,int>> result;

    while (startDisabled || !disablePos.empty()) {
        int start;
        if (startDisabled) {
            start = 0;
            startDisabled = false;
        }
        else {
            start = disablePos.front();
            disablePos.pop();
        }
        // We are now in a disable zone
        // Find end, if any
        int end = string::npos;
        while (!enablePos.empty() && (end == string::npos || end < start) ) {
            end = enablePos.front();
            enablePos.pop();
        }
        // Loop above keeps running until we have end >= start or we run out of enables
        // Check what we ended up with
        if (end == string::npos || end < start) {
            // Didn't find an end, so set to end of line
            end = linelength;
        }
        // Make and add what we found to the list
        // Unless start and end are the same pos. in which case we don't bother (line started out disabled
        // but was immediately overruled by a "do()" at pos. 0 for e.g.)
        if (start != end) {
            pair<int,int> zone = {start, end};
            result.push(zone);
            //cout << "Found disable zone from start: " << start << " to end: " << end << endl;
        }
        // Need to discard any following disables that are in this interval before continuing
        // as they happen in a region that is already disabled
        while (!disablePos.empty() && disablePos.front() < end) {
            // Discard it
            disablePos.pop();
        }
    }

    return result;
}

// Also discards any zones to the left of pos for efficiency
bool is_pos_in_zone(int pos, queue<pair<int,int>>& zones) {
    // Discard all zones to the left of us
    while (!zones.empty() && zones.front().second < pos) {
        //cout << "Discarding zone " << zones.front().first << ", " << zones.front().second << endl;
        zones.pop();
    }
    // Check remaining zones (if any) to see if we are inside one
    if (!zones.empty()) {
        // Don't need to check (disableZones.front().second > pos), because the while loop did it above
        return ( zones.front().first <= pos );
    } else {
        return false;
    }
}

