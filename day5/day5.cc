#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <sstream>
#include <unordered_map>

using namespace std;

vector<int> parseNumbers(string input, char delimiter);
bool containsElement(vector<int> input, int search, int& pos);
bool checkValidList(vector<int> list, unordered_multimap<int, int> pageOrderPairs, pair<int,int>& invalidIndices);

int main(int argc, char* argv[])
{
    cout << "--\nAoC Day 5\n--\n";
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
    int sum = 0;
    int sumP2 = 0;

    // Read file
    string line;
    unordered_multimap<int,int> pageOrderPairs;
    vector<vector<int>> pageUpdateLists;
    // 2 component sections to file
    //     1st: Page ordering rules: 12|34
    //     2nd: "Update" page numbers: 12,34,56,78
    // There is a blank line between the sections so loop until we hit that (line != "" or length() == 0)
    // No whitespace, but every other line starts with a digit so could test that if needed

    // Section 1
    while (getline(input, line) && line.length() != 0 && (!debugapply || debug < debuglimit))
    {
        debug++;
        //cout << "Line: " << debug << ": " << line << endl;

        // Inside 1st section: Page ordering rules: 12|34
        vector<int> num = parseNumbers(line, '|');
        if (num.size() != 2) {
            cout << "ERROR: Expect 2 numbers delimited by '|' in section 1 line " << debug << endl;
            return 1;
        }
        pageOrderPairs.insert({num[0], num[1]});
    }

    // Section 2
    cout << "Blank line between section 1 and 2 found on line: " << debug + 1 << endl;
    while (getline(input, line) && (!debugapply || debug < debuglimit))
    {
        debug++;
        //cout << "Line: " << debug << ": " << line << endl;

        // Inside 2nd section: "Update" page numbers: 12,34,56,78
        vector<int> pagelist = parseNumbers(line, ',');
        pageUpdateLists.push_back(pagelist);
    }

    // Process data
    for (auto list : pageUpdateLists) {
        // Inside 1 list of page no's
        pair<int,int> invalidIndices;
        bool validList = checkValidList(list, pageOrderPairs, invalidIndices);

        if (validList) {
            // Add middle value to sum
            sum += list[(list.size()-1)/2];
        }
        else {
            // Part 2: Fix invalid lists
            // Lets try a naive swap of the offending indices, and see if it converges
            // Need to start check from the beginning each time since any 2 positions can be swapped even those
            // already checked
            int convergeLimit = 1000;
            int i = 0;
            while (!validList && i++ < convergeLimit) {
                // Naive swap
                int swap = list[invalidIndices.first];
                list[invalidIndices.first] = list[invalidIndices.second];
                list[invalidIndices.second] = swap;
                // Send for rechecking as many times as needed
                validList = checkValidList(list, pageOrderPairs, invalidIndices);
            }
            // Either worked or we exhausted our tries after getting stuck in a loop
            if (validList) {
                // Part 2 sum, take middle number
                sumP2 += list[(list.size()-1)/2];
            }
            else if (i >= convergeLimit) {
                cout << "ERROR: Hit max swap attempts of " << convergeLimit << " for list" << endl;
                return 1;
            }
        }
    }

    // Output
    cout << "--\n";
    cout << "SumP1 = " << sum << endl;
    cout << "SumP2 = " << sumP2 << endl;

    // Finished with input file
    input.close();

    cout << "--\nEnd.\n";
    return 0;
}

vector<int> parseNumbers(string input, char delimiter)
{
    vector<int> output;
    string num;
    stringstream ss(input);
    while (getline(ss, num, delimiter)) {
        if (num.length() != 0) {
            // Make sure a trailing delimiter isn't tacked onto the end of a line
            output.push_back(stoi(num));
        }
    }
    return output;
}

bool containsElement(vector<int> input, int search, int& pos)
{
    auto p = find(input.begin(), input.end(), search);
    if (p != input.end()) {
        ptrdiff_t d = p - input.begin();
        pos = d;
        return true;
    }
    else {
        return false;
    }
}

bool checkValidList(vector<int> list, unordered_multimap<int, int> pageOrderPairs, pair<int,int>& invalidIndices)
{
    // After looping all elements to check if they are left position pairs
    // we know if it is a valid list or not
    // Don't need to check for right values because any that occur we know do not
    // have corresponding left values in our list or we would have processed them already
    bool validList = true;
    for (int leftpos = 0; validList && leftpos < list.size(); leftpos++) {
        int left = list[leftpos];
        // For 1 page no. in list
        // Check to see if it is the 1st of any pairing
        if (pageOrderPairs.contains(left)) {
            // Page is a key (leftside) in a pairing, loop over them
            auto range = pageOrderPairs.equal_range(left);
            for (auto it = range.first; it != range.second; it++) {
                // Check to see if the 2nd of the pair is after the 1st in our list
                int rightpos;
                int right = it->second;
                if (containsElement(list, right, rightpos)) {
                    //cout << "For key " << left << " pos. " << leftpos << " in list found value " << right << " at pos. " << rightpos << endl;
                    if (leftpos > rightpos) {
                        validList = false;
                        invalidIndices.first = leftpos;
                        invalidIndices.second = rightpos;
                    }
                }
            }
        }
    }
    return validList;
}
