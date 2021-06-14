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

    protected:
        semantic_action the_semantic_action;

    public:
        base_rule() = default;
        base_rule(base_rule const &) = default;
        virtual ~base_rule() {}


        bool match(match_range &context, match_range &the_match_range) {
            match_range local = context;

            if (test(local)) {
                the_match_range = { context.first, local.first };
                if (the_semantic_action) {
                    the_semantic_action(std::string(the_match_range.first, the_match_range.second));
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
            match_range the_match_range;
            return (*the_rule)->match(context, the_match_range);
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

        virtual std::shared_ptr<base_rule> clone() const override {
            return std::make_shared<rule>(*this);
        }

        base_rule &operator() (semantic_action const &an_action) {
            if (!(*the_rule)) throw "Undefined rule";
            (**the_rule) (an_action);
            return *this;
        }
};

class succession : public base_rule {
    private:
        std::shared_ptr<base_rule> first_rule;
        std::shared_ptr<base_rule> second_rule;

        virtual bool test(match_range &context) override {
            match_range the_match_range;
            return first_rule->match(context, the_match_range) && second_rule->match(context, the_match_range);
        }

    public:
        succession(std::shared_ptr<base_rule> first_rule, std::shared_ptr<base_rule> second_rule) :
            first_rule(first_rule),
            second_rule(second_rule) {}

        virtual std::shared_ptr<base_rule> clone() const override {
            return std::make_shared<succession>(*this);
        }
};

class repetition : public base_rule {
    private:
        std::shared_ptr<base_rule> the_rule;

        virtual bool test(match_range &context) override {
            match_range the_match_range;
            if (the_rule->match(context, the_match_range)) {
                while (the_rule->match(context, the_match_range))
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
    std::string text = "0x2beaf5";
    match_range context(text.begin(), text.end()), result;
    rule hex_num, hex_prefix, hex_digit;
    
    hex_num <<= hex_prefix << +hex_digit;
    hex_prefix <<= character("0") << character("x");
    hex_digit <<= character("0123456789abcdef");

    if (hex_num.match(context, result)) {
        std::cout << "Matched: " << std::string(result.first, result.second) << std::endl;
    }
    else std::cout << "Didn't match" << std::endl;

    return 0;
}


