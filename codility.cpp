#include <limits>
#include <iostream>
#include<string>
#include<stack>
#include <exception>

using namespace std;



class BadExpressionException: public exception {
public:

  BadExpressionException(string m)
      : message(m) {
  }

  virtual const char* what() const throw()
  {
    return ("Illegal expression provided for StackMachine: " + message).c_str();
  }

  string message;
};


class StackMachine {

public:

    StackMachine() {
    }

    int calculate(string& expr) {
        ops = stack<int>();
        for (auto& c: expr) {
            if (c >= '0' && c <= '9') {
                push(c - '0');
            } else if (c == '+') {
                add();
            } else if (c == '*') {
                multiply();
            } else {
                throw BadExpressionException("illegal characters in expression");
            }
        }

        return pop();
    }

private:

    // note on overflow - because StackMachine only supports adding and multiplying
    // positive integers, there is no concern about underflow, but may as well do the
    // full bounds check in case it is ever extended

    void add() {
        long sum = long(pop()) + long(pop());
        if (sum >= INT_MAX || sum <= INT_MIN) {
            throw BadExpressionException("expression results in integer overflow");
        }
        push(int(sum));
    }

    void multiply() {
        long product = long(pop()) * long(pop());
        if (product >= INT_MAX || product <= INT_MIN) {
            throw BadExpressionException("expression results in integer overflow");
        }
        push(int(product));
    }



    void push(int num) {
        ops.push(num);
    }

    int pop() {
        if (ops.size() == 0) {
            throw BadExpressionException("no arguments in stack");
        }

        int top_value = ops.top();
        ops.pop();
        return top_value;
    }

    stack<int> ops;
};

int solution(string &S) {
    try {
        return StackMachine().calculate(S);
    } catch (BadExpressionException e) {
        cout << e.what() << endl;
        return -1;
    }
}

int main() {
    string test = "13+62*7+*";
    int result = solution(test);
    cout << test << " = " << result << endl;

    test = "11++";
    result = solution(test);
    cout << test << " = " << result << endl;

    test = "43+62*7+*";
    result = solution(test);
    cout << test << " = " << result << endl;

    test = "43+62*7+*{}";
    result = solution(test);
    cout << test << " = " << result << endl;

    test = "99*9*9*9*9*9*9*99999*9*9*9*9*9*9*9**9*9*9*9*9*9*9**9*9*9*9*9*9*9*9*9*9*9*9*9*9*";
    result = solution(test);
    cout << test << " = " << result << endl;

}
