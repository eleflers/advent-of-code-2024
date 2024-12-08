#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <deque>
#include <stack>
#include <sstream>

using namespace std;

long long testEquations(stack<long long> numbers, long long result);
long long combineNumbersAsStrings(long long a, long long b);

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
    stack<long long> numStack;
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
                // Put on the front of the queue so the order is reversed, we will read it into
                // a stack afterwards so the recursion is more efficient
                num.emplace_front(stoll(n));
            }
        }
        // Reverse order back onto a stack so it comes off in correct order
        while (!num.empty()) {
            numStack.push(num.front());
            num.pop_front();
        }
        // Process
        //cout << "Found " << num.size() << " numbers in line " << debug << " for result " << result << endl;
        // Try all possible operators ('+' and '*' for part 1)
        sum += testEquations(numStack, result);

        // Clear queue for next time
        stack<long long> empty;
        swap(numStack, empty);
    }

    // Output
    cout << "--\n";
    cout << "Sum = " << sum << endl;

    // Finished with input file
    input.close();

    cout << "--\nEnd.\n";
    return 0;
}

long long testEquations(stack<long long> numbers, long long result)
{
    if (numbers.size() == 2) {
        //cout << "testEq called with 2 queue elements\n";
        long long first = numbers.top(); numbers.pop();
        if (first + numbers.top() == result 
                    || first * numbers.top() == result
                    || combineNumbersAsStrings(first, numbers.top()) == result) {
            return result;
        }
        else {
            return 0;
        }
    }
    else if (numbers.size() > 2) {
        long long first = numbers.top(); numbers.pop();
        long long second = numbers.top(); numbers.pop();
        //cout << "1: " << first << ", 2: " << second << ", 3: ?, calling recursively..." << endl;
        stack<long long> newQueue = {numbers};
        stack<long long> newQueueP2 = {numbers};
        numbers.push(first + second);
        newQueue.push(first * second);
        newQueueP2.push(combineNumbersAsStrings(first, second));

        if (testEquations(numbers, result) == result
                    || testEquations(newQueue, result) == result
                    || testEquations(newQueueP2, result) == result) {
            return result;
        }
        else {
            return  0;
        }
    }
    else if (numbers.size() == 1) {
        // Maybe there is an odd special case where we are given 1 number
        if (numbers.top() == result) {
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

long long combineNumbersAsStrings(long long a, long long b)
{
    ostringstream os;
    os << a << b;
    return stoll(os.str());
}

