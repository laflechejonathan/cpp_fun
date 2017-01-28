#include <vector>
#include <iostream>
#include <memory>

using namespace std;

class Node {

public:
    Node() : next(NULL) {
    }

    ~Node() {
        delete next;
    }

    Node(vector<int>& numbers) {
        auto prev = this;
        data = numbers[0];
        for (size_t i = 1; i < numbers.size(); ++i) {
            prev->next = new Node();
            prev->next->data = numbers[i];
            prev = prev->next;
        }
    }

    Node* reverse() {
        if (!next) {
            return this;
        }

        auto old_next = next;
        auto new_root = old_next->reverse();
        next = NULL;
        old_next->next = this;
        return new_root;
    }

    int data;
    Node* next;


};

ostream& operator<<(ostream &strm, const Node& n) {
  if (n.next) {
    strm << n.data << "->" << *n.next;
  } else {
    strm << n.data << endl;
  }
  return strm;
}


int main() {
    vector<int> list = {1, 1, 2, 3, 5, 8, 13, 21};
    auto root = new Node(list);
    cout << *root;
    root = root->reverse();
    cout << *root;
    delete root;
}

