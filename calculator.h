#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <iostream>
#include <math.h>
#include <string.h>
#include <unordered_map>
#include <vector>
#include <stdint.h>
#include <stack>
#include <optional>
#include <variant>
#include <functional>

#if defined LOWP
using fp_t = float;
inline fp_t stofp(const std::string& s){return std::stof(s);}
inline constexpr fp_t g_inf = HUGE_VALF;
#define DEFINE_FUNCTION(FUNCTION_NAME) FUNCTION_NAME##f
#elif defined MEDIUMP
using fp_t = double;
inline fp_t stofp(const std::string& s){return std::stod(s);}
inline constexpr fp_t g_inf = HUGE_VAL;
#define DEFINE_FUNCTION(FUNCTION_NAME) [](const fp_t& lhs){return std::FUNCTION_NAME(lhs);}
#else
using fp_t = long double;
inline fp_t stofp(const std::string& s){return std::stold(s);}
inline constexpr fp_t g_inf = HUGE_VALL;
#define DEFINE_FUNCTION(FUNCTION_NAME) FUNCTION_NAME##l
#endif

#define DEFINE_TOKEN_TYPE_ENUM              \
BEGIN_ENUM                                  \
{                                           \
    DECLARE_ENUM_ELEMENT(Number),           \
    DECLARE_ENUM_ELEMENT(Plus),             \
    DECLARE_ENUM_ELEMENT(Minus),            \
    DECLARE_ENUM_ELEMENT(Asterisk),         \
    DECLARE_ENUM_ELEMENT(Slash),            \
    DECLARE_ENUM_ELEMENT(Power),            \
    DECLARE_ENUM_ELEMENT(OpenParentheses),  \
    DECLARE_ENUM_ELEMENT(CloseParentheses), \
    DECLARE_ENUM_ELEMENT(Sin),              \
    DECLARE_ENUM_ELEMENT(Cos),              \
    DECLARE_ENUM_ELEMENT(Tan),              \
    DECLARE_ENUM_ELEMENT(Cot),              \
    DECLARE_ENUM_ELEMENT(Sec),              \
    DECLARE_ENUM_ELEMENT(Csc),              \
    DECLARE_ENUM_ELEMENT(Min),              \
    DECLARE_ENUM_ELEMENT(Max),              \
    DECLARE_ENUM_ELEMENT(Ln),               \
    DECLARE_ENUM_ELEMENT(Sqrt),             \
    DECLARE_ENUM_ELEMENT(Root),             \
    DECLARE_ENUM_ELEMENT(Abs),              \
    DECLARE_ENUM_ELEMENT(Sign),             \
    DECLARE_ENUM_ELEMENT(FnInt),            \
    DECLARE_ENUM_ELEMENT(Deriv),            \
    DECLARE_ENUM_ELEMENT(Expression),       \
    DECLARE_ENUM_ELEMENT(Log),              \
    DECLARE_ENUM_ELEMENT(Log10),            \
    DECLARE_ENUM_ELEMENT(Neg),              \
    DECLARE_ENUM_ELEMENT(Atan),             \
    DECLARE_ENUM_ELEMENT(Asin),             \
    DECLARE_ENUM_ELEMENT(Acos),             \
    DECLARE_ENUM_ELEMENT(Sigma),            \
    DECLARE_ENUM_ELEMENT(Lim),              \
    DECLARE_ENUM_ELEMENT(Product),          \
    DECLARE_ENUM_ELEMENT(Log2),             \
    DECLARE_ENUM_ELEMENT(Constant),         \
    DECLARE_ENUM_ELEMENT(Modulo),           \
    DECLARE_ENUM_ELEMENT(Comma),            \
    DECLARE_ENUM_ELEMENT(Undefined)         \
};                                          \

#define BEGIN_ENUM enum class Type
#define DECLARE_ENUM_ELEMENT(ELEMENT_NAME) ELEMENT_NAME

using opt_fp_t = std::optional<fp_t>;
inline bool g_debugModeOn = false;

struct Token
{
    DEFINE_TOKEN_TYPE_ENUM
    Token::Type type = Token::Type::Undefined;
    std::variant<opt_fp_t, std::string> value;
    inline Token(const Token::Type& type, const opt_fp_t& value = std::nullopt) : type(type), value(value) {}
    inline Token(const Token::Type& type, const std::string& value) : type(type), value(value) {}
    inline bool HoldsString() const
    {
        return std::holds_alternative<std::string>(value) && !std::get<std::string>(value).empty();
    }
    inline bool HoldsReal() const
    {
        return std::holds_alternative<opt_fp_t>(value) && std::get<opt_fp_t>(value).has_value();
    }
    inline bool HasValue() const
    {
        return HoldsReal() || HoldsString();
    }
    inline fp_t GetReal() const
    {
        if(HoldsReal())
            return std::get<opt_fp_t>(value).value();
        else if(HoldsString())
            return stofp(std::get<std::string>(value));
        else
            throw std::runtime_error("Real value doesn't exist!");
    }
    inline std::string GetString() const
    {
        if(HoldsReal())
            return std::to_string(std::get<opt_fp_t>(value).value());
        else if(HoldsString())
            return std::get<std::string>(value);
        else
            throw std::runtime_error("String is empty!");
    }
};

#undef BEGIN_ENUM
#undef DECLARE_ENUM_ELEMENT

#define BEGIN_ENUM std::unordered_map<Token::Type, const char*> g_mapTokenTypeNames = 
#define DECLARE_ENUM_ELEMENT(ELEMENT_NAME) {Token::Type::ELEMENT_NAME, #ELEMENT_NAME}
DEFINE_TOKEN_TYPE_ENUM

inline std::ostream& operator<<(std::ostream& os, const Token::Type& t)
{
    os << g_mapTokenTypeNames.at(t);
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const Token& token)
{
    os << "Type:" << token.type << ",Value:" << (token.HasValue() ? token.GetString() : "Null") << '\n';
    return os;
}

const std::unordered_map<std::string_view, Token::Type> g_mapFunctionNames = 
{
	{"ln", Token::Type::Ln},
    {"sin", Token::Type::Sin},
	{"cos", Token::Type::Cos},
	{"tan", Token::Type::Tan},
	{"max", Token::Type::Max},
	{"min", Token::Type::Min},
	{"cot", Token::Type::Cot},
	{"sec", Token::Type::Sec},
	{"csc", Token::Type::Csc},
	{"abs", Token::Type::Abs},
	{"lim", Token::Type::Lim},
	{"log", Token::Type::Log},
	{"neg", Token::Type::Neg},
	{"sqrt", Token::Type::Sqrt},
	{"root", Token::Type::Root},
	{"sign", Token::Type::Sign},
	{"asin", Token::Type::Asin},
	{"atan", Token::Type::Atan},
	{"acos", Token::Type::Acos},
	{"log10", Token::Type::Log10},
    {"log2", Token::Type::Log2},
	{"sigma", Token::Type::Sigma},
	{"deriv", Token::Type::Deriv},
	{"fnint", Token::Type::FnInt},
    {"prod", Token::Type::Product}
};

const std::unordered_map<Token::Type, std::function<fp_t(const fp_t&)>> g_mapUnaryFunctions = 
{
    {Token::Type::Ln, DEFINE_FUNCTION(log)},
    {Token::Type::Sin, DEFINE_FUNCTION(sin)},
    {Token::Type::Cos, DEFINE_FUNCTION(cos)},
    {Token::Type::Tan, DEFINE_FUNCTION(tan)},
    {Token::Type::Abs, DEFINE_FUNCTION(fabs)},
    {Token::Type::Acos, DEFINE_FUNCTION(acos)},
    {Token::Type::Atan, DEFINE_FUNCTION(atan)},
    {Token::Type::Sqrt, DEFINE_FUNCTION(sqrt)},
    {Token::Type::Asin, DEFINE_FUNCTION(asin)},
    {Token::Type::Log2, DEFINE_FUNCTION(log2)},
    {Token::Type::Log10, DEFINE_FUNCTION(log10)},
    {Token::Type::Neg, [](const fp_t& lhs){return -1 * lhs;}},
    {Token::Type::Cot, [](const fp_t& lhs){return 1 / std::tan(lhs);}},
    {Token::Type::Sec, [](const fp_t& lhs){return 1 / std::cos(lhs);}},
    {Token::Type::Csc, [](const fp_t& lhs){return 1 / std::sin(lhs);}},
    {Token::Type::Sign, [](const fp_t& lhs){return lhs >= 0 ? 1 : -1;}}
};

const std::unordered_map<Token::Type, std::function<fp_t(const fp_t&, const fp_t&)>> g_mapBinaryFunctions =
{
    {Token::Type::Max, [](const fp_t& lhs, const fp_t& rhs){return lhs < rhs ? rhs : lhs;}},
    {Token::Type::Min, [](const fp_t& lhs, const fp_t& rhs){return lhs > rhs ? rhs : lhs;}},
    {Token::Type::Log, [](const fp_t& lhs, const fp_t& rhs){return std::log2(rhs) / std::log2(lhs);}},
    {Token::Type::Root, [](const fp_t& lhs, const fp_t& rhs){return std::pow(rhs, static_cast<fp_t>(1) / lhs);}}
};

const std::unordered_map<std::string_view, fp_t> g_mapConstantNames =
{
    {"golden_ratio", static_cast<fp_t>(1.6180339887498948482)},
    {"tau", static_cast<fp_t>(6.28318530717958647692)},
    {"phi", static_cast<fp_t>(1.6180339887498948482)},
    {"pi", static_cast<fp_t>(3.141592653589793)},
	{"e", static_cast<fp_t>(2.718281828459045)},
	{"epsilon", static_cast<fp_t>(0.00001)},
    {"G", static_cast<fp_t>(6.6743E-11)},
	{"g", static_cast<fp_t>(9.807)},
    {"infinity", g_inf},
    {"inf", g_inf}
};

std::unordered_map<std::string_view, fp_t> g_mapUserDefinedConstants;

struct Operator
{
    uint8_t precedence;
    char op;
    bool leftAssoc;
};

const std::unordered_map<Token::Type, Operator> g_mapOperators = 
{
    {Token::Type::Plus, {1, '+', true}},
	{Token::Type::Minus, {1, '-', true}},
	{Token::Type::Asterisk, {2, '*', true}},
	{Token::Type::Slash, {2, '/', true}},
    {Token::Type::Modulo, {2, '%', false}},
	{Token::Type::Power, {3, '^', false}}
};

const std::unordered_map<char, Token::Type> g_mapCharType = 
{
    {'+', Token::Type::Plus},
    {'-', Token::Type::Minus},
    {'*', Token::Type::Asterisk},
    {'/', Token::Type::Slash},
    {'^', Token::Type::Power},
    {'%', Token::Type::Modulo}
};

inline bool DoesOperatorExist(char c)
{
    return g_mapCharType.count(c);
}

inline bool DoesFunctionExist(const std::string& s)
{
    return g_mapFunctionNames.count(s);
}

inline bool DoesConstantExist(const std::string& s)
{
    return g_mapConstantNames.count(s) || g_mapUserDefinedConstants.count(s);
}

inline bool DoesFunctionExist(const Token::Type& token)
{
    if(g_mapOperators.count(token))
        return false;
    switch(token)
    {
        case Token::Type::Number:
        case Token::Type::OpenParentheses:
        case Token::Type::CloseParentheses:
        case Token::Type::Comma:
        case Token::Type::Undefined:
            return false;
        default:
            return true;
    }
}

inline std::optional<Operator> GetOperator(char c)
{
    if(DoesOperatorExist(c))
        return g_mapOperators.at(g_mapCharType.at(c));
    else
        return std::nullopt;
}

inline std::optional<Token::Type> GetFunction(const std::string& s)
{
    if(DoesFunctionExist(s))
        return g_mapFunctionNames.at(s);
    else
        return std::nullopt;
}

inline opt_fp_t GetConstant(const std::string& s)
{
    if(g_mapUserDefinedConstants.count(s))
        return g_mapUserDefinedConstants.at(s);
    else if(g_mapConstantNames.count(s))
        return g_mapConstantNames.at(s);
    else
        return std::nullopt;
}

fp_t Evaluate(const std::string& infix)
{
    std::stack<Token::Type> operators;
    std::vector<Token> tokens;
    std::string buffer;
    Token::Type prevToken = Token::Type::Undefined;
    auto getPrecedence = [&operators]() -> int
    {
        int res = -1;
        if(!operators.empty())
        {
            std::optional<Operator> op = g_mapOperators.count(operators.top()) ? std::make_optional(g_mapOperators.at(operators.top())) : std::nullopt;
            if(op.has_value())
                res = op.value().precedence;
        }
        return res;
    };
    auto addOperator = [getPrecedence, &prevToken, &tokens, &operators](const Operator& op)
    {
        int precedence = getPrecedence();
        while(precedence > op.precedence || (precedence == op.precedence && op.leftAssoc))
        {
            tokens.emplace_back(prevToken = operators.top());
            operators.pop();
            precedence = getPrecedence();
        }
        operators.push(g_mapCharType.at(op.op));
    };
    for(size_t i = 0; i < infix.size(); i++)
    {
        const char& c = infix.at(i);
        if(std::isdigit(c) || c == '.')
        {
            while(i < infix.size() && (std::isdigit(infix.at(i)) || infix.at(i) == '.'))
            {
                buffer.push_back(infix.at(i));
                if(i + 1 < infix.size() && (std::isdigit(infix.at(i + 1)) || infix.at(i + 1) == '.'))
                    i++;
                else
                    break;
            }
            tokens.emplace_back(prevToken = Token::Type::Number, stofp(buffer));
            buffer.clear();
        }
        else if(std::isalpha(c))
        {
            while(i < infix.size() && (std::isalpha(infix.at(i)) || std::isdigit(infix.at(i))))
            {
                buffer.push_back(infix.at(i));
                if(i + 1 < infix.size() && (std::isalpha(infix.at(i + 1)) || std::isdigit(infix.at(i + 1))))
                    i++;
                else
                    break;
            }
            if(prevToken == Token::Type::Number)
                addOperator(g_mapOperators.at(Token::Type::Asterisk));
            if(DoesConstantExist(buffer))
                tokens.emplace_back(prevToken = Token::Type::Number, GetConstant(buffer).value());
            else if(DoesFunctionExist(buffer))
                operators.push(prevToken = g_mapFunctionNames.at(buffer));
            buffer.clear();
        }
        else if(c == '\'')
        {
            ++i;
            while(i < infix.size() && infix.at(i) != '\'')
            {
                buffer.push_back(infix.at(i));
                if(i + 1 < infix.size() && infix.at(i+1) != '\'')
                    i++;
                else
                    break;
            }
            tokens.emplace_back(prevToken = Token::Type::Expression, buffer);
            buffer.clear();
            ++i;
        }
        else if(c == ',')
        {
            prevToken = Token::Type::Comma;
            while(operators.top() != Token::Type::OpenParentheses)
            {
                tokens.emplace_back(operators.top());
                operators.pop();
            }
            continue;
        }
        else if(c == '(')
        {
            if(prevToken == Token::Type::Number)
                addOperator(g_mapOperators.at(Token::Type::Asterisk));
            operators.push(prevToken = Token::Type::OpenParentheses);
        }
        else if(c == ')')
        {
            prevToken = Token::Type::CloseParentheses;
            while(operators.top() != Token::Type::OpenParentheses)
            {
                tokens.emplace_back(operators.top());
                operators.pop();
            }
            operators.pop();
            if(!operators.empty() && DoesFunctionExist(operators.top()))
            {
                tokens.emplace_back(operators.top());
                operators.pop();
            }
        }
        else if(DoesOperatorExist(c))
        {
            prevToken = g_mapCharType.at(c);
            addOperator(GetOperator(c).value());
        }
    }
    while(!operators.empty())
    {
        tokens.emplace_back(operators.top());
        operators.pop();
    }
    if(g_debugModeOn)
        for(auto& t : tokens)
            std::cout << t;
    std::stack<Token> s;
    auto getOperand = [&s]() -> Token
    {
        Token res = s.top();
        s.pop();
        return res;
    };
    std::string x, exp;
    auto eval = [&x, &exp](const fp_t& value) -> fp_t
    {
        g_mapUserDefinedConstants[x] = value;
        return Evaluate(exp);
    };
    fp_t old, hi, lo, inc; bool def;
    for(size_t i = 0; i < tokens.size(); i++)
    {
        if(tokens.at(i).HasValue())
            s.push(tokens.at(i));
        else if(DoesFunctionExist(tokens.at(i).type))
        {
            if(g_mapUnaryFunctions.count(tokens.at(i).type))
                s.emplace(Token::Type::Number, g_mapUnaryFunctions.at(tokens.at(i).type)(getOperand().GetReal()));
            else if(g_mapBinaryFunctions.count(tokens.at(i).type))
                s.emplace(Token::Type::Number, g_mapBinaryFunctions.at(tokens.at(i).type)(getOperand().GetReal(), getOperand().GetReal()));
            else
            {
                x = getOperand().GetString();
                if(def = g_mapUserDefinedConstants.count(x))
                    old = g_mapUserDefinedConstants.at(x);
                switch(tokens.at(i).type)
                {
                    case Token::Type::FnInt:
                    {
                        hi = getOperand().GetReal();
                        lo = getOperand().GetReal();
                        exp = getOperand().GetString();
                        const fp_t h = (hi - lo) / 3;
                        s.emplace(Token::Type::Number, ((3 * h) / 8) * (eval(lo) + 3 * eval((2 * lo + hi) / 3) + 3 * eval((lo + 2 * hi) / 3) + eval(hi)));
                    }
                    break;
                    case Token::Type::Product:
                    {
                        inc = getOperand().GetReal();
                        hi = getOperand().GetReal();
                        lo = getOperand().GetReal();
                        exp = getOperand().GetString();
                        fp_t res = 1;
                        for(fp_t i = lo; i <= hi; i += inc) res *= eval(i);
                        s.emplace(Token::Type::Number, res);
                    }
                    break;
                    case Token::Type::Sigma:
                    {
                        inc = getOperand().GetReal();
                        hi = getOperand().GetReal();
                        lo = getOperand().GetReal();
                        exp = getOperand().GetString();
                        fp_t res = 0;
                        for(fp_t i = lo; i <= hi; i += inc) res += eval(i);
                        s.emplace(Token::Type::Number, res);
                    }
                    break;
                    case Token::Type::Lim:
                    {
                        lo = getOperand().GetReal();
                        exp = getOperand().GetString();
                        s.emplace(Token::Type::Number, (eval(lo + g_mapConstantNames.at("epsilon")) + eval(lo - g_mapConstantNames.at("epsilon"))) * static_cast<fp_t>(0.5));
                    }
                    break;
                    case Token::Type::Deriv:
                    {
                        static constexpr fp_t h = 10;
                        lo = getOperand().GetReal();
                        exp = getOperand().GetString();
                        s.emplace(Token::Type::Number, (eval(lo + h) - eval(lo - h)) / (2 * h));
                    }
                    break;
                }
                if(def)
                    g_mapUserDefinedConstants[x] = old;
                else
                    g_mapUserDefinedConstants.erase(x);
            }
        }
        else
            switch (tokens.at(i).type)
            {
                case Token::Type::Asterisk:
                    s.emplace(Token::Type::Number, getOperand().GetReal() * getOperand().GetReal());
                break;
                case Token::Type::Slash:
                    s.emplace(Token::Type::Number, 1 / (getOperand().GetReal() / getOperand().GetReal()));
                break;
                case Token::Type::Minus:
                    s.emplace(Token::Type::Number, (getOperand().GetReal() - getOperand().GetReal()) * -1);
                break;
                case Token::Type::Plus:
                    s.emplace(Token::Type::Number, getOperand().GetReal() + getOperand().GetReal());
                break;
                case Token::Type::Power:
                {
                    const fp_t rhs = getOperand().GetReal();
                    const fp_t lhs = getOperand().GetReal();
                    s.emplace(Token::Type::Number, std::pow(lhs, rhs));
                }
                break;
                case Token::Type::Modulo:
                {
                    const fp_t rhs = getOperand().GetReal();
                    const fp_t lhs = getOperand().GetReal();
                    s.emplace(Token::Type::Number, std::fmod(lhs, rhs));
                }
            }
    }
    return s.top().GetReal();
}

void PrintInstructions()
{
choose:
    std::cin.clear();
    std::string choice;
    std::cout << "Enter command:\n";
    std::getline(std::cin, choice);
    std::transform(choice.begin(), choice.end(), choice.begin(), [](char c){return std::tolower(c);});
    if(choice == "quit")
    {
        std::cout << "Terminating program...\n";
        std::exit(0);
    }
    else if(choice == "enter equation")
    {
        std::cout << "Enter the equation:\n";
        choice.clear();
        std::getline(std::cin, choice);
        try {std::cout << (g_mapUserDefinedConstants["Ans"] = Evaluate(choice)) << '\n';}
        catch(const std::runtime_error& e) {std::cout << e.what() << '\n';}
        catch(const std::bad_variant_access& e) {std::cout << e.what() << '\n';}
        catch(const std::bad_optional_access& e) {std::cout << e.what() << '\n';}
        catch(const std::invalid_argument& e) {std::cout << e.what() << '\n';}
    }
    else if(choice.find("precision") != std::string::npos)
    {
        choice.erase(std::remove_if(choice.begin(), choice.end(), ::isspace), choice.end());
        if(choice.find('=') != std::string::npos)
            try
            {
                std::cout.precision(std::stoi(choice.substr(choice.find('=') + 1, choice.size() - choice.find('=') - 1)));
            }
            catch(const std::invalid_argument& e) {std::cout << e.what() << '\n';}
    }
    else if(choice == "clear variable list")
    {
        fp_t ans = 0;
        bool def = false;
        if(def = g_mapUserDefinedConstants.count("Ans"))
            ans = g_mapUserDefinedConstants.at("Ans");
        g_mapUserDefinedConstants.clear();
        if(def)
            g_mapUserDefinedConstants["Ans"] = ans;
    }
    else if(choice == "define variable")
    {
        std::cin.clear();
        choice.clear();
        std::getline(std::cin, choice);
        choice.erase(std::remove_if(choice.begin(), choice.end(), ::isspace), choice.end());
        if(choice.find('=') != std::string::npos)
        {
            const std::string var = choice.substr(0, choice.find_first_of('='));
            try
            {
                if(!g_mapConstantNames.count(var) && var != "Ans")
                    g_mapUserDefinedConstants[var] = stofp(choice.substr(choice.find_first_of('=') + 1, choice.size() - choice.find_first_of('=') - 1));
                else
                    std::cout << "Constant already exists\n";
            }
            catch(const std::invalid_argument& e) {std::cout << e.what() << '\n';}
        }
        if(g_debugModeOn)
            for(auto& [name, value] : g_mapUserDefinedConstants)
                std::cout << "Name:" << name << ",Value:" << value << '\n';
    }
    else if(choice.find("debug") != std::string::npos)
    {
        choice.erase(std::remove_if(choice.begin(), choice.end(), ::isspace), choice.end());
        const char* s = choice.substr(choice.find_first_of('=') + 1, choice.size() - choice.find_first_of('=') - 1).c_str();
        if(!strcmp(s, "on") || !strcmp(s, "true"))
            g_debugModeOn = true;
        else if(!strcmp(s, "off") || !strcmp(s, "false"))
            g_debugModeOn = false;
        else
            std::cout << "Invalid Argument\n";
    }
    else if(choice == "clear")
        system("cls");
    else if(choice == "help")
    {
        std::cout << "To define a variable: define variable\n";
        std::cout << "To clear the console: clear\n";
        std::cout << "To enter an equation: enter equation\n";
        std::cout << "To quit the application: quit\n";
        std::cout << "To set how many digits will be displayed after the decimal point:precision=(number of digits)\n";
        std::cout << "To clear the list of variables: clear variable list\n";
        std::cout << "To turn the debug mode on/off: debug=(on/off)\n";
    }
    else
        std::cout << "Command is not recognized\n";
    goto choose;
}

#endif