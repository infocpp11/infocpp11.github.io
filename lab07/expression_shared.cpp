#include <iostream>
#include <memory>


class Expression;

class Expression {
  public:
    /* returns value of expression at x */
    virtual double evaluate(double x) const = 0;

    /* outputs expression as string to stream os */
    virtual void print(std::ostream &os) const = 0;
    
    virtual std::shared_ptr<Expression> derivative() const = 0;
    
    Expression() = default;
    Expression(Expression const &) = default;

    virtual ~Expression() {}
};


/* pass print request to virtual print() method */
std::ostream & operator<<(std::ostream & os, Expression const &e) {
    e.print(os);
    return os;
}


class Constant final : public Expression {
  private:
    double c_;

  public:
    Constant(double c): c_{c} {}
    
    virtual double evaluate(double) const override {
        return c_;
    }

    virtual void print(std::ostream &os) const override {
        os << c_;
    }
    
    virtual std::shared_ptr<Expression> derivative() const override {
        return std::make_shared<Constant>(0);
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

    virtual std::shared_ptr<Expression> derivative() const override {
        return std::make_shared<Constant>(1);
    }
};


class TwoOperand : public Expression {
  public:
    TwoOperand(std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs) : lhs_{lhs}, rhs_{rhs} {}

    virtual double evaluate(double x) const override final {
        return do_operator(lhs_->evaluate(x), rhs_->evaluate(x));
    }

    virtual void print(std::ostream &os) const override final {
        os << '(' << *lhs_ << get_operator() << *rhs_ << ')';
    }

  private:
    /* subclass has to provide function to return its operator */
    virtual char get_operator() const = 0;

    /* subclass has to provide function to do the calculation */
    virtual double do_operator(double lhs, double rhs) const = 0;

  protected:
    /* left and right hand side operands */
    std::shared_ptr<Expression> lhs_, rhs_;
};


class Sum final : public TwoOperand {
  public:
    using TwoOperand::TwoOperand;
    virtual std::shared_ptr<Expression> derivative() const override {
        return std::make_shared<Sum>(lhs_->derivative(), rhs_->derivative());
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
    virtual std::shared_ptr<Expression> derivative() const override {
        return std::make_shared<Sum>(
            std::make_shared<Product>(lhs_->derivative(), rhs_),
            std::make_shared<Product>(lhs_, rhs_->derivative())
        );
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
    auto f = std::make_shared<Product>(
                std::make_shared<Constant>(5),
                std::make_shared<Sum>(
                    std::make_shared<Constant>(3),
                    std::make_shared<Variable>()
                )
            );
    std::cout << "f(x) = " << *f << std::endl;
    std::cout << "f(3) = " << f->evaluate(3) << std::endl;
    
    auto df = f->derivative();
    std::cout << "f'(x) = " << *df << std::endl;
}
