#include <iostream>
#include <fstream>
#include <algorithm> // For sort()
#include <vector>

using namespace std;

int main(int argc, char* argv[])
{
    cout << "--\nAoC Day 1\n--\n";
    // For testing
    int debuglimit = 5;
    int debug = 0;
    bool debugapply = false;
    if (debugapply) { cout << "DEBUG MODE : ON\nLINE LIMIT : " << debuglimit << "\n--" << endl; }

    // Input file
    ifstream input("input");

    // Vectors to store and sort the numbers
    vector<int> column1, column2;

    // Read file and get numbers
    string line;
    while (getline(input, line) && (!debugapply || debug < debuglimit))
    {
        debug++;
        //cout << line << endl;
        int num1, num2;
        int i = 0;
        // Assume first number starts at pos. 0, and find the first pos.
        // with no digit (ie. the length of the first number)
        // Whatever remains in the line can be parsed as the second
        while (isdigit(line[i]))
        {
            i++;
        }
        num1 = stoi(line.substr(0,i));
        num2 = stoi(line.substr(i));
        //cout << num1 << ", " << num2 << endl;

        column1.push_back(num1);
        column2.push_back(num2);
    }

    // Sort and compare them
    sort(column1.begin(), column1.end());
    sort(column2.begin(), column2.end());
    long sumDistance = 0;
    long sumSimilarity = 0;
    for (int i = 0; i < column1.size(); i++)
    {
        // cout << "Element " << i << ": " << column1[i] << ", " << column2[i] << endl;
        int distance = abs(column1[i] - column2[i]);
        sumDistance += distance;

        // How many times does num1 appear in 2nd col.?
        // This could be optimised for larger datasets, because both columns are already sorted
        int count = std::count(column2.begin(), column2.end(), column1[i]);
        // "Similarity score" is the num1 in question multiplied by its count in col. 2
        sumSimilarity += column1[i] * count;
    }

    // Output
    cout << "Sum of location distances = " << sumDistance << endl;
    cout << "Sum of similarity = " << sumSimilarity << endl;

    // Finished with input file
    input.close();

    cout << "--\nEnd.\n";
    return 0;
}

