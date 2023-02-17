#pragma once
#include <string>
#include <cctype>
#include <algorithm>
#include <math.h>
#include <stack>
#include <sstream>
#include <string>
#include <vector>
#include <map>

using namespace std;

class Calculator {
public:
    int preced(char ch) {
        if (ch == '+' || ch == '-') {
            return 1;           
        }
        else if (ch == '*' || ch == '/') {
            return 2;          
        }
        else if (ch == '^') {
            return 3;          
        }
        else {
            return 0;
        }
    }
    string convert(string infix) {
        stack<char> stk;
        stk.push('#');             
        stringstream postfix;
        string::iterator it;
        string buffer;
        for (it = infix.begin(); it != infix.end(); it++) {
            if (isdigit(char(*it)))
                buffer += (char)(*it);
            else if (*it == 'x')
                postfix << *it << ' ';
            else if (*it == '(') {
                stk.push('(');
                if (!buffer.empty()) {
                    postfix << buffer << ' ';
                    buffer.clear();
                }
            }
            else if (*it == '^') {
                stk.push('^');
                if (!buffer.empty()) {
                    postfix << buffer << ' ';
                    buffer.clear();
                }
            }
            else if (*it == ')') {
                if (!buffer.empty()) {
                    postfix << buffer << ' ';
                    buffer.clear();
                }
                while (stk.top() != '#' && stk.top() != '(') {
                    postfix << stk.top() << ' ';
                    stk.pop();
                }
                stk.pop();         
            }
            else {
                if (!buffer.empty()) {
                    postfix << buffer << ' ';
                    buffer.clear();
                }
                if (preced(*it) > preced(stk.top()))
                    stk.push(*it);
                else {
                    while (stk.top() != '#' && preced(*it) <= preced(stk.top())) {
                        postfix << stk.top() << ' ';
                        stk.pop();
                    }
                    stk.push(*it);
                }
            }
        }
        if (!buffer.empty()) {
            postfix << buffer << ' ';
            buffer.clear();
        }
        while (stk.top() != '#') {
            postfix << stk.top() << ' ';
            stk.pop();
        }
        return postfix.str();
    }
    double getDegree(const std::string& postfix) {
        std::stack<double> operands;
        double maxDegree = 0;
        for (int i = 0; i < postfix.length(); i++) {
            char c = postfix[i];
            if (isdigit(c)) {
                string operand = "";
                while (i < postfix.length() && isdigit(postfix[i])) {
                    operand += postfix[i];
                    i++;
                }
                i--;
                operands.push(stoi(operand));
            }
            else if (c == 'x') { operands.push(1); }
            else if (isspace(c)) { continue; }
            else {
                int operand2 = operands.top();
                operands.pop();
                int operand1 = operands.top();
                operands.pop();
                int result;
                switch (c) {
                case '+':
                case '-': {result = operand1; break; }
                case '*': {result = operand1 + operand2; break; }
                case '/': {result = operand1 - operand2; break; }
                case '^': {result = operand1 * operand2; break; }
                }
                maxDegree = fmax(maxDegree, result);
                operands.push(result);
            }
        }
        return maxDegree;
    }
    double calculate(double value, const std::string& postfix) {
        std::stack<double> operands;
        for (int i = 0; i < postfix.length(); i++) {
            char c = postfix[i];
            if (isdigit(c)) {
                string operand = "";
                while (i < postfix.length() && isdigit(postfix[i])) {
                    operand += postfix[i];
                    i++;
                }
                i--;
                operands.push(stod(operand));
            }
            else if (c == 'x') {
                operands.push(value);
            }
            else if (isspace(c)) {
                continue;
            }
            else {
                double operand2 = operands.top();
                operands.pop();
                double operand1 = operands.top();
                operands.pop();
                double result;
                switch (c) {
                case '+': {result = operand1 + operand2; break; }
                case '-': {result = operand1 - operand2; break; }
                case '*': {result = operand1 * operand2; break; }
                case '/': {result = operand1 / operand2; break; }
                case '^': {result = pow(operand1, operand2); break; }
                }
                operands.push(result);
            }
        }
        return operands.top();
    }

    double integrate(string input, double a, double b, double n) {
        double sum = (this->calculate(a, this->convert(input)) + this->calculate(b, this->convert(input)));
        double h = (b - a) / n;
        for (int i = 1; i < n; i++)
        {
            double x = a + i * h;
            sum += 2 * this->calculate(x, this->convert(input));
        }
        return sum * (h / 2);
    }
    double derivate(string input, double x, double h) {
        return (this->calculate(x + h, this->convert(input)) - this->calculate(x - h, this->convert(input))) / (2 * h);
    }
};