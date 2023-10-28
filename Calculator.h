#include <iostream>
#include <algorithm>
#include <math.h>
#include <stack>
#include <sstream>
#include <cstring>
#include <vector>

class Calculator {
public:
	enum class Type {
		Number,
		Plus,
		Minus,
		Asterisk,
		Slash,
		Power,
		Open,
		Close,
		Sin,
		Cos,
		Tan,
		Cot,
		Sec,
		Csc,
		Min,
		Max
	};
	struct Token {
		Type type;
		std::string value;
	};
	enum class Associativity {
		Left,
		Right
	};
	static inline std::vector<Token> Convert(double variable, std::string infix) {
		std::vector<Token> output;
		std::string buffer;
		for (int i = 0; i < infix.size(); i++) {
			if (isdigit(infix.at(i)) || infix.at(i) == '.') {
				buffer.push_back(infix.at(i));
			}
			else if (infix.at(i) == 'x') {
				if (!buffer.empty()) {
					output.push_back(Token{ Type::Number, buffer });
					buffer.clear();
				}
				output.push_back(Token{Type::Number, std::to_string(variable)});
			}
			else if (infix.at(i) == ',') {
				if (!buffer.empty()) {
					output.push_back(Token{Type::Number, buffer });
					buffer.clear();
				}
				while (operatorStack.top() != Type::Open) {
					output.push_back(Token{ operatorStack.top() });
					operatorStack.pop();
				}
				continue;
			}
			else if (!GetFunction(infix, i).empty()) {
				if (!buffer.empty()) {
					output.push_back(Token{ Type::Number, buffer });
					buffer.clear();
				}
				std::string function = GetFunction(infix, i);
				Type functionType = GetFunctionToken(function);
				operatorStack.push(functionType);
				i += function.size() - 1;
			}
			else if (isOperator(infix.at(i))) {
				if (!buffer.empty()) {
					output.push_back(Token{ Type::Number, buffer });
					buffer.clear();
				}
				int precedence = operatorStack.empty() ? 0 : GetPrecedence(operatorStack.top());
				if (precedence > GetPrecedence(GetOperator(infix.at(i))) ||
					(precedence == GetPrecedence(GetOperator(infix.at(i))) && GetAssoc(GetOperator(infix.at(i))) == Associativity::Left)) {
					output.push_back(Token{ operatorStack.top() });
					operatorStack.pop();
				}
				operatorStack.push(GetOperator(infix.at(i)));
			}
			else if (infix.at(i) == '(') {
				if (!buffer.empty()) {
					output.push_back(Token{ Type::Number, buffer });
					buffer.clear();
				}
				operatorStack.push(Type::Open);
			}
			else if (infix.at(i) == ')') {
				if (!buffer.empty()) {
					output.push_back(Token{ Type::Number, buffer });
					buffer.clear();
				}
				while (operatorStack.top() != Type::Open) {
					output.push_back(Token{ operatorStack.top() });
					operatorStack.pop();
				}
				if (operatorStack.top() == Type::Open)
					operatorStack.pop();
				if (isFunction(operatorStack.top())) {
					output.push_back(Token{ operatorStack.top() });
					operatorStack.pop();
				}
			}
		}
		if (!buffer.empty()) {
			output.push_back(Token{ Type::Number, buffer });
			buffer.clear();
		}
		while (!operatorStack.empty()) {
			output.push_back(Token{ operatorStack.top() });
			operatorStack.pop();
		}
		return output;
	}
	template <class T> static inline T Calculate(std::vector<Token> tokens) {
		std::stack<Token> stack;
		for (int i = 0; i < tokens.size(); i++) {
			if (tokens.at(i).type == Type::Number) {
				stack.push(tokens.at(i));
			}
			else if (isOperator(tokens.at(i).type)) {
				T value1 = static_cast<T>(atof(GetOperand(stack).value.c_str()));
				T value2 = static_cast<T>(atof(GetOperand(stack).value.c_str()));
				std::string value;
				switch (tokens.at(i).type) {
				case Type::Asterisk: {value = std::to_string(value1 * value2); } break;
				case Type::Slash: {value = std::to_string(value2 / value1); } break;
				case Type::Minus: {value = std::to_string(value2 - value1); } break;
				case Type::Plus: {value = std::to_string(value1 + value2); } break;
				case Type::Power: {value = std::to_string(pow(value2, value1)); } break;
				}
				highestDegree = fmax(highestDegree, atof(value.c_str()));
				stack.push(Token{ .type = Type::Number, .value = value });
			}
			else if (isFunction(tokens.at(i).type)) {
				T value1 = static_cast<T>(atof(GetOperand(stack).value.c_str()));
				std::string value;
				switch (tokens.at(i).type) {
				case Type::Sin: {value = std::to_string(std::sin(value1)); }; break;
				case Type::Cos: {value = std::to_string(std::cos(value1)); } break;
				case Type::Tan: {value = std::to_string(std::tan(value1)); } break;
				case Type::Max: {value = std::to_string(fmax(value1, atof(GetOperand(stack).value.c_str()))); } break;
				case Type::Min: {value = std::to_string(fmin(value1, atof(GetOperand(stack).value.c_str()))); } break;
				case Type::Cot: {value = std::to_string(1.f / std::tan(value1)); } break;
				case Type::Sec: {value = std::to_string(1.f / std::cos(value1)); } break;
				case Type::Csc: {value = std::to_string(1.f / std::sin(value1)); } break;
				}
				highestDegree = fmax(highestDegree, atof(value.c_str()));
				stack.push(Token{ .type = Type::Number, .value = value });
			}
		}
		return static_cast<T>(atof(stack.top().value.c_str()));
	}
	static inline double GetValue(std::string infix, double variable = 0) {
		return Calculate<double>(Convert(variable, infix));
	}
	static inline Token GetOperand(std::stack<Token>& tokens) {
		Token token = tokens.top();
		tokens.pop();
		return token;
	}
	static inline int GetPrecedence(const Type& input) {
		switch (input) {
		case Type::Plus: case Type::Minus: {return 1; } break;
		case Type::Asterisk: case Type::Slash: {return 2; } break;
		case Type::Power: {return 3; } break;
		default: {return 0; } break;
		}
	}
	static inline Type GetOperator(const char& input) {
		switch (input) {
		case '+': {return Type::Plus; } break;
		case '-': {return Type::Minus; } break;
		case '*': {return Type::Asterisk; } break;
		case '/': {return Type::Slash; } break;
		case '^': {return Type::Power; } break;
		default: {return Type::Number; } break;
		}
	}
	static inline Associativity GetAssoc(const Type& input) {
		switch (input) {
		case Type::Asterisk: case Type::Minus: case Type::Plus: case Type::Slash: { return Associativity::Left; } break;
		case Type::Power: {return Associativity::Right; } break;
		default: {return Associativity::Left; } break;
		}
	}
	static inline bool isOperator(const char& input) {
		std::vector<char> operators = { '+', '^', '-', '/', '*' };
		return std::find(operators.begin(), operators.end(), input) != operators.end();
	}
	static inline bool isOperator(const Type& input) {
		std::vector<Type> operators = { Type::Plus, Type::Power, Type::Minus, Type::Slash, Type::Asterisk };
		return std::find(operators.begin(), operators.end(), input) != operators.end();
	}
	static inline bool isFunction(const Type& input) {
		std::vector<Type> operators = { Type::Sin, Type::Cos, Type::Tan, Type::Cot, Type::Sec, Type::Csc, Type::Max, Type::Min };
		return std::find(operators.begin(), operators.end(), input) != operators.end();
	}
	static inline std::string GetFunction(const std::string& infix, const int& index) {
		std::vector<std::string> functions = { "sin", "cos", "tan", "cot", "sec", "csc", "min", "max" };
		for (int i = 0; i < functions.size(); i++)
			if (infix.substr(index, functions.at(i).size()) == functions.at(i))
				return functions.at(i);
		return "";
	}
	static inline Type GetFunctionToken(const std::string& function) {
		Type type;
		if (function == "sin") type = Type::Sin;
		else if (function == "cos") type = Type::Cos;
		else if (function == "tan") type = Type::Tan;
		else if (function == "max") type = Type::Max;
		else if (function == "min") type = Type::Min;
		else if (function == "cot") type = Type::Cot;
		else if (function == "sec") type = Type::Sec;
		else if (function == "csc") type = Type::Csc;
		return type;
	}
	static inline double Integrate(std::string input, double a, double b) {
		highestDegree = 1;
		double sum = (GetValue(input, a) + GetValue(input, b));
		double h = (b - a) / highestDegree;
		for (int i = 1; i < highestDegree; i++)
		{
			double x = a + i * h;
			sum += 2 * GetValue(input, x);
		}
		return sum * (h / 2);
	}
	static inline double Derivate(std::string input, double x, double h) {
		return (GetValue(input, x+h) - GetValue(input, x-h)) / (2 * h);
	}
private:
	static std::stack<Type> operatorStack;
	static int highestDegree;
};