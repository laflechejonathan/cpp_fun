#include <iostream>
#include <vector>

using namespace std;
/*
 * For a period of 30 days and a list of dates when we are traveling,
 * calculate the minimum amount of money to spend on tickets - no 
 * need to return the ticket distribution.
 *
 * 1-day, costs 2$
 * 7-day, costs 7$, 7 cons.
 * 30-day, costs 25$, 30 cons
 *
 */

enum Ticket {
    DAY = 1,
    WEEK = 7,
    MONTH = 30,
};

int cost(Ticket t) {
    switch (t) {
        case DAY:
            return 2;
            break;
        case WEEK:
            return 7;
            break;
        case MONTH:
            return 25;
            break;
    }
}

int week_or_singles(int n) {
    // assuming that N days within a week
    return min(cost(WEEK), n * cost(DAY));
}

int solution(vector<int> &A) {
    if (A.size() > MONTH) {
        // too many dates in schedule
        return -1;
    }

    // first, right to left

    int cost_so_far = 0;
    int last_date_to_fit = *(A.end() - 1);
    int dates_to_fit = 1;

    for (int i = A.size() - 2; i >= 0; --i) {

        if (A[i] < 0 || A[i] > MONTH) {
            // date out of range
            return -1;
        }

        if (last_date_to_fit - A[i] > WEEK) {
            cost_so_far += week_or_singles(dates_to_fit);
            last_date_to_fit = A[i];
            dates_to_fit = 0;
        }

        dates_to_fit++;
    }

    cost_so_far += week_or_singles(dates_to_fit);

    // second, left 2 right

    int cost_so_far_2 = 0;
    last_date_to_fit = A[0];
    dates_to_fit = 1;

    for (size_t i = 1; i < A.size(); ++i) {

        if (A[i] - last_date_to_fit > WEEK) {
            cost_so_far_2 += week_or_singles(dates_to_fit);
            last_date_to_fit = A[i];
            dates_to_fit = 0;
        }

        dates_to_fit++;
    }

    cost_so_far_2 += week_or_singles(dates_to_fit);
    cost_so_far = min(cost_so_far, cost_so_far_2);    

    // 25 is the upper bound, book tickets for every day.
    return min(cost_so_far, cost(MONTH));
}

void print_v(const vector<int>& v) {
    cout << "[";
    for(auto& n: v) {
        cout << n << ", ";
    }
    cout << "]";
}

int main() {
    vector<int> test = {1, 2, 4, 5, 7, 29, 30};
    cout << "best price for ";
    print_v(test);
    cout << " is " << solution(test) << " (expected 11) " << endl;

    test = {1, 2, 4, 5, 7, 9, 29, 30};
    cout << "best price for ";
    print_v(test);
    cout << " is " << solution(test) << " (expected 15) " << endl;

    test = {1, 2, 3, 11, 12, 14, 20, 21, 28, 29, 30,};
    cout << "best price for ";
    print_v(test);
    cout << " is " << solution(test) << endl;

    test = {31};
    cout << "best price for ";
    print_v(test);
    cout << " is " << solution(test) << endl;

    test = {1, 1, 2, 3, 5, 1, 1, 2, 3, 5, 2, 3, 5, 1, 1, 2, 3, 5, 2, 3, 5, 1, 1, 2, 3, 5, 2, 3, 5, 1, 1, 2, 3, 5};
    cout << "best price for ";
    print_v(test);
    cout << " is " << solution(test) << endl;
}
