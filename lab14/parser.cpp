#include <string>
#include <utility>
#include <memory>
#include <iostream>
#include <functional>

using match_range = std::pair< std::string::iterator, std::string::iterator >;
using semantic_action = std::function< void(std::string const &) >;

class base_rule {
    private:
        virtual bool test(match_range &context) = 0;
        semantic_action the_semantic_action;

    public:
        virtual ~base_rule() = default;


        bool match(match_range &context, match_range &matching_range) {
            match_range local = context;

            if (test(local)) {
                matching_range = { context.first, local.first };
                if (the_semantic_action) {
                    the_semantic_action(std::string(matching_range.first, matching_range.second));
                }

                context = local;
                return true;
            }

            return false;
        }

        virtual base_rule &operator() (semantic_action const &an_action) {
            the_semantic_action = an_action;
            return *this;
        }

        virtual std::shared_ptr<base_rule> clone() const = 0;
};

class rule : public base_rule {
    private:
        std::shared_ptr< std::shared_ptr<base_rule> > the_rule;

        virtual bool test(match_range &context) override {
            if (!(*the_rule)) throw "Undefined rule";
            match_range matching_range;
            return (*the_rule)->match(context, matching_range);
        }

    public:
        rule(std::shared_ptr<base_rule> a_rule = nullptr) :
            the_rule(std::make_shared<std::shared_ptr<base_rule>>(a_rule)) {}

        void set_rule(std::shared_ptr<base_rule> a_rule) {
            *the_rule = a_rule;
        }

        rule &operator<<=(base_rule const &a_rule) {
            set_rule(a_rule.clone());
            return *this;
        }

        virtual base_rule &operator() (semantic_action const &an_action) {
            if (!(*the_rule)) throw "Undefined rule";
            (**the_rule) (an_action);
            return *this;
        }

        virtual std::shared_ptr<base_rule> clone() const override {
            return std::make_shared<rule>(*this);
        }
};

class succession : public base_rule {
    private:
        std::shared_ptr<base_rule> first_rule;
        std::shared_ptr<base_rule> second_rule;

        virtual bool test(match_range &context) override {
            match_range matching_range;
            return first_rule->match(context, matching_range) && second_rule->match(context, matching_range);
        }

    public:
        succession(std::shared_ptr<base_rule> first_rule, std::shared_ptr<base_rule> second_rule) :
            first_rule(first_rule),
            second_rule(second_rule) {}

        virtual std::shared_ptr<base_rule> clone() const override {
            return std::make_shared<succession>(*this);
        }
};

class alternation : public base_rule {
    private:
        std::shared_ptr<base_rule> first_rule;
        std::shared_ptr<base_rule> second_rule;

        virtual bool test(match_range &context) override {
            match_range matching_range;
            return first_rule->match(context, matching_range) || second_rule->match(context, matching_range);
        }

    public:
        alternation(std::shared_ptr<base_rule> first_rule, std::shared_ptr<base_rule> second_rule) :
            first_rule(first_rule),
            second_rule(second_rule) {}

        virtual std::shared_ptr<base_rule> clone() const override {
            return std::make_shared<alternation>(*this);
        }
};

class repetition : public base_rule {
    private:
        std::shared_ptr<base_rule> the_rule;

        virtual bool test(match_range &context) override {
            match_range matching_range;
            if (the_rule->match(context, matching_range)) {
                while (the_rule->match(context, matching_range))
                    ;
                return true;
            }
            return false;
        }

    public:
        repetition(std::shared_ptr<base_rule> a_rule) : the_rule(a_rule) {}

        virtual std::shared_ptr<base_rule> clone() const override {
            return std::make_shared<repetition>(*this);
        }

};

succession operator <<(base_rule const &first_rule, base_rule const &second_rule) {
    return succession(first_rule.clone(), second_rule.clone());
}


alternation operator |(base_rule const &first_rule, base_rule const &second_rule) {
    return alternation(first_rule.clone(), second_rule.clone());
}

repetition operator +(base_rule const &a_rule) {
    return repetition(a_rule.clone());
}

class character : public base_rule {
    private:
        std::string values;

        virtual bool test(match_range &context) override {
            if (context.first == context.second) return false;

            for (auto c : values) {
                if (*context.first == c) {
                    ++context.first;
                    return true;
                }
            }

            return false;
        }

    public:
        character(std::string const &values) : values(values) {}

        virtual std::shared_ptr<base_rule> clone() const override {
            return std::make_shared<character>(*this);
        }
};

int main() {
    std::string text = "0xfce2";

    /* HEXA SZÁM */
    rule hex_num, hex_prefix, hex_digit;

    auto print_hex_prefix = [] (std::string const & s) {
        std::cout << "Found hex prefix: " << s << std::endl;
    };

    hex_num <<= hex_prefix << +hex_digit;
    hex_prefix <<= character("0") << character("x");
    hex_digit <<= character("0123456789abcdef");
    hex_prefix(print_hex_prefix);

    match_range context(text.begin(), text.end()), result;
    if (hex_num.match(context, result)) {
        std::cout << "Matched: " << std::string(result.first, result.second) << std::endl;
    }
    else std::cout << "Didn't match" << std::endl;

    return 0;
}
