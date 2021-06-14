#include <iostream>
#include <memory>

class Expression;

using ExpressionPtr = std::shared_ptr<Expression>;

class Expression {
  public:
    /* returns value of expression at x */
    virtual double evaluate(double x) const = 0;

    /* outputs expression as string to stream os */
    virtual void print(std::ostream &os) const = 0;

    /* return derivative expression */
    virtual ExpressionPtr derivative() const = 0;

    /* return simplified version. */
    virtual ExpressionPtr simplify() const = 0;

    Expression() = default;
    
    Expression(Expression const &) = default;
    Expression(Expression &&) = default;
    Expression & operator=(Expression const &) = default;
    Expression & operator=(Expression &&) = default;

    virtual ~Expression() {}
};


/* pass print request to virtual print() method */
std::ostream & operator<<(std::ostream & os, ExpressionPtr e) {
    e->print(os);
    return os;
}


class Constant final : public Expression {
  private:
    double const c_;

  public:
    Constant(double c): c_(c) {}
    
    virtual double evaluate(double) const override {
        return c_;
    }

    virtual void print(std::ostream &os) const override {
        os << c_;
    }
    
    virtual ExpressionPtr derivative() const override {
        return std::make_shared<Constant>(0);
    }
    
    virtual ExpressionPtr simplify() const override {
        return std::make_shared<Constant>(*this);
    }

    double get_value() const {
        return c_;
    }
};


class Variable final : public Expression {
  public:
    virtual double evaluate(double x) const override {
        return x;
    }

    virtual void print(std::ostream &os) const override {
        os << 'x';
    }

    virtual ExpressionPtr derivative() const override {
        return std::make_shared<Constant>(1);
    }

    virtual ExpressionPtr simplify() const override {
        return std::make_shared<Variable>(*this);
    }
};


class TwoOperand : public Expression {
  public:
    TwoOperand(ExpressionPtr lhs, ExpressionPtr rhs)
        : lhs_(lhs), rhs_(rhs) {}

    virtual double evaluate(double x) const override final {
        return do_operator(lhs_->evaluate(x), rhs_->evaluate(x));
    }

    virtual void print(std::ostream &os) const override final {
        os << '(' << lhs_ << get_operator() << rhs_ << ')';
    }

  private:
    /* subclass has to provide function to return its operator */
    virtual char get_operator() const = 0;

    /* subclass has to provide function to do the calculation */
    virtual double do_operator(double lhs, double rhs) const = 0;

  protected:
    /* left and right hand side operands */
    ExpressionPtr const lhs_, rhs_;
};


class Sum final : public TwoOperand {
  public:
    using TwoOperand::TwoOperand;

    virtual ExpressionPtr derivative() const override {
        return std::make_shared<Sum>(lhs_->derivative(), rhs_->derivative());
    }

    virtual ExpressionPtr simplify() const override {
        auto lhs_simpl = lhs_->simplify();
        auto rhs_simpl = rhs_->simplify();
        auto lhs_cons = std::dynamic_pointer_cast<Constant const>(lhs_simpl);
        auto rhs_cons = std::dynamic_pointer_cast<Constant const>(rhs_simpl);
        if (lhs_cons != nullptr && lhs_cons->get_value() == 0.0)    /* 0 + a = a */
            return rhs_simpl;
        if (rhs_cons != nullptr && rhs_cons->get_value() == 0.0)    /* a + 0 = a */
            return lhs_simpl;
        if (lhs_cons != nullptr && rhs_cons != nullptr)   /* c + c = c */
            return std::make_shared<Constant>(lhs_cons->get_value() + rhs_cons->get_value());
        return std::make_shared<Sum>(lhs_simpl, rhs_simpl);
    }


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

    virtual ExpressionPtr derivative() const override {
        return std::make_shared<Sum>(
            std::make_shared<Product>(lhs_->derivative(), rhs_),
            std::make_shared<Product>(lhs_, rhs_->derivative())
        );
    }

    virtual ExpressionPtr simplify() const override {
        auto lhs_simpl = lhs_->simplify();
        auto rhs_simpl = rhs_->simplify();
        auto lhs_cons = std::dynamic_pointer_cast<Constant const>(lhs_simpl);
        auto rhs_cons = std::dynamic_pointer_cast<Constant const>(rhs_simpl);
        if (lhs_cons != nullptr && lhs_cons->get_value() == 0.0)    /* 0 * a = 0 */
            return lhs_simpl;   /* = 0 */
        if (rhs_cons != nullptr && rhs_cons->get_value() == 0.0)    /* a * 0 = a */
            return rhs_simpl;   /* = 0 */
        if (lhs_cons != nullptr && lhs_cons->get_value() == 1.0)    /* 1 * a = a */
            return rhs_simpl;
        if (rhs_cons != nullptr && rhs_cons->get_value() == 1.0)    /* a * 1 = a */
            return lhs_simpl;
        if (lhs_cons != nullptr && rhs_cons != nullptr) {   /* c * c = c */
            double new_value = lhs_cons->get_value() * rhs_cons->get_value();
            return std::make_shared<Constant>(new_value);
        }
        return std::make_shared<Product>(lhs_simpl, rhs_simpl);
    }

  private:
    virtual char get_operator() const override {
        return '*';
    }

    virtual double do_operator(double lhs, double rhs) const override {
        return lhs * rhs;
    }
};


int main() {
    ExpressionPtr c = std::make_shared<Product>(
                          std::make_shared<Constant>(5),
                          std::make_shared<Sum>(
                              std::make_shared<Constant>(3),
                              std::make_shared<Variable>()
                          )
                      );
    std::cout << "f(x) = " << c << std::endl;
    std::cout << "f(3) = " << c->evaluate(3) << std::endl;
    
    std::cout << "f'(x) = " << c->derivative() << std::endl;
    std::cout << "f'(x) egyszerusitve = " << c->derivative()->simplify() << std::endl;

    std::cout << "f''(x) = " << c->derivative()->derivative() << std::endl;
    std::cout << "f''(x) egyszerusitve = " << c->derivative()->derivative()->simplify() << std::endl;
}
