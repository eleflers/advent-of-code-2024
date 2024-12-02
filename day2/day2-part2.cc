#include <iostream>
#include <fstream>
#include <algorithm> // For sort()
#include <vector>

using namespace std;

int main(int argc, char* argv[])
{
    cout << "--\nAoC Day 2\n--\n";
    // For testing
    int debuglimit = 5;
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
        int i = 0, j = 0;
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
        bool isAsc;
        bool isDesc;

        // Make a sorted copy of the vector so we don't modify the original
        vector<int> numSort = num;
        sort(numSort.begin(), numSort.end());

        // If they match, we are ascending
        if (num == numSort) {
            //cout << "Vectors are identical" << endl;
            isAsc = true;
        }
        else {
            isAsc = false;
            // May be descending instead, check by reversing sorted array
            reverse(numSort.begin(), numSort.end());
            if (num == numSort) {
                isDesc = true;
            }
            else {
                isDesc = false;
            }
        }

        //cout << "Vector orig:\n";
        //for (int i = 0; i < num.size(); i++) {
        //    cout << num[i] << endl;
        //}
        //cout << "Vector copy:\n";
        //for (int i = 0; i < numSort.size(); i++) {
        //    cout << numSort[i] << endl;
        //}

        // Check that difference between successive values is no more than 3, and not 0
        safe = isAsc || isDesc;
        if (safe) {
            for (size_t i = 1; safe && i < num.size(); i++) {
                if (abs(num[i] - num[i-1]) == 0 || abs(num[i] - num[i-1]) > 3) {
                    safe = false;
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

