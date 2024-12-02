#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>

using namespace std;

bool isLevelSafe(vector<int> level);
bool isLevelAscOrDesc(vector<int> level);

int main(int argc, char* argv[])
{
    cout << "--\nAoC Day 2\n--\n";
    // For testing
    int debuglimit = 1;
    int debug = 0;
    bool debugapply = false;
    if (debugapply) { cout << "DEBUG MODE : ON\nLINE LIMIT : " << debuglimit << "\n--" << endl; }

    // Input file
    ifstream input("input");

    // Variables for output
    int sum = 0;

    // Read file and get numbers
    string line;
    while (getline(input, line) && (!debugapply || debug < debuglimit))
    {
        debug++;
        //cout << line << endl;

        vector<int> num;

        // TODO: Make general function number parser for this year
        // Read numbers
        size_t i = 0, j = 0;
        while (i < line.length())
        {
            // i = start of no.
            // j = end of no. (i.e. pos. of char. after last digit, (j-i)=len)
            while (j < line.length() && isdigit(line[j])) {
                j++;
            }
            // Found digit from pos. i to j
            num.push_back(stoi(line.substr(i,j-i)));
            //cout << "Parsed number: '" << line.substr(i,j-i) << "'\n";
            // Find start of next number first
            i = j;
            while (i < line.length() && !isdigit(line[i])) {
                i++;
            }
            // Either found start of next no. or hit the end of the string
            j = i;
        }
        //cout << "Found " << num.size() << " num" << endl;

        // Program logic
        // Find out if all asc or desc
        bool safe;
        bool isAscOrDesc;

        // Keep track of whether we need subarrays
        int removedItems;
        vector<vector<int>> listValidSubArrays;

        isAscOrDesc = false;
        // Are we already asc or desc?
        if (isLevelAscOrDesc(num)) {
            isAscOrDesc = true;
            removedItems = 0;
        }
        else {
            // Part 2: Tolerate 1 incorrect element, try removing each in turn
            // to see if we still end up with valid sub-arrays
            // The logic around which of the possible elements should be removed is difficult
            // to know in advance when combined with the 2nd part of part 2 involving
            // the differences between elements. So just store them all to try later
            for (size_t i = 0; i < num.size(); i++) {
                vector<int> subList = num;
                // Remove element i
                subList.erase(subList.begin() + i);
                // Check validity
                if (isLevelAscOrDesc(subList)) {
                    //cout << "Found a valid sublist by removing element " << i << " of line " << debug << endl;
                    listValidSubArrays.push_back(subList);
                    removedItems = 1;
                    isAscOrDesc = true;
                }
            }
        }

        // Now check 2nd part, difference between successive values
        // We already know our list (or sublists) are asc/desc now
        safe = false;
        if (isAscOrDesc) {
            if (removedItems == 0) {
                // We didn't remove any items, so we can check our original vector
                safe = isLevelSafe(num);
                if (!safe) {
                    // If we failed here, we still one last chance to remove an element
                    // So add all those to the list of sub arrays for testing
                    for (size_t i = 0; i < num.size(); i++) {
                        vector<int> subList = num;
                        subList.erase(subList.begin() + i);
                        listValidSubArrays.push_back(subList);
                        removedItems = 1;
                    }
                }
            }
            // Might have some sub arrays to test, either from asc/desc stage or above
            // Make sure we don't do this if we are already found to be safe above
            if (!safe && removedItems == 1 && listValidSubArrays.size() > 0) {
                // We have some valid sub arrays to check
                for (size_t i = 0; !safe && i < listValidSubArrays.size(); i++) {
                    if (isLevelSafe(listValidSubArrays[i]))
                        safe = true;
                }
            }
        }

        // Keep running sum
        if (safe)
            sum++;
    }

    // Output
    cout << "--\n";
    cout << "Sum of safe reports = " << sum << endl;

    // Finished with input file
    input.close();

    cout << "--\nEnd.\n";
    return 0;
}

bool isLevelAscOrDesc(vector<int> level)
{
    // Check that the vector is a run of straight asc or desc values
    // Try asc first
    bool result = true;
    for (size_t i = 1; result && i < level.size(); i++) {
        if (level[i] <= level[i-1]) {
            result = false;
        }
    }
    // If asc, return now
    if (result)
        return true;
    // Try desc, since we are not asc
    result = true;
    for (size_t i = 1; result && i < level.size(); i++) {
        if (level[i] >= level[i-1]) {
            result = false;
        }
    }
    return result;
}

bool isLevelSafe(vector<int> level)
{
    // Check that difference between successive values is no more than 3, and not 0
    // This function assumes that the array is already valid and sorted (ie. ascending
    // or descending order)
    for (size_t i = 1; i < level.size(); i++) {
        if (abs(level[i] - level[i-1]) == 0 || abs(level[i] - level[i-1]) > 3) {
            return false;
        }
    }
    return true;
}

