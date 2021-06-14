#include <iostream>


class Expression {
  public:
    /* returns value of expression at x */
    virtual double evaluate(double x) const = 0;

    Expression() = default;
    Expression(Expression const &) = default;

    virtual ~Expression() = default;

  private:
    /* outputs expression as string to stream os */
    virtual void print(std::ostream &os) const = 0;
    friend std::ostream & operator<<(std::ostream & os, Expression const &e);
};


/* pass print request to virtual print() method */
std::ostream & operator<<(std::ostream & os, Expression const &e) {
    e.print(os);
    return os;
}


class Constant final : public Expression {
  public:
    Constant(double c): c_{c} {}

    virtual double evaluate(double) const override {
        return c_;
    }

  private:
    double c_;

    virtual void print(std::ostream &os) const override {
        os << c_;
    }
};


class Variable final : public Expression {
  public:
    virtual double evaluate(double x) const override {
        return x;
    }

  private:
    virtual void print(std::ostream &os) const override {
        os << 'x';
    }
};


class TwoOperand : public Expression {
  public:
    /* create object, adopt dynamically allocated expressions */
    TwoOperand(Expression *lhs, Expression *rhs) : lhs_{lhs}, rhs_{rhs} {}

    ~TwoOperand() {
        delete lhs_;
        delete rhs_;
    }

    /* no copy construction */
    TwoOperand(TwoOperand const &) = delete;

    /* no copy assignment */
    TwoOperand & operator=(TwoOperand const &) = delete;

    virtual double evaluate(double x) const override final {
        return do_operator(lhs_->evaluate(x), rhs_->evaluate(x));
    }

  private:
    virtual void print(std::ostream &os) const override final {
        os << '(' << *lhs_ << get_operator() << *rhs_ << ')';
    }

    /* subclass has to provide function to return its operator */
    virtual char get_operator() const = 0;

    /* subclass has to provide function to do the calculation */
    virtual double do_operator(double lhs, double rhs) const = 0;

    /* left and right hand side operands */
    Expression *lhs_, *rhs_;
};


class Sum final : public TwoOperand {
  public:
    using TwoOperand::TwoOperand;

  private:
    virtual char get_operator() const override {
        return '+';
    }

    virtual double do_operator(double lhs, double rhs) const override {
        return lhs + rhs;
    }
};


class Product final : public TwoOperand {
  public:
    using TwoOperand::TwoOperand;

  private:
    virtual char get_operator() const override {
        return '*';
    }

    virtual double do_operator(double lhs, double rhs) const override {
        return lhs * rhs;
    }
};


int main() {
    Expression *c = new Product{
                        new Constant{5},
                        new Sum{
                            new Constant{3},
                            new Variable
                        }
                    };
    std::cout << "f(x) = " << *c << std::endl;
    std::cout << "f(3) = " << c->evaluate(3) << std::endl;
    delete c;
}
