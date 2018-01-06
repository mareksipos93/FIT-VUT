/**
** tail.cpp
** Solution: IJC-DU2, task a), 10.04 2016
** Author: Marek Sipos (xsipos03), FIT VUT (BUT)
** Compiled with: gcc 4.9.2, w/o optimization
*/

#include <cstdlib> // Because of exit()
#include <fstream>
#include <iostream>
#include <queue> // Queue is used to store loaded lines
#include <sstream>
#include <string>
#include <vector> // Vector is used to convert arguments to string

using namespace std;

// Convert string to long int
bool strtolong(string s, long *result) {
    long casted;
    istringstream ss(s);
    if (ss >> casted) {
        *result = casted;
        return true;
    }
    else {
        return false;
    }
}

// Handle file input
void printtail(istream &input, unsigned long printlines) {

    // Nothing to print
    if (printlines == 0) {
        return;
    }

    // Initialization
    queue<string> q;
    string temp;
    size_t numoflines;

    // Get line by line input
    do {
/* */   getline(input, temp);
        if (input.fail() && !input.eof()) {
            cerr << "Neznama chyba pri cteni dat" << endl;
            exit(1);
        }

        q.push(temp);
    }
    while (!input.eof());

    numoflines = q.size();

    // Skip last empty line
    if (q.back().length() == 0) {
        numoflines--;
    }

    // We can output only lines which are present in queue
    if (printlines > numoflines)
        printlines = numoflines;

    // Get rid of unwanted lines
    while (numoflines != printlines) {
        q.pop();
        numoflines--;
    }
    // Print results
    while (numoflines != 0) {
        temp = q.front();
        cout << temp << endl;
        q.pop();
        numoflines--;
    }

    return;
}

int main(int argc, char *argv[]) {

    ios::sync_with_stdio(false); // Disable sync with IO from C

    // Convert arguments to strings
    vector<string> args(argc);
    for (int i = 0; i < argc; i++){
        args[i] = argv[i];
    }

    long lines = 10; // Amount of lines to print
    int argindex = 0; // Last file argument index to handle
    ifstream f; // File to be opened
    if (argc > 1) {
        // Handle arguments
        for (int i = 1; i < argc; i++) {
            // '-' = direct input
            if (args[i].compare("-") == 0) { // Direct input
                argindex = i;
            }
            // '-n' = number of lines to print
            else if (args[i].compare("-n") == 0) {
                if (argv[i+1] != NULL) { // Argument i+1 with number
                    if (strtolong(args[i+1], &lines)) { // Is valid number
                        if (lines < 0) // Negative = Positive (like POSIX)
                            lines = -lines;
                        i++; // argument i+1 was handled
                    }
                    else {
                        cerr << "tail: '" << args[i+1] << "': Chybny pocet radku" << endl;
                        exit(1);
                    }
                }
                else {
                    cerr << "tail: prepinac vyzaduje argument - 'n'" << endl;
                    exit(1);
                }
            }
            // Unknown argument starting with '-'
            else if (argv[i][0] == '-') {
                    cerr << "tail: neplatny prepinac " << args[i] << endl;
                    exit(1);
            }
            // This should be filename to open
            else {
                argindex = i;
            }
        }
    }
    // Direct input
    if (argindex == 0 || args[argindex].compare("-") == 0) {
        istream *input = &cin; // Istream 'input' now works with direct input
        printtail(*input, lines);
    }
    // File input
    else {
        f.open(argv[argindex], ifstream::in); // in = read only
        if (f.fail() || !f.is_open()) {
            cerr << "tail: '" << args[argindex] << "' nelze otevrit pro cteni" << endl;
            exit(1);
        }

        istream *input = &f; // Istream 'input' now works with ifstream 'f'
        printtail(*input, lines);
        // Dont need to close ifstream, destructor handles it
    }
    return 0;
}
