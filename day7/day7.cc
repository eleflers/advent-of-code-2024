#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <deque>

using namespace std;

long long testEquations(deque<long long> numbers, long long result);

int main(int argc, char* argv[])
{
    cout << "--\nAoC Day 7\n--\n";
    // For testing
    int debuglimit = 2;
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
    long long sum = 0;

    // Read file
    string line;
    deque<long long> num;
    while (getline(input, line) && (!debugapply || debug < debuglimit))
    {
        debug++;
        if (debugapply) {
            cout << line << endl;
        }

        // Read result
        auto pos = line.find(":");
        long long result = stoll(line.substr(0,pos));
        //cout << "Result: " << result << endl;
        // Loop rest of numbers
        while (pos != line.length()) {
            // Find next num
            while (!isdigit(line[pos]) && pos != line.length()) {
                pos++;
            }
            // Now at start of next num or end of line
            if (pos != line.length()) {
                string n = "";
                while (isdigit(line[pos]) && pos != line.length()) {
                    n += line[pos];
                    pos++;
                }
                //cout << "Parsed number " << n << endl;
                num.emplace_back(stoi(n));
            }
        }
        // Process
        //cout << "Found " << num.size() << " numbers in line " << debug << " for result " << result << endl;
        // Try all possible operators ('+' and '*' for part 1)
        sum += testEquations(num, result);

        // Clear queue for next time
        deque<long long> empty;
        swap(num, empty);
    }

    // Output
    cout << "--\n";
    cout << "Sum = " << sum << endl;

    // Finished with input file
    input.close();

    cout << "--\nEnd.\n";
    return 0;
}

long long testEquations(deque<long long> numbers, long long result)
{
    if (numbers.size() == 2) {
        //cout << "testEq called with 2 queue elements\n";
        long long first = numbers.front(); numbers.pop_front();
        if (first + numbers.front() == result  || first * numbers.front() == result) {
            return result;
        }
        else {
            return 0;
        }
    }
    else if (numbers.size() > 2) {
        long long first = numbers.front(); numbers.pop_front();
        long long second = numbers.front(); numbers.pop_front();
        //cout << "1: " << first << ", 2: " << second << ", 3: ?, calling recursively..." << endl;
        deque<long long> newQueue = {numbers};
        numbers.emplace_front(first + second);
        newQueue.emplace_front(first * second);

        if (testEquations(numbers, result) == result || testEquations(newQueue, result) == result) {
            return result;
        }
        else {
            return  0;
        }
    }
    else if (numbers.size() == 1) {
        // Maybe there is an odd special case where we are given 1 number
        if (numbers.front() == result) {
            return result;
        }
        else {
            return 0;
        }
    }
    else {
        throw runtime_error("0 elements in the queue");
    }
}

