#include <iostream>
#include <string>
#include <cstdlib> // For std::exit and EXIT_FAILURE

bool applyRelationalOperation(double q, double p, const std::string& op) {
    if (op == "==") return q == p;
    if (op == "!=") return q != p;
    if (op == "le") return q <= p;
    if (op == "ge") return q >= p;
    if (op == "lt") return q < p;
    if (op == "gt") return q > p;
    return false; // Should never reach here if inputs are validated
}

double applyArithmeticOperation(double q, double p, const std::string& op) {
    if (op == "+") return q + p;
    if (op == "-") return q - p;
    if (op == "x") return q * p;
    if (op == "/") return q / p;
    return 0; // Should never reach here if inputs are validated
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " operator value\n";
        std::exit(EXIT_FAILURE);
    }

    std::string op(argv[1]);
    double p = std::stod(argv[2]);

    double q;
    while (std::cin >> q) {
        if (op == "+" || op == "-" || op == "x" || op == "/") {
            std::cout << applyArithmeticOperation(q, p, op) << std::endl;
        } else if (op == "==" || op == "!=" || op == "le" || op == "ge" || op == "lt" || op == "gt") {
            if (applyRelationalOperation(q, p, op)) {
                std::cout << q << std::endl;
            }
        } else {
            std::cerr << "Invalid operator: " << op << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }

    return 0;
}
