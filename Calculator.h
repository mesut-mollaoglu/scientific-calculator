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
		Max,
		Ln,
		Sqrt,
		Root,
		Abs,
		Sign,
		FnInt,
		Deriv,
		Expr,
		Log,
		Neg,
		Undefined
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
			else if (!GetConstant(infix, i).empty()) {
				std::string constant = GetConstant(infix, i);
				double value = GetConstantValue(constant, variable);
				if (!buffer.empty()) {
					output.push_back(Token{ Type::Number, buffer });
					buffer.clear();
					int precedence = operatorStack.empty() ? -1 : GetPrecedence(operatorStack.top());
					while (precedence >= GetPrecedence(Type::Asterisk)) {
						output.push_back(Token{ operatorStack.top() });
						operatorStack.pop();
						precedence = operatorStack.empty() ? -1 : GetPrecedence(operatorStack.top());
					}
					operatorStack.push(Type::Asterisk);
					output.push_back(Token{ Type::Number, std::to_string(value) });
				}
				else {
					output.push_back(Token{ Type::Number, std::to_string(value) });
				}
				i += constant.size() - 1;
			}
			else if (infix.at(i) == ',') {
				if (!buffer.empty()) {
					output.push_back(Token{ Type::Number, buffer });
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
				bool bExpression = (functionType == Type::FnInt || functionType == Type::Deriv);
				i += function.size() - (bExpression ? 0 : 1);
				if (bExpression) {
					std::string expr;
					if (infix.at(i) == '(') {
						operatorStack.push(Type::Open);
						i++;
					}
					while (infix.at(i) != ',') {
						expr.push_back(infix.at(i));
						i++;
					}
					output.push_back(Token{ Type::Expr, expr });
				}
			}
			else if (isOperator(infix.at(i))) {
				if (!buffer.empty()) {
					output.push_back(Token{ Type::Number, buffer });
					buffer.clear();
				}
				int precedence = operatorStack.empty() ? -1 : GetPrecedence(operatorStack.top());
				while (precedence > GetPrecedence(GetOperator(infix.at(i))) ||
					(precedence == GetPrecedence(GetOperator(infix.at(i))) && GetAssoc(GetOperator(infix.at(i))) == Associativity::Left)) {
					output.push_back(Token{ operatorStack.top() });
					operatorStack.pop();
					precedence = operatorStack.empty() ? -1 : GetPrecedence(operatorStack.top());
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
			if (!tokens.at(i).value.empty()) {
				stack.push(tokens.at(i));
			}
			else if (isOperator(tokens.at(i).type)) {
				double value1 = std::stod(GetOperand(stack).value.c_str());
				double value2 = std::stod(GetOperand(stack).value.c_str());
				std::string value;
				switch (tokens.at(i).type) {
				case Type::Asterisk: {value = std::to_string(value1 * value2); } break;
				case Type::Slash: {value = std::to_string(value2 / value1); } break;
				case Type::Minus: {value = std::to_string(value2 - value1); } break;
				case Type::Plus: {value = std::to_string(value1 + value2); } break;
				case Type::Power: {value = std::to_string(pow(value2, value1)); } break;
				}
				stack.push(Token{ Type::Number, value });
			}
			else if (isFunction(tokens.at(i).type)) {
				double value1 = std::stod(GetOperand(stack).value.c_str());
				std::string value;
				switch (tokens.at(i).type) {
				case Type::Sin: {value = std::to_string(std::sin(value1)); }; break;
				case Type::Cos: {value = std::to_string(std::cos(value1)); } break;
				case Type::Tan: {value = std::to_string(std::tan(value1)); } break;
				case Type::Max: {value = std::to_string(fmax(value1, std::stod(GetOperand(stack).value.c_str()))); } break;
				case Type::Min: {value = std::to_string(fmin(value1, std::stod(GetOperand(stack).value.c_str()))); } break;
				case Type::Cot: {value = std::to_string(1.f / std::tan(value1)); } break;
				case Type::Sec: {value = std::to_string(1.f / std::cos(value1)); } break;
				case Type::Csc: {value = std::to_string(1.f / std::sin(value1)); } break;
				case Type::Ln: {value = std::to_string(std::log(value1)); } break;
				case Type::Abs: {value = std::to_string(std::abs(value1)); } break;
				case Type::Sqrt: {value = std::to_string(std::sqrt(value1)); } break;
				case Type::Root: {value = std::to_string(std::pow(std::stod(GetOperand(stack).value.c_str()), 1.f / value1)); } break;
				case Type::Sign: {value = std::to_string((value1 >= 0) ? 1.f : -1.f); } break;
				case Type::Log: {value = std::to_string(std::log10(value1)); } break;
				case Type::Neg: {value = std::to_string(-1 * value1); } break;
				case Type::FnInt: {
					double value2 = std::stod(GetOperand(stack).value.c_str());
					std::string expr = GetOperand(stack).value;
					value = std::to_string(Integrate(expr, value2, value1));
				} break;
				case Type::Deriv: {
					std::string expr = GetOperand(stack).value;
					value = std::to_string(Derivate(expr, value1));
				} break;
				}
				stack.push(Token{ Type::Number, value });
			}
		}
		return static_cast<T>(std::stod(stack.top().value.c_str()));
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
		default: {return Type::Undefined; } break;
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
		std::vector<Type> operators = { Type::Neg, Type::Log, Type::FnInt, Type::Deriv, Type::Sin, Type::Cos, Type::Tan, Type::Cot, Type::Sec, Type::Csc, Type::Max, Type::Min, Type::Ln, Type::Sqrt, Type::Root, Type::Abs, Type::Sign };
		return std::find(operators.begin(), operators.end(), input) != operators.end();
	}
	static inline std::string GetFunction(const std::string& infix, const int& index) {
		std::vector<std::string> functions = { "neg", "log", "fnint", "deriv", "sin", "cos", "tan", "cot", "sec", "csc", "min", "max", "ln", "sqrt", "root", "abs", "sign" };
		for (int i = 0; i < functions.size(); i++)
			if (infix.substr(index, functions.at(i).size()) == functions.at(i))
				return functions.at(i);
		return "";
	}
	static inline std::string GetConstant(const std::string& infix, const int& index) {
		std::vector<std::string> functions = { "e", "pi", "g", "EPSILON", "x" };
		for (int i = 0; i < functions.size(); i++)
			if (infix.substr(index, functions.at(i).size()) == functions.at(i))
				return functions.at(i);
		return "";
	}
	static inline double GetConstantValue(std::string& constant, double variable) {
		if (constant == "pi") return 3.141592653589793;
		else if (constant == "EPSILON") return 0.000001;
		else if (constant == "e") return 2.71828;
		else if (constant == "g") return 9.81;
		else if (constant == "x") return variable;
		else return 0.0;
	}
	static inline Type GetFunctionToken(const std::string& function) {
		Type type = Type::Undefined;
		if (function == "sin") type = Type::Sin;
		else if (function == "cos") type = Type::Cos;
		else if (function == "tan") type = Type::Tan;
		else if (function == "max") type = Type::Max;
		else if (function == "min") type = Type::Min;
		else if (function == "cot") type = Type::Cot;
		else if (function == "sec") type = Type::Sec;
		else if (function == "csc") type = Type::Csc;
		else if (function == "ln") type = Type::Ln;
		else if (function == "abs") type = Type::Abs;
		else if (function == "sqrt") type = Type::Sqrt;
		else if (function == "root") type = Type::Root;
		else if (function == "sign") type = Type::Sign;
		else if (function == "fnint") type = Type::FnInt;
		else if (function == "deriv") type = Type::Deriv;
		else if (function == "log") type = Type::Log;
		else if (function == "neg") type = Type::Neg;
		return type;
	}
	static inline double Integrate(std::string input, double a, double b) {
		double h = (b - a) / 3;
		double area = 0.0;
		area = ((3 * h) / 8) * (GetValue(input, a) + 3 * GetValue(input, (2 * a + b) / 3) + 3 * GetValue(input, (a + 2 * b) / 3) + GetValue(input, b));
		return area;
	}
	static inline double Derivate(std::string input, double x, double h = 10.0) {
		return (GetValue(input, x + h) - GetValue(input, x - h)) / (2 * h);
	}
private:
	static std::stack<Type> operatorStack;
};