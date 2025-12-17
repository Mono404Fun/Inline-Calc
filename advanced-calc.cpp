#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <deque>
#include <ios>
#include <iostream>
#include <map>
#include <stack>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

std::unordered_map<std::string, std::string> errors = {
    {"invalid_num", "Invalid number."},
    {"spaces", "No spaces allowed between digits."},
    {"op_end", "Cannot end expression with an operator."},
    {"multi_points", "Number has multiple decimal points."},
    {"op_start", "Cannot start expression with an operator."},
    {"multi_op", "Consecutive operators are not allowed."},
    {"missing_(", "Opening parenthesis '(' missing."},
    {"unknown_token", "Unknown token encountered."},
    {"mismatched_brackets", "Parentheses mismatch detected."},
    {"not_enough_opnds", "Insufficient operands for operator."},
    {"div_by_0", "Division by zero."},
    {"unknown_op", "Unknown operator in evaluation."},
    {"malformed_rpn", "Malformed RPN expression (stack != 1)."},
    {"parenthesis_required", "Parenthesis are required for operator."},
    {"num_in_op", "Invalid function name."},
    {"invalid_func", "An invalid function form."},
    {"no_func", "No function available."},
    {"sep_end", "Cannot end with separator."},
    {"sep_start", "Cannot start with separator."},
    {"multi_sep", "Consecutive separators are not allowed."},
    {"missplaced_sep", "Missplaced separator ;."},
    {"negative_num", "Negative argument to sqrt."}};

std::string alphabet = "abcdefghijklmnopqrstuvwxyz";
std::array<std::string, 5> ops = {"+", "-", "*", "/", "^"};
std::unordered_map<std::string, uint8_t> funcs = {{"sqrt", 1}, {"pow", 2}};

bool is_op(char c) {
  return std::find(ops.begin(), ops.end(), std::string(1, c)) != ops.end();
}

bool is_op(std::string c) {
  return std::find(ops.begin(), ops.end(), c) != ops.end();
}

bool is_num(std::string token) {
  if (token.empty())
    return false;

  size_t start = 0;
  if (token[0] == '+' || token[0] == '-') {
    start = 1;
    if (token.length() == 1)
      return false;
  }

  bool has_digit = false;
  bool has_dot = false;

  for (size_t i = start; i < token.length(); i++) {
    char c = token[i];

    if (std::isdigit(static_cast<unsigned char>(c))) {
      has_digit = true;
    } else if (c == '.') {
      if (has_dot)
        return false;
      has_dot = true;
    } else {
      return false;
    }
  }

  return has_digit;
}

bool is_letter(char c) {
  return alphabet.find(std::tolower(c)) != std::string::npos;
}

bool _is_func(std::string f) { return funcs.find(f) != funcs.end(); }

uint8_t return_op_precedence(std::string token) {
  if (token == "^")
    return 3;
  if (token == "*" || token == "/")
    return 2;
  if (token == "+" || token == "-")
    return 1;
  return 0;
}

template <typename T, typename... Rest> void print(T &&first, Rest &&...rest) {
  std::cout << std::forward<T>(first);
  ((std::cout << ' ' << std::forward<Rest>(rest)), ...);
}

std::deque<std::string> tokenize(std::string &input) {
  std::deque<std::string> tokens;
  std::string number = "";
  std::string func_name = "";
  std::string op_str = "";
  int parenthesis_balance = 0;

  auto push_number = [&number, &tokens]() mutable {
    if (!number.empty()) {
      if (!is_num(number)) {
        print(number);
        throw std::runtime_error(errors["invalid_num"]);
      }
      tokens.push_back(number);
      number.clear();
    }
  };

  auto push_func = [&func_name, &tokens]() mutable {
    if (!func_name.empty()) {
      if (!_is_func(func_name))
        throw std::runtime_error(errors["invalid_func"]);
      tokens.push_back(func_name);
      func_name.clear();
    }
  };

  for (size_t i = 0; i < input.size(); i++) {
    char c = input[i];

    if (std::isspace(c)) {
      continue;
    }

    if (std::isdigit(c) || c == '.') {
      if (c == '.') {
        if (number.find('.') != std::string::npos)
          throw std::runtime_error(errors["multi_points"]);
        if (number.empty())
          number = "0";
      }

      number += c;
    } else if (is_op(c)) {
      push_number();
      push_func();

      op_str = std::string(1, c);

      if ((c == '+' || c == '-') &&
          (tokens.empty() || tokens.back() == "(" || is_op(tokens.back()) ||
           tokens.back() == ";" || _is_func(tokens.back()))) {

        number += c;

        if (input[i + 1] == '(') {
          print("hehe", "\n");
          number += '1';
        }
      } else {
        if (!tokens.empty() && is_op(tokens.back()) &&
            !(tokens.back() == "+" || tokens.back() == "-")) {
          throw std::runtime_error(errors["multi_op"]);
        }

        tokens.push_back(op_str);
      }
    } else if (c == '(') {
      push_number();

      if (!tokens.empty() && (is_num(tokens.back()) || tokens.back() == ")")) {
        tokens.push_back("*");
      }

      if (!func_name.empty()) {
        push_func();
      }

      tokens.push_back("(");
      parenthesis_balance++;
    } else if (c == ')') {
      push_number();
      push_func();

      if (parenthesis_balance <= 0)
        throw std::runtime_error(errors["missing_("]);

      tokens.push_back(")");
      parenthesis_balance--;
    } else if (c == ';') {
      push_number();
      push_func();

      if (parenthesis_balance < 1)
        throw std::runtime_error(errors["missplaced_sep"]);

      tokens.push_back(";");
    } else if (is_letter(c)) {
      if (!number.empty()) {
        throw std::runtime_error(errors["num_in_op"]);
      }

      func_name += c;

      if (i + 1 < input.size() && is_letter(input[i + 1])) {
        continue;
      } else if (i + 1 < input.size() && input[i + 1] == '(') {
        continue;
      } else {
        throw std::runtime_error(errors["parenthesis_required"]);
      }
    } else {
      throw std::runtime_error(errors["unknown_token"]);
    }
  }

  push_number();
  push_func();

  if (parenthesis_balance != 0)
    throw std::runtime_error(errors["mismatched_brackets"]);

  if (!tokens.empty() && is_op(tokens.back()))
    throw std::runtime_error(errors["op_end"]);

  return tokens;
}

std::deque<std::string> to_rpn(std::deque<std::string> tokens) {
  std::stack<std::string> hs;
  std::deque<std::string> rpn;
  std::stack<std::string> func_stack;
  std::stack<std::deque<std::string>> func_args_stack;
  std::deque<std::string> current_args;
  std::string current_arg = "";

  for (auto &token : tokens) {
    if (is_num(token)) {
      if (func_stack.empty()) {
        rpn.push_back(token);
      } else {
        current_arg += token;
      }
    } else if (_is_func(token)) {
      func_stack.push(token);
      func_args_stack.push(current_args);
      current_args.clear();
      current_arg = "";
    } else if (token == "(") {
      if (func_stack.empty()) {
        hs.push(token);
      }
    } else if (token == ")") {
      if (func_stack.empty()) {
        while (!hs.empty() && hs.top() != "(") {
          rpn.push_back(hs.top());
          hs.pop();
        }

        if (hs.empty())
          throw std::runtime_error(errors["missing_("]);

        hs.pop();
      } else {
        if (!current_arg.empty()) {
          current_args.push_back(current_arg);
          current_arg = "";
        }

        std::string func_name = func_stack.top();
        func_stack.pop();

        std::deque<std::string> args = current_args;
        if (!func_args_stack.empty()) {
          current_args = func_args_stack.top();
          func_args_stack.pop();
        } else {
          current_args.clear();
        }

        for (auto &arg : args) {
          rpn.push_back(arg);
        }

        rpn.push_back("@" + func_name);
      }
    } else if (token == ";") {
      if (func_stack.empty()) {
        throw std::runtime_error(errors["missplaced_sep"]);
      }
      if (!current_arg.empty()) {
        current_args.push_back(current_arg);
        current_arg = "";
      }
    } else if (is_op(token)) {
      if (func_stack.empty()) {
        while (!hs.empty() && hs.top() != "(" && is_op(hs.top()) &&
               return_op_precedence(hs.top()) >= return_op_precedence(token)) {
          rpn.push_back(hs.top());
          hs.pop();
        }
        hs.push(token);
      } else {
        current_arg += token;
      }
    } else {
      throw std::runtime_error(errors["unknown_token"]);
    }
  }

  if (!current_arg.empty()) {
    current_args.push_back(current_arg);
  }

  while (!hs.empty()) {
    if (hs.top() == "(" || hs.top() == ")")
      throw std::runtime_error(errors["mismatched_brackets"]);
    rpn.push_back(hs.top());
    hs.pop();
  }

  return rpn;
}

double evaluate_rpn(std::deque<std::string> rpn) {
  std::stack<double> hs;

  for (auto &token : rpn) {
    if (is_num(token)) {
      hs.push(std::stod(token));
    } else if (is_op(token)) {
      if (hs.size() < 2)
        throw std::runtime_error(errors["not_enough_opnds"]);

      double b = hs.top();
      hs.pop();
      double a = hs.top();
      hs.pop();
      double result = 0.0;

      if (token == "+")
        result = a + b;
      else if (token == "-")
        result = a - b;
      else if (token == "*")
        result = a * b;
      else if (token == "/") {
        if (b == 0.0)
          throw std::runtime_error(errors["div_by_0"]);
        result = a / b;
      } else if (token == "^")
        result = std::pow(a, b);

      hs.push(result);
    } else if (token.size() > 1 && token[0] == '@') {
      std::string func_name = token.substr(1);

      if (func_name == "sqrt") {
        if (hs.empty())
          throw std::runtime_error(errors["not_enough_opnds"]);
        double a = hs.top();
        hs.pop();
        if (a < 0)
          throw std::runtime_error(errors["negative_num"]);
        hs.push(std::sqrt(a));
      } else if (func_name == "pow") {
        if (hs.size() < 2)
          throw std::runtime_error(errors["not_enough_opnds"]);
        double b = hs.top();
        hs.pop();
        double a = hs.top();
        hs.pop();
        hs.push(std::pow(a, b));
      }
    }
  }

  if (hs.size() != 1)
    throw std::runtime_error(errors["malformed_rpn"]);

  return hs.top();
}

int main() {
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

    if (input[0] == ':') {
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
                  << ":history  Show previous calculations\n"
                  << ":theme    Toggle theme (dark/neon)\n"
                  << ":clear    Clear screen\n"
                  << ":clearh   Clear history\n"
                  << ":exit     Exit program\n\n"
                  << C["primary"] << C["bold"] << "\nAvailable functions:\n"
                  << C["reset"] << C["bold"]
                  << "GENERAL FORM: func(a;b...;c)\n\n"
                  << C["reset"]
                  << "pow       Power a number 'a' with number 'b'\n"
                  << "          Example usage: pow(2;3) -> 8\n"
                  << "sqrt      Square a number 'a'\n"
                  << "          Example usage: sqrt(5) -> 25\n";
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

      if (cmd == "clearh") {
        if (history.size() == 0)
          std::cout << "No calculations available.\n";

        std::cout << C["success"] << history.size()
                  << " calculations cleared!\n"
                  << C["reset"];
        history.clear();
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
      auto t = tokenize(input);
      auto r = to_rpn(t);
      auto result = evaluate_rpn(r);

      if (result == std::floor(result))
        last_result = std::to_string(static_cast<int>(result));
      else
        last_result = std::to_string(result);

      history.push_back(input + " = " + last_result);

      std::cout << C["primary"] << C["bold"]
                << "\n────────── RESULT ──────────\n"
                << C["reset"];
      std::cout << C["success"] << colorize(last_result) << "\n";
      std::cout << C["primary"] << "────────────────────────────\n\n"
                << C["reset"];
    } catch (std::exception &e) {
      std::cout << C["error"] << "⚠ " << e.what() << "\n\n";
    }
  }

  std::cout << C["gray"] << "Goodbye!\n" << C["reset"];
  return 0;
}
