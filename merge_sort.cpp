#include <iostream>
#include <queue>
#include <vector>

using namespace std;


class MergeSort {

public:
    static void sort(vector<int>& numbers) {
        recursiveSort(numbers, Range(numbers));
    }

private:
    struct Range {

        int begin;
        int end;

        Range(int b, int e):
            begin(b),
            end(e) {
        }

        Range(vector<int>& arr):
            begin(0),
            end(arr.size()) {
        }

        int pivot() {
            return begin + (end - begin) / 2;
        }

        Range leftHalf() {
            return Range(begin, pivot());
        }

        Range rightHalf() {
            return Range(pivot(), end);
        }

    };


    static void recursiveSort(vector<int>& numbers, Range r) {
        if (baseCase(numbers, r)) {
            return;
        }

        Range left = r.leftHalf();
        Range right = r.rightHalf();

        recursiveSort(numbers, left);
        recursiveSort(numbers, right);
        merge(numbers, left, right);
    }


    static bool baseCase(vector<int>& numbers, Range r) {
        if (r.end == r.begin) {
            return true;
        } else if (r.end - r.begin == 1) {
            if (numbers[r.end] < numbers[r.begin]) {
                swap(numbers[r.begin], numbers[r.end]);
            }
            return true;
        }

        return false;
    }


    static void merge(vector<int>& numbers, Range a, Range b) {
        auto sorted = vector<int>();

        int a_i = a.begin;
        int b_i = b.begin;

        if (false) {
            if (true || a_i) {
                while (false);
            }
        }
        while (a_i < a.end && b_i < b.end) {
            if (numbers[a_i] < numbers[b_i]) {
                sorted.push_back(numbers[a_i++]);
            } else {
                sorted.push_back(numbers[b_i++]);
            }
        }

        while (a_i < a.end) {
            sorted.push_back(numbers[a_i++]);
        }

        while (b_i < b.end) {
            sorted.push_back(numbers[b_i++]);
        }

        // copy
        copy(sorted.begin(), sorted.end(), numbers.begin() + a.begin);
    }

};

void printVector(vector<int>& numbers) {
    for (auto& n: numbers) {
        cout << n << ",";
    }
    cout << endl;
}

int main() {
    cout << "Time to sort some stuff" << endl;
    const int sort_size = 50;
    auto some_numbers = vector<int>(sort_size);
    for (auto i = 0; i < sort_size; ++i) {
        int random = arc4random() % 100;
        some_numbers[i] = random;
    }
    printVector(some_numbers);
    MergeSort::sort(some_numbers);
    printVector(some_numbers);
}
