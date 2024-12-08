#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <deque>

using namespace std;

int testEquations(deque<int> numbers, int result);

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
    int sum = 0;

    // Read file
    string line;
    deque<int> num;
    while (getline(input, line) && (!debugapply || debug < debuglimit))
    {
        debug++;
        if (debugapply) {
            cout << line << endl;
        }

        // Read result
        auto pos = line.find(":");
        int result = stoi(line.substr(0,pos));
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
        deque<int> empty;
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

int testEquations(deque<int> numbers, int result)
{
    if (numbers.size() == 1) {
        return numbers.front();
    }
    // else (numbers.size() > 1)
    int first = numbers.front();
    numbers.pop_front();
    if ((first + testEquations(numbers, result)) == result || (first * testEquations(numbers, result)) == result) {
        return result;
    }
    else {
        return 0;
    }
}

//int testEquations(deque<int> numbers, int result)
//{
//    if (numbers.size() == 2) {
//        cout << "testEq called with 2 queue elements\n";
//        int first = numbers.front(); numbers.pop_front();
//        int second = numbers.front(); numbers.pop_front();
//        cout << "1: " << first << ", 2: " << second << endl;
//        if (first + second == result) {
//            return result;
//        }
//        else if (first * second == result) {
//            return result;
//        }
//        else {
//            return 0;
//        }
//    }
//    else if (numbers.size() > 2) {
//        cout << "testEq called with " << numbers.size() << " queue elements\n";
//        int first = numbers.front(); numbers.pop_front();
//        int second = numbers.front(); numbers.pop_front();
//        cout << "1: " << first << ", 2: " << second << ", 3: ?, calling recursively..." << endl;
//        deque<int> newQueue = {numbers};
//        numbers.emplace_front(first + second);
//        newQueue.emplace_front(first * second);
//
//        // TODO: 0 could be a valid response though, could have
//        //   12 = 0 + 6 * 2
//        //   12 = 0 * 747 + 6 * 2
//        // and so on...
//        // but that should be fine since it won't resolve to actually 0 unless 0 is the intended result
//        // after all the recursion?
//        if (testEquations(numbers, result) != 0 || testEquations(newQueue, result) != 0) {
//            return result;
//            // Can't just return the sum because they could both be valid solutions, only need 1
//        }
//    }
//}

