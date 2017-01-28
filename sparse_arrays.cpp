#include <string>
#include <map>
#include <iostream>

using namespace std;


int readInt() {
    string input_line;
    getline(cin, input_line);
    return stoi(input_line);
}

int main() {
    int num_strings = readInt();
    string input_line;
    auto strings = map<string, int>();

    for (int i = 0; i < num_strings; ++i ) {
        getline(cin, input_line);
        strings[input_line] += 1;
    }

    int num_queries = readInt();
    for (int i = 0; i < num_queries; ++i ) {
        getline(cin, input_line);
        cout << strings[input_line] << endl;
    }
}
