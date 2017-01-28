#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <set>

using namespace std;

static const auto DICT_PATH = "/usr/share/dict/words";

class Trie {

public:

    /* 
     * Factory function to build a trie for all english words
     */
    static Trie fromDict() {
        Trie t;
        ifstream fin(DICT_PATH, ios::in);
        string line;

        while (fin >> line) {
            transform(line.begin(), line.end(), line.begin(), ::tolower);
            t.addWord(line);
        }

        fin.close();
        return t;
    }

    Trie(): isWordNode(false) {
    }

    bool isWord(const string& s) const {
        if (s.length()) {
            if (children.count(s[0])) {
                return children.at(s[0]).isWord(s.substr(1));
            } else {
                return false;
            }
        } else {
            return isWordNode;
        }
    }

    void addWord(const string& s) {
        if (s.length()) {
            auto key = s[0];
            if (children.find(key) == children.end()) {
                children[key] = Trie();
            }
            children[key].addWord(s.substr(1));
        } else {
            isWordNode = true;
        }
    }

private:
    bool isWordNode;
    map<char, Trie> children;
};

// sorts words in the same order that the text twist game expectes
struct TextTwistSort
{
    inline bool operator() (const string& s1, const string& s2)
    {
        if (s1.length() != s2.length()) {
            // first sort by size
            return s1.length() < s2.length();
        } else {
            // otherwise lexical sort 
            return (s1 < s2);
        }
    }
};

void listAllWordsImpl(const Trie& dict, const string& builtUp, const string& leftChars, set<string>& results) {
    if (builtUp.size() >= 3 && dict.isWord(builtUp)) {
        results.insert(builtUp);
    }

    for (size_t i = 0; i < leftChars.length(); ++i) {
        string candidate = builtUp + leftChars[i];
        string newLeftChars = leftChars.substr(0, i) + leftChars.substr(i + 1);
        listAllWordsImpl(dict, candidate, newLeftChars, results);
    }
}

void listAllWords(const string& chars) {
    // collect the set of possible words
    set<string> results;
    listAllWordsImpl(Trie::fromDict(), "", chars, results);

    // convert into sorted vector
    auto sorted_results = vector<string>(results.size());
    copy(results.begin(), results.end(), sorted_results.begin());
    sort(sorted_results.begin(), sorted_results.end(), TextTwistSort());

    // print to user
    cout << "Valid words are:" << endl;
    for (auto& s: sorted_results) {
        cout << s << endl;
    }
}

int main() {
    string chars;
    cout << "Enter a char list from text twist: " << endl;
    cin >> chars;
    listAllWords(chars);
}
