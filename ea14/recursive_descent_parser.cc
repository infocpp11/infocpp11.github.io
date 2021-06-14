#include <iostream>
#include <string>
#include <utility>

using match_range = std::pair< std::string::const_iterator, std::string::const_iterator >;

bool integer(match_range &context, int &result);
bool sum(match_range &context, int &result);
bool product(match_range &context, int &result);
bool mult(match_range &context, int &result);
bool brace(match_range &context, int &result);

bool digit(match_range &context, int &digit) {
    if (context.first != context.second && *context.first >= '0' && *context.first <= '9') {
        digit = *context.first - '0';
        ++context.first;
        return true;
    }
    return false;
}

bool nonzero_digit(match_range &context, int &digit) {
    if (context.first != context.second && *context.first >= '1' && *context.first <= '9') {
        digit = *context.first - '0';
        ++context.first;
        return true;
    }
    return false;
}

bool character(match_range &context, std::string const &values, char &result) {
    if (context.first == context.second) return false;

    for (char c : values) {
        if (c == *context.first) {
            result = c;
            ++context.first;
            return true;
        }
    }

    return false;
}

bool integer(match_range &context, int &result) {
    match_range local = context;
    int number = 0, a_digit, tmp;

    if (digit(local, a_digit) && !digit(local, tmp)) {
        number = a_digit;

        context = local;
        result = number;
        return true;
    }

    local.first = context.first;

    if (nonzero_digit(local, a_digit)) {
        number += a_digit;

        if (digit(local, a_digit)) {
            number = number * 10 + a_digit;

            while (digit(local, a_digit)) {
                number = number * 10 + a_digit;
            }

            context = local;
            result = number;
            return true;
        }

        return false;
    }

    return false;
}

bool expression(match_range &context, int &result) {
    return sum(context, result);
}

bool sum(match_range &context, int &result) {
    match_range local = context;
    int a_product;

    if (product(local, a_product)) {
        int other_product;
        char op;
        bool operator_matched = false;

        while ((operator_matched = character(local, "+-", op)) && product(local, other_product)) {
            switch (op) {
                case '+':
                    a_product += other_product;
                    break;

                case '-':
                    a_product -= other_product;
                    break;
            }

            operator_matched = false;
        }

        if (operator_matched) return false;

        result = a_product;
        context = local;
        return true;
    }

    return false;
}

bool product(match_range &context, int &result) {
    match_range local = context;
    int a_mult;

    if (mult(local, a_mult)) {
        int other_mult;
        char op;
        bool operator_matched = false;

        while ((operator_matched = character(local, "*/", op)) && mult(local, other_mult)) {
            switch (op) {
                case '*':
                    a_mult *= other_mult;
                    break;

                case '/':
                    a_mult /= other_mult;
                    break;
            }

            operator_matched = false;
        }

        if (operator_matched) return false;

        result = a_mult;
        context = local;
        return true;
    }

    return false;
}

bool mult(match_range &context, int &result) {
    match_range local = context;
    int tmp;

    if (integer(local, tmp)) {
        result = tmp;
        context = local;
        return true;
    }

    if (brace(local, tmp)) {
        result = tmp;
        context = local;
        return true;
    }

    return false;
}

bool brace(match_range &context, int &result) {
    match_range local = context;
    int tmp;
    char c;

    if (character(local, "(", c) && expression(local, tmp) && character(local, ")", c)) {
        context = local;
        result = tmp;
        return true;
    }

    return false;
}

int main() {
    std::string text = "3+4*7+2*(4-3*(2+5))";

    match_range context(text.cbegin(), text.cend());
    int result;

    if (expression(context, result)) {
        std::cout << result << std::endl;
    }
    else std::cout << "Didn't match" << std::endl;

    return 0;
}
