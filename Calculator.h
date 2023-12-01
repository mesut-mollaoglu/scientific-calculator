#include <iostream>
#include <algorithm>
#include <math.h>
#include <stack>
#include <sstream>
#include <cstring>
#include <vector>
#define PI_VALUE 3.141592653589793
#define E_VALUE 2.718281828459045
#define EPSILON 0.000001

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
	Log10,
	Neg,
	Atan,
	Asin,
	Acos,
	Sigma,
	Lim,
	Product,
	Undefined
};

enum class Associativity {
	Left,
	Right
};

struct Token {
	Type type;
	std::string value;
};

struct Constant {
	std::string name;
	double value;
};

struct Function {
	std::string name;
	Type type;
	bool bExpression = false;
};

struct Operator {
	Type type;
	int precedence;
	char oper;
	Associativity assoc;
};

const std::vector<Function> functions = {
	{"sin", Type::Sin},
	{"cos", Type::Cos},
	{"tan", Type::Tan},
	{"max", Type::Max},
	{"min", Type::Min},
	{"cot", Type::Cot},
	{"sec", Type::Sec},
	{"csc", Type::Csc},
	{"ln", Type::Ln},
	{"abs", Type::Abs},
	{"sqrt", Type::Sqrt},
	{"root", Type::Root},
	{"sign", Type::Sign},
	{"fnint", Type::FnInt, true},
	{"deriv", Type::Deriv, true},
	{"sigma", Type::Sigma, true},
	{"lim", Type::Lim, true},
	{"product", Type::Product, true},
	{"log", Type::Log},
	{"log10", Type::Log10},
	{"neg", Type::Neg},
	{"atan" ,Type::Atan},
	{"acos" ,Type::Acos},
	{"asin" ,Type::Asin},
};

const std::vector<Constant> constants = {
	{"pi", PI_VALUE},
	{"epsilon", EPSILON},
	{"g", 9.81},
	{"e", E_VALUE},
	{"inf", HUGE_VAL}
};

const std::vector<Operator> operators = {
	{Type::Plus, 1, '+', Associativity::Left},
	{Type::Minus, 1, '-', Associativity::Left},
	{Type::Asterisk, 2, '*', Associativity::Left},
	{Type::Slash, 2, '/', Associativity::Left},
	{Type::Power, 3, '^', Associativity::Right}
};

class Calculator {
public:
	static inline std::vector<Token> Convert(double variable, std::string infix) {
		std::vector<Token> output;
		std::string buffer;
		auto clear_buffer = [&]() {
			if (!buffer.empty()) {
				output.push_back(Token{ Type::Number, buffer });
				buffer.clear();
			}
		};
		auto get_precedence = [&]() {
			int precedence = -1;
			if (!operatorStack.empty()) {
				Operator oper = GetOperator(operatorStack.top());
				if (oper.oper != '\0') precedence = oper.precedence;
			}
			return precedence;
		};
		auto add_operator = [&](Operator oper) {
			int precedence = get_precedence();
			while ((precedence > oper.precedence || (precedence == oper.precedence && oper.assoc == Associativity::Left))) {
				output.push_back(Token{ operatorStack.top() });
				operatorStack.pop();
				precedence = get_precedence();
			}
			operatorStack.push(oper.type);
		};
		for (int i = 0; i < infix.size(); i++) {
			if (!GetConstant(infix, i).name.empty() || infix.at(i) == 'x') {
				Constant constant = (infix.at(i) == 'x') ? Constant{ "x", variable } : GetConstant(infix, i);
				if (!buffer.empty()) {
					output.push_back(Token{ Type::Number, buffer });
					buffer.clear();
					add_operator({ Type::Asterisk, 2, '*', Associativity::Left });
					output.push_back(Token{ Type::Number, std::to_string(constant.value) });
				}
				else {
					output.push_back(Token{ Type::Number, std::to_string(constant.value) });
				}
				i += constant.name.size() - 1;
			}
			else if (infix.at(i) == ',') {
				clear_buffer();
				while (operatorStack.top() != Type::Open) {
					output.push_back(Token{ operatorStack.top() });
					operatorStack.pop();
				}
				continue;
			}
			else if (!GetFunction(infix, i).name.empty()) {
				Function function = GetFunction(infix, i);
				clear_buffer();
				operatorStack.push(function.type);
				i += function.name.size() - (function.bExpression ? 0 : 1);
				if (function.bExpression) {
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
			else if (GetOperator(infix, i).oper != '\0') {
				clear_buffer();
				add_operator(GetOperator(infix, i));
			}
			else if (infix.at(i) == '(') {
				clear_buffer();
				operatorStack.push(Type::Open);
			}
			else if (infix.at(i) == ')') {
				clear_buffer();
				while (operatorStack.top() != Type::Open) {
					output.push_back(Token{ operatorStack.top() });
					operatorStack.pop();
				}
				operatorStack.pop();
				if (!operatorStack.empty() && isFunction(operatorStack.top())) {
					output.push_back(Token{ operatorStack.top() });
					operatorStack.pop();
				}
			}
			else if (isdigit(infix.at(i)) || infix.at(i) == '.') {
				buffer.push_back(infix.at(i));
			}
		}
		clear_buffer();
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
			else if (GetOperator(tokens.at(i).type).oper != '\0') {
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
				case Type::Neg: {value = std::to_string(-1 * value1); } break;
				case Type::Ln: {value = std::to_string(std::log(value1)); } break;
				case Type::Cos: {value = std::to_string(std::cos(value1)); } break;
				case Type::Tan: {value = std::to_string(std::tan(value1)); } break;
				case Type::Abs: {value = std::to_string(std::abs(value1)); } break;
				case Type::Sin: {value = std::to_string(std::sin(value1)); }; break;
				case Type::Acos: {value = std::to_string(std::acos(value1)); } break;
				case Type::Atan: {value = std::to_string(std::atan(value1)); } break;
				case Type::Sqrt: {value = std::to_string(std::sqrt(value1)); } break;
				case Type::Asin: {value = std::to_string(std::asin(value1)); }; break;
				case Type::Log10: {value = std::to_string(std::log10(value1)); } break;
				case Type::Cot: {value = std::to_string(1.0 / std::tan(value1)); } break;
				case Type::Sec: {value = std::to_string(1.0 / std::cos(value1)); } break;
				case Type::Csc: {value = std::to_string(1.0 / std::sin(value1)); } break;
				case Type::Sign: {value = std::to_string((value1 >= 0) ? 1.0 : -1.0); } break;
				case Type::Max: {value = std::to_string(fmax(value1, std::stod(GetOperand(stack).value.c_str()))); } break;
				case Type::Min: {value = std::to_string(fmin(value1, std::stod(GetOperand(stack).value.c_str()))); } break;
				case Type::Log: {value = std::to_string(Calculator::log(std::stod(GetOperand(stack).value.c_str()), value1)); } break;
				case Type::Root: {value = std::to_string(std::pow(std::stod(GetOperand(stack).value.c_str()), 1.0 / value1)); } break;
				case Type::FnInt: {
					double value2 = std::stod(GetOperand(stack).value.c_str());
					std::string expr = GetOperand(stack).value;
					value = std::to_string(Integral(expr, value2, value1));
				} break;
				case Type::Deriv: {
					std::string expr = GetOperand(stack).value;
					value = std::to_string(Derivative(expr, value1));
				} break;
				case Type::Sigma: {
					double end = std::stod(GetOperand(stack).value.c_str());
					double start = std::stod(GetOperand(stack).value.c_str());
					std::string expr = GetOperand(stack).value;
					value = std::to_string(Sigma(expr, start, end, value1));
				}break;
				case Type::Lim: {
					std::string expr = GetOperand(stack).value;
					value = std::to_string(Limit(expr, value1));
				} break;
				case Type::Product: {
					double end = std::stod(GetOperand(stack).value.c_str());
					double start = std::stod(GetOperand(stack).value.c_str());
					std::string expr = GetOperand(stack).value;
					value = std::to_string(Product(expr, start, end, value1));
				}break;
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
	static inline bool isFunction(const Type& input) {
		for (auto& element : functions)
			if (element.type == input)
				return true;
		return false;
	}
	static inline Operator GetOperator(const std::string& infix, const int& index) {
		for (auto& element : operators)
			if (element.oper == infix.at(index))
				return element;
		return {};
	}
	static inline Function GetFunction(const std::string& infix, const int& index) {
		for (auto& element : functions)
			if (infix.substr(index, element.name.size()) == element.name)
				return element;
		return {};
	}
	static inline Constant GetConstant(const std::string& infix, const int& index) {
		for (auto& element : constants)
			if (infix.substr(index, element.name.size()) == element.name)
				return element;
		return {};
	}
	static inline Operator GetOperator(const Type& type) {
		for (auto& element : operators)
			if (element.type == type)
				return element;
		return {};
	}
	static inline double Limit(std::string expr, double limit) {
		return (GetValue(expr, limit + EPSILON) + GetValue(expr, limit - EPSILON)) / 2;
	}
	static inline double Product(std::string input, double start, double end, double inc = 1.0) {
		double product = 1.0;
		for (double i = start; i <= end; i += inc)
			product *= GetValue(input, i);
		return product;
	}
	static inline double Sigma(std::string input, double start, double end, double inc = 1.0) {
		double sum = 0.0;
		for (double i = start; i <= end; i += inc)
			sum += GetValue(input, i);
		return sum;
	}
	static inline double Integral(std::string input, double a, double b) {
		double h = (b - a) / 3;
		double area = 0.0;
		area = ((3 * h) / 8) * (GetValue(input, a) + 3 * GetValue(input, (2 * a + b) / 3) + 3 * GetValue(input, (a + 2 * b) / 3) + GetValue(input, b));
		return area;
	}
	static inline double Derivative(std::string input, double x, double h = 10.0) {
		return (GetValue(input, x + h) - GetValue(input, x - h)) / (2 * h);
	}
	static inline double log(double a, double b)
	{
		return std::log2(b) / std::log2(a);
	}
private:
	static std::stack<Type> operatorStack;
};