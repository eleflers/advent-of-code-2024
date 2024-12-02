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

        // Read numbers TODO: Make general function for this year
        int i = 0, j = 0;
        while (i < line.length())
        {
            // i = start of no.
            // j = end of no. (i.e. pos. of char. after last digit, (j-i)=len)
            while (j < line.length() && isdigit(line[j]))
            {
                j++;
            }
            // Found digit from pos. i to j
            num.push_back(stoi(line.substr(i,j-i)));
            //cout << "Parsed number: '" << line.substr(i,j-i) << "'\n";
            // Find start of next number first
            i = j;
            while (i < line.length() && !isdigit(line[i]))
                i++;
            // Either found start of next no. or hit the end of the string
            j = i;
        }
        //cout << "Found " << num.size() << " num" << endl;

        // Problem logic here
        bool safe = false;
        bool allAsc = true, allDesc = true;
        int levelCount = num.size();
        for (int i = 1; i < levelCount; i++)
        {
            if (num[i] > num[i-1])
                allDesc = false;
            if (num[i] < num[i-1])
                allAsc = false;
        }
        if (allAsc || allDesc)
        {
            safe = true; // safe for now, but check all differences are between 1 and 3
            for (int i = 1; i < levelCount; i++)
            {
                if (abs(num[i] - num[i-1]) == 0 || abs(num[i] - num[i-1]) > 3)
                    safe = false;
            }
        }

        if (safe)
            sum += 1;
    }

    // Output
    cout << "--\n";
    cout << "Sum of safe reports = " << sum << endl;

    // Finished with input file
    input.close();

    cout << "--\nEnd.\n";
    return 0;
}

