#include <algorithm> // for std::ranges::transform and misc algorithms
#include <array>     // for std::array used to store operator
#include <cctype>    // for std::tolower and character checks
#include <cmath>
#include <cstddef>
#include <cstdint>   // for fixed-width integer types
#include <deque>     // for std::deque
#include <exception> // for std::exception base class
#include <iostream>  // for std::cin / std::cout input-output
#include <map>       // for std::map used in theme/color maps
#include <stack>     // for std::stack
#include <stdexcept> // for std::runtime_error and thrown errors
#include <string>    // for std::string and string operations
#include <sys/types.h>
#include <unordered_map> // for fast lookup tables
#include <vector>        // for std::vector used in tokens, containers

std::array<std::string, 5> ops = {"+", "-", "*", "/",
                                  "^"}; // store all available operators

struct Func {
  std::string name;
  uint8_t args;
};

std::unordered_map<std::string, uint8_t> funcs = {{"sqrt", 1}, {"pow", 2}};

std::unordered_map<std::string, std::string> errors =
    { // store various exceptions, paired with their name
        {"invalid_num", "Invalid number."},
        {"spaces", "No spaces allowed between digits."},
        {"op_end", "Cannot end expression with an operator."},
        {"multi_points", "Number has multiple decimal points."},
        {"op_start", "Cannot start expression with an operator."},
        {"multi_op", "Consecutive operators are not allowed."},
        {"missing_(", "Opening parenthesis '(' missing."},
        {"unknown_token", "Unknown token encountered."},
        {"missmatched_brackets", "Parentheses mismatch detected."},
        {"not_enough_opnds", "Insufficient operands for operator."},
        {"div_by_0", "Division by zero."},
        {"unknown_op", "Unknown operator in evaluation."},
        {"malformed_rpn", "Malformed RPN expression (stack != 1)."},
        {"parenthesis_required", "Parenthesis are required for operator."},
        {"num_in_op", "Invalid function name."},
        {"invalid_char", "Invalid character."},
        {"invalid_func", "An invalid function form."}};

std::string alphabet = "abcdefghijklmnopqrstuvwxyz";

template <typename T>
void print(T input = "",
           std::string end = "\n") { // a python like function to print texts
                                     // easly i don't want to use std::print
                                     // since it will blow up executable size
  std::cout << input << end;
}

namespace utils { // a namespace that contains some utility function, to
                  // simplify life (:
// this checks if a given "char" is an operator
bool is_op(char c) {
  if (std::find(ops.begin(), ops.end(), std::string(1, c)) !=
      ops.end()) // if given char is available
                 // inside operators array
    return true; // it's an operator
  return false;  // otherwise it's not an operator
}

// this one used to check for a "string"
bool is_op(std::string c) {
  // same magic happens here
  if (std::find(ops.begin(), ops.end(), c) != ops.end())
    return true;
  return false;
}

// check if a given string is a valid number
bool is_num(std::string token) {
  bool has_digit = false; // check if variable has a digit
  bool has_dot = false;   // if it has a dot (float number)
  bool is_unary = false;  // check if it's unary number or not

  if (!token.empty()) {
    if (token.front() == '+' || token.front() == '-') {
      is_unary = true;
      token.erase(token.begin());
    }

    for (char c : token) { // iterate over our token char by char
      if (is_unary && (token.front() == '+' || token.front() == '-')) {
        return false;
      }

      if (std::isdigit(
              static_cast<unsigned char>(c))) // if current char is a digit
        has_digit = true;                     // set has_digit to true
      else if (c == '.') {                    // if it is a float number point
        if (has_dot) {                        // and it had another one before
          return false;                       // it's not a valid number
        }

        has_dot = true;
      }
    }

    return has_digit;
  } else {
    return true;
  };
}

bool is_func(std::string f) {
  if (funcs.find(f) != funcs.end())
    return true;

  return false;
}

bool validate_func(std::string f, std::vector<std::string> args) {
  return true;
}

bool is_letter(char c) {
  if (alphabet.find(c) != std::string::npos)
    return true;

  return false;
}

// used to return precedence/priority of a given operator
uint8_t return_op_precedence(std::string token) {
  if (token == "^")
    return 3;
  if (token == "*" || token == "/")      // if it's multiplication or division
    return 2;                            // return 2 (according to PEDMAS)
  else if (token == "+" || token == "-") // if it's addition or substraction
    return 1;                            // return 1
  else
    return 0; // if it's another operator, return 0 (not valid)
}
} // namespace utils

enum class TokenType : uint8_t {
  BNUM = 1,    // a binary number (3, 5, 9..etc)
  UNUM = 2,    // a unary number (-3, +999..etc)
  OP = 3,      // an operator (-, +, *, /)
  UNKNOWN = 4, // unkown token type
};

enum class OperatorType { ADD, SUB, MUL, DIV, POW, FUNC, PAREN };

struct Token {
  TokenType type;
  std::string value;

  Token(TokenType type, std::string value) : type(type), value(value) {}

  Token operator+(const Token &other) {
    return Token(type, value + other.value);
  }

  friend std::ostream &operator<<(std::ostream &out, const Token &t) {
    out << t.value;
    return out;
  }
};

struct Operator {
  std::string value;
  uint8_t precedence;
};

template <template <typename, typename...> class C>
  requires std::ranges::range<C<Token>>
class Expression {
private:
  using type = C<Token>;
  using it = type::iterator;
  using cit = type::const_iterator;

  C<Token> tokens;

  it begin() { return tokens.begin(); }
  it end() { return tokens.end(); }

  cit begin() const { return tokens.begin(); }
  cit end() const { return tokens.end(); }

  cit cbegin() const { return tokens.cbegin(); }
  cit cend() const { return tokens.cend(); }

  void push(Token &token) { tokens.push_back(token); }
  void pop() { tokens.pop(); }

  /* bool empty() {}
  uint8_t size() {}
  void clear() {} */

  Token first() {
    if (tokens.empty())
      throw std::out_of_range("Expression is empty.");

    return *tokens.begin();
  }

  const Token last() {
    if (tokens.empty())
      throw std::out_of_range("Expression is empty.");

    return *tokens.end();
  }
};

/*
 * This function is used to convert an input expression to
 * a deque of tokens: 1 + 9-9/(-9-6) -> [1,+,9,-,9,/,(,-9,-,6,)]
 * */
std::deque<std::string> tokenize_str(std::string input) {
  std::deque<std::string> tokens; // a deque to store our tokens one by one
                                  // std::deque is similar to std::stack,
                                  // but i prefer using std::deque here since we
                                  // can iterate over it's elements ^-^

  char c;                  // current char while iterating over expression later
  std::string number = ""; // current working number, used to push it as a token
                           // once it's completed
  TokenType last_token_type; // keep track of last token type, we will use that
                             // later for unary/binary numbers
  uint8_t parenthesis_balance = 0;
  bool expect_unary = true;
  bool is_func = false;
  std::string temp = "";

  /*
   * this function is used to push our current
   * working number to tokens deque once it's completed
   */
  auto push_number = [&number, &expect_unary,
                      &tokens]() mutable { // lambda function
    if (!number.empty()) {                 // if number isn't empty
      if (!utils::is_num(number))          // if it's invalid
        throw std::runtime_error(errors["invalid_num"]); // throw exception
      tokens.push_back(
          number);    // otherwise if it's valid, just push it to tokens deque
      number.clear(); // then clean it
      expect_unary = false; // a number was just pushed, next token *must* be an
                            // op or '('. bro it's logic (:
    }
  };

  // if no expression is given, just ignore
  if (input.empty())
    return {};
  else if (input.size() == 1 && utils::is_op(input))
    throw std::runtime_error(errors["not_enough_opnds"]);

  if (utils::is_op(input.front()) && input.front() != '+' &&
      input.front() != '-')
    throw std::runtime_error(errors["op_start"]);

  for (int i = 0; i < input.size();
       i++) {     // iterate over expression char by char
    c = input[i]; // get current char by index

    if (std::isspace(c)) { // if current char is space
      // check for spaces between digits (5 99+9)
      //                                   | this one for example
      if ((std::isdigit((input[i + 1])) || input[i + 1] == '.') &&
          !number.empty()) // so if next char is digit or it's a floating number
                           // point throw exception
        throw std::runtime_error(errors["spaces"]);

      continue; // allow spaces between numbers/operators and go to next char
    }

    else if (c == ';') {
      if (parenthesis_balance > 0 && is_func) {
        if (!number.empty())
          push_number();
      } else

        throw std::runtime_error(errors["invalid_func"]);
    }

    else if (std::isdigit(c)) // if current char is a digit
      number += c;            // just append it to current working number

    else if (c == '.') { // if it's a float number point
      if (number.find('.') !=
          std::string::npos) // check if there are previous
                             // points in current number, so
                             // we won't allow multipe floating points
        throw std::runtime_error(
            errors["multi_points"]); // throw an exception if true
      if (number.empty())            // if current number is empty
        number = "0.";               // just add a zero at the start
                                     // so it will support numbers such
                                     // as (.5, .559, .63...etc)

      else             // if number is not empty (only natural part available)
        number += '.'; // append the floating point
    }

    else if (utils::is_op(c)) { // if current char is an operator
      if ((c == '-' || c == '+') && expect_unary && number.empty()) {
        number = c;

        if (input[i + 1] == '(') {
          number += '1';
        }

        expect_unary = false;
        continue;
      } else {
        push_number(); // so push current number if it's not empty (completed)

        if (!tokens.empty() && utils::is_op(tokens.back()) &&
            tokens.back() != "+" && tokens.back() != "-" && c != '+' &&
            c != '-')
          throw std::runtime_error(errors["multi_op"]);

        tokens.push_back(std::string(1, c)); // push that opeartor as a token
        expect_unary = true;
        continue;
      }
    } else if (c == '(') { // if current char is an opening bracket
      if (utils::is_num(number))
        push_number(); // push number if it's not empty

      // if the previous char is a number, and there aren't any operator
      // between that number and the opening bracket, just add a '*' for
      // example: 5(4) -> 5*(4)

      if (!tokens.empty()) {
        if (utils::is_num(tokens.back()))
          tokens.push_back("*");
      }

      tokens.push_back("("); // push that bracket to tokens deque
      parenthesis_balance++;
      expect_unary = true;
      continue;            // go to next char directly
    } else if (c == ')') { // if it's a closing bracket
      if (utils::is_num(number)) {
        if (is_func)
          is_func = false;

        push_number(); // push number as well here
      }

      if (utils::is_op(
              tokens.back())) // if last token (the one before closing
                              // bracket) was an operator, throw an exception
        throw std::runtime_error(errors["op_end"]);

      if (parenthesis_balance <= 0)
        throw std::runtime_error(errors["missing_("]);

      tokens.push_back(")"); // push that closing bracket
      parenthesis_balance--; // since one bracket is closed, so...actually,
                             // i'm too lazy to explain, since it's a simple
                             // thing
      expect_unary = false;  // after ')', we expect a binary operator
      continue;
    } else if (utils::is_letter(c)) {
      if (utils::is_letter(input[i + 1])) {
        temp += c;
        continue;
      } else if (input[i + 1] == '(') {
        temp += c;
        tokens.push_back(temp);
        is_func = true;
      } else if (utils::is_num(std::string(1, input[i + 1])))
        throw std::runtime_error(errors["num_in_op"]);
      else {
        print(c);
        print(errors["parenthesis_required"]);
      }

      continue;
    } else {
      throw std::runtime_error(errors["invalid_char"]);
    }
    // throw std::runtime_error(errors["unknown_token"]);
  }

  if (utils::is_num(number))
    push_number(); // push last number to tokens

  // if input expression ends with an operator, throw exception
  if (utils::is_op(input.back()))
    throw std::runtime_error(errors["op_end"]);

  if (parenthesis_balance != 0)
    throw std::runtime_error(errors["missmatched_brackets"]);

  return tokens; // return tokens deque
}

std::deque<std::string> to_rpn(std::deque<std::string> tokens) {
  std::stack<std::string> hs;  // a temp holding stack
  std::deque<std::string> rpn; // our output stack (actually a deque)

  for (auto &token : tokens) {  // iterate over tokens one by one
    if (utils::is_num(token)) { // if token is a valid number
      rpn.push_back(token);     // push it to output stack (rpn)
      continue;
    } else if (token == "(") { // and if it's an opening bracket
      hs.push(token);          // push it to holding stack
      continue;
    } else if (token == ")") {                 // if it's a closing bracket now
      while (!hs.empty() && hs.top() != "(") { // pop numbers until we find
                                               // the opening bracket
        rpn.push_back(hs.top());
        hs.pop();
      }

      if (hs.empty()) // if holding stack is empty and we didn't find opening
                      // bracket
        throw std::runtime_error(
            errors["missing_("]); // throw an exception (mising '(')
      hs.pop();

      continue;
    } else if (utils::is_op(token)) { // if token is an operator
      while (!hs.empty() && utils::is_op(hs.top()) &&
             utils::return_op_precedence(hs.top()) >=
                 utils::return_op_precedence(
                     token)) { // check if current token/operator's precedence
                               // is less than previous one
        // if true, pot it from holding into output stack
        rpn.push_back(hs.top());
        hs.pop();
      }

      hs.push(token); // otherwise, just push it to holding stack
      continue;
    } else // if token isn't one of these, it's unknow token - throw an
           // exception
      throw std::runtime_error(errors["unknown_token"]);
  }

  // pop remaining tokens in holding stack to output stack (rpn)
  while (!hs.empty()) {
    if (hs.top() == "(" ||
        hs.top() == ")") // any bracket must not remain in holding stack
      throw std::runtime_error(
          errors["missmatched_brackets"]); // if any is available, throw
                                           // exception
    rpn.push_back(hs.top());
    hs.pop();
  }

  return rpn; // finally, return output rpn expression (tokens) as a deque
}

/* this function evaluates an rpn expression and returns
 * a double result value
 * */
double evaluate_rpn(std::deque<std::string> rpn) {
  std::stack<double> hs; // a temp holding stack

  if (rpn.empty()) // if input rpn expression is empty, just ignore
    return {};

  for (auto &token : rpn) { // iterate over our rpn expression token by token
    if (utils::is_num(token)) {  // if oken is a number
      hs.push(std::stod(token)); // conver to double and push to holding stack
      continue;                  // go to next token directly
    } else if (utils::is_op(token)) { // if it's an operator
      if (hs.size() < 2) // check if there are enough operands for operator
                         // if there aren't enough, throw an exception
        throw std::runtime_error(errors["not_enough_opnds"]);

      // other wise get those operands
      double b = hs.top(); // second operand
      hs.pop();            // pop it from holding stack, so we can get first one
      double a = hs.top(); // first operand
      hs.pop();            // idk even why i did this here
      double res = 0.0;    // double variable to store result

      if (token == "+") { // additon
        res = a + b;
      } else if (token == "-") { // substraction
        res = a - b;
      } else if (token == "*") { // multiplication
        res = a * b;
      } else if (token == "/") { // division
        if (b == 0.0)
          throw std::runtime_error(errors["div_by_0"]); // if second operand
                                                        // is zero through an
                                                        // exception
        res = a / b; // otherwise, divide safely and calculate result
      } else if (token == "^") {
        res = std::pow(a, b);
      } else // if operator is something else
             // through an exception
        throw std::runtime_error(errors["unknown_op"]);

      hs.push(res); // push result to holding stack
      continue;     // go to next token
    }
  }

  if (hs.size() !=
      1) // if holding stack doesn't have only one item (final result)
    throw std::runtime_error(errors["malformed_rpn"]); // throw an exception

  double result = hs.top();

  if (result == std::floor(result)) { // if it's a whole number
    if (result == -0)                 // if result is -0, turn it to 0 (:
      return 0;

    result = std::floor(result); // return it as int
  }

  return result; // return the final result
}

int main() {
  // i'm too lazy to explain the rest of these, it's easy to understand BTW

  // COLOR SCHEMES
  auto dark_th = std::map<std::string, std::string>{
      {"reset", "\033[0m"},    {"bold", "\033[1m"},    {"dim", "\033[2m"},
      {"num", "\033[32m"},   // green
      {"op", "\033[36m"},    // cyan
      {"paren", "\033[35m"}, // magenta
      {"primary", "\033[36m"}, {"accent", "\033[35m"}, {"warn", "\033[33m"},
      {"error", "\033[31m"},   {"gray", "\033[90m"},   {"success", "\033[32m"}};

  auto neon_th = std::map<std::string, std::string>{
      {"reset", "\033[0m"},         {"bold", "\033[1m"},
      {"dim", "\033[2m"},           {"num", "\033[38;5;46m"},
      {"op", "\033[38;5;51m"},      {"paren", "\033[38;5;207m"},
      {"primary", "\033[38;5;51m"}, {"accent", "\033[38;5;207m"},
      {"warn", "\033[38;5;226m"},   {"error", "\033[38;5;196m"},
      {"gray", "\033[38;5;240m"},   {"success", "\033[38;5;82m"}};

  std::string theme = "dark";
  std::map<std::string, std::string> C = dark_th;

  auto refresh_theme = [&]() { C = (theme == "dark" ? dark_th : neon_th); };

  refresh_theme();

  auto colorize = [&](const std::string &expr) {
    std::string out;
    for (char c : expr) {
      if (std::isdigit(c)) {
        out += C["num"];
        out += c;
        out += C["reset"];
      } else if (std::string("+-*/%^").find(c) != std::string::npos) {
        out += C["op"];
        out += c;
        out += C["reset"];
      } else if (c == '(' || c == ')') {
        out += C["paren"];
        out += c;
        out += C["reset"];
      } else
        out += c;
    }
    return out;
  };

  auto clear_screen = []() { std::cout << "\033[2J\033[H"; };

  clear_screen();

  std::cout << C["primary"] << C["bold"];
  std::cout << "═══════════════════════════════════════════════════════════\n";
  std::cout << "                     MONOAL’S CALCULATOR\n";
  std::cout << "═══════════════════════════════════════════════════════════\n";
  std::cout << C["reset"] << "\n";

  std::cout << C["gray"] << "Type an expression. Commands start with "
            << C["accent"] << "'.'" << C["reset"] << "\n\n";

  std::string input;
  std::vector<std::string> history;
  std::string last_result = "";

  while (true) {
    std::cout << C["accent"] << C["bold"] << "> " << C["reset"];
    std::getline(std::cin, input);

    if (input.empty())
      continue;

    if (input[0] == '.') {
      std::string cmd = input.substr(1);

      if (cmd == "exit")
        break;

      if (cmd == "clear") {
        clear_screen();
        continue;
      }

      if (cmd == "help") {
        std::cout << C["primary"] << C["bold"] << "\nCommands:\n" << C["reset"];
        std::cout << C["gray"] << ".help     Show commands\n"
                  << ".history  Show previous calculations\n"
                  << ".theme    Toggle theme (dark/neon)\n"
                  << ".clear    Clear screen\n"
                  << ".exit     Exit program\n\n";
        continue;
      }

      if (cmd == "history") {
        std::cout << C["primary"] << C["bold"]
                  << "\n────────── HISTORY ──────────\n"
                  << C["reset"];
        if (history.empty()) {
          std::cout << C["gray"] << "(empty)\n\n";
        } else {
          for (int i = 0; i < history.size(); i++)
            std::cout << C["gray"] << i + 1 << ". " << C["reset"] << history[i]
                      << "\n";
          std::cout << "\n";
        }
        continue;
      }

      if (cmd == "theme") {
        theme = (theme == "dark" ? "neon" : "dark");
        refresh_theme();
        std::cout << C["success"] << "Theme switched to " << theme << "\n\n";
        continue;
      }

      std::cout << C["error"] << "Unknown command: " << cmd << "\n\n";
      continue;
    }

    try {
      auto t = tokenize_str(input);
      auto r = to_rpn(t);
      auto result = evaluate_rpn(r);

      if (result == std::floor(result))
        last_result = std::to_string(static_cast<int>(result));
      else
        last_result = std::to_string(result);

      history.push_back(input + " = " +
                        last_result); // push last result to history

      std::cout << C["primary"] << C["bold"]
                << "\n────────── RESULT ──────────\n"
                << C["reset"];
      std::cout << C["success"] << colorize(last_result) << "\n";
      std::cout << C["primary"] << "────────────────────────────\n\n"
                << C["reset"]; */
    } catch (std::exception &e) {
      std::cout << C["error"] << "⚠ " << e.what() << "\n\n";
    }
  }

  std::cout << C["gray"] << "Goodbye!\n" << C["reset"];
  return 0;
}
