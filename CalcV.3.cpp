#define _USE_MATH_DEFINES
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <iomanip>
#include <limits>
#include <cctype>
#include <algorithm>
#include <stdexcept>

using namespace std;

// UI Color Terminal Codes
#define RESET "\033[0m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define CYAN "\033[36m"
#define RED "\033[31m"

// Custom Exception class for specific calculator errors
class CalcException : public runtime_error
{
public:
    CalcException(const string &message) : runtime_error(message) {}
};

enum TokenType
{
    NUMBER,
    OPERATOR,
    OPEN_PAREN,
    CLOSE_PAREN,
    FUNC,
    MEMORY,
    END
};

struct Token
{
    TokenType type;
    string value;
};

// --- [1] The Refined Robust Scanner ---
vector<Token> scanner(string input)
{
    vector<Token> tokens;
    for (int i = 0; i < input.length(); i++)
    {
        char ch = input[i];
        if (isspace(ch))
            continue;

        if (isdigit(ch) || ch == '.')
        {
            string num = "";
            while (i < input.length() && (isdigit(input[i]) || input[i] == '.'))
                num += input[i++];
            i--;
            tokens.push_back({NUMBER, num});
        }
        else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == 'r' || ch == '^')
        {
            tokens.push_back({OPERATOR, string(1, ch)});
        }
        else if (ch == '(')
            tokens.push_back({OPEN_PAREN, "("});
        else if (ch == ')')
            tokens.push_back({CLOSE_PAREN, ")"});
        else if (string("SCTPB R").find(toupper(ch)) != string::npos)
        {
            tokens.push_back({FUNC, string(1, (char)toupper(ch))});
        }
        else if (toupper(ch) == 'A' || toupper(ch) == 'M')
        {
            if (toupper(ch) == 'A' && i + 2 < input.length())
                i += 2;
            tokens.push_back({MEMORY, "M"});
        }
        else
        {
            throw CalcException("Unknown character detected: '" + string(1, ch) + "' at position " + to_string(i + 1));
        }
    }
    tokens.push_back({END, ""});
    return tokens;
}

// --- [2] The Parser (State Passed as Arguments) ---
double expression(const vector<Token> &tokens, int &pos, double lastRes, bool hasHist);

double factor(const vector<Token> &tokens, int &pos, double lastRes, bool hasHist)
{
    if (tokens[pos].type == END)
        throw CalcException("Syntax Error: Expected a number or expression but reached the end");

    if (tokens[pos].value == "-")
    {
        pos++;
        return -factor(tokens, pos, lastRes, hasHist);
    }
    if (tokens[pos].value == "+")
    {
        pos++;
        return factor(tokens, pos, lastRes, hasHist);
    }

    Token t = tokens[pos++];

    if (t.type == NUMBER)
        return stod(t.value);

    if (t.type == MEMORY)
    {
        // FIX: Check if there's actual history before using Ans/M
        if (!hasHist)
            throw CalcException("Memory Error: 'Ans' is empty. Please perform a calculation first.");
        return lastRes;
    }

    if (t.type == FUNC)
    {
        if (t.value == "P")
            return M_PI;
        double val = factor(tokens, pos, lastRes, hasHist);
        if (t.value == "R")
        {
            if (val < 0)
                throw CalcException("Math Error: Square root of negative number");
            return sqrt(val);
        }
        if (t.value == "S")
            return sin(val * M_PI / 180.0);
        if (t.value == "C")
            return cos(val * M_PI / 180.0);
        if (t.value == "T")
            return tan(val * M_PI / 180.0);
        if (t.value == "B")
            return M_PI + val;
        return val;
    }

    if (t.type == OPEN_PAREN)
    {
        double res = expression(tokens, pos, lastRes, hasHist);
        if (tokens[pos].type != CLOSE_PAREN)
            throw CalcException("Syntax Error: Missing ')'");
        pos++;
        return res;
    }
    throw CalcException("Syntax Error: Unexpected token '" + t.value + "'");
}

double power(const vector<Token> &tokens, int &pos, double lastRes, bool hasHist)
{
    double res = factor(tokens, pos, lastRes, hasHist);
    while (tokens[pos].value == "^")
    {
        pos++;
        double exp = power(tokens, pos, lastRes, hasHist);
        res = pow(res, exp);
    }
    return res;
}

double term(const vector<Token> &tokens, int &pos, double lastRes, bool hasHist)
{
    double res = power(tokens, pos, lastRes, hasHist);
    while (true)
    {
        Token nextT = tokens[pos];
        if (nextT.value == "*" || nextT.value == "/" || nextT.value == "r")
        {
            pos++;
            double nextVal = power(tokens, pos, lastRes, hasHist);
            if (nextT.value == "*")
                res *= nextVal;
            else if (nextT.value == "/")
            {
                if (nextVal == 0)
                    throw CalcException("Math Error: Division by zero");
                res /= nextVal;
            }
            else if (nextT.value == "r")
            {
                if (nextVal == 0)
                    throw CalcException("Math Error: Modulo by zero");
                res = fmod(res, nextVal);
            }
        }
        else if (nextT.type == NUMBER || nextT.type == OPEN_PAREN || nextT.type == FUNC || nextT.type == MEMORY)
        {
            res *= power(tokens, pos, lastRes, hasHist);
        }
        else
            break;
    }
    return res;
}

double expression(const vector<Token> &tokens, int &pos, double lastRes, bool hasHist)
{
    double res = term(tokens, pos, lastRes, hasHist);
    while (tokens[pos].value == "+" || tokens[pos].value == "-")
    {
        string op = tokens[pos++].value;
        double next = term(tokens, pos, lastRes, hasHist);
        if (op == "+")
            res += next;
        else if (op == "-")
            res -= next;
    }
    return res;
}

// --- [3] Main Loop ---
int main()
{
    double lastResult = 0;
    bool hasHistory = false;
    string input;

    cout << CYAN << "===========================================" << RESET << endl;
    cout << BLUE << "            CASIO SMART TERMINAL           " << RESET << endl;
    cout << YELLOW << "       (STRICT PARSING & MEMORY GUARD)     " << RESET << endl;
    cout << CYAN << "===========================================" << RESET << endl;

    while (true)
    {
        if (hasHistory)
            cout << BLUE << "[Ans = " << lastResult << "]" << RESET << " | Equation: ";
        else
            cout << YELLOW << "[Ans = EMPTY]" << RESET << " | Equation: ";

        if (!getline(cin, input))
            break;

        string lowerInput = input;
        transform(lowerInput.begin(), lowerInput.end(), lowerInput.begin(), ::tolower);

        if (lowerInput == "exit" || lowerInput == "quit" || lowerInput == "q")
        {
            cout << RED << "Exiting... Goodbye!" << RESET << endl;
            break;
        }
        if (input.empty())
            continue;

        try
        {
            vector<Token> tokens = scanner(input);
            int pos = 0;
            // Now passing hasHistory to the parser logic
            double result = expression(tokens, pos, lastResult, hasHistory);

            if (fabs(result) < 1e-9)
                result = 0.0;

            lastResult = result;
            hasHistory = true; // Memory is now active
            cout << GREEN << " Result => " << fixed << setprecision(4) << result << RESET << endl;
        }
        catch (const CalcException &e)
        {
            cout << RED << " [Calculation Error]: " << e.what() << RESET << endl;
        }
        catch (const exception &e)
        {
            cout << RED << " [System Error]: " << e.what() << RESET << endl;
        }
        catch (...)
        {
            cout << RED << " [Fatal]: An unknown error occurred." << RESET << endl;
        }
    }
    return 0;
}