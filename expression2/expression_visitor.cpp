#include <iostream>
#include <memory>


class ExpressionVisitor;

class Expression {
  public:
    virtual void accept_visitor(ExpressionVisitor &v) = 0;

    Expression() = default;
    
    Expression(Expression const &) = default;
    Expression(Expression &&) = default;
    Expression & operator=(Expression const &) = default;
    Expression & operator=(Expression &&) = default;

    virtual ~Expression() {}
};


class Constant;
class Variable;
class Sum;
class Product;

class ExpressionVisitor {
  public:
    virtual void visit_constant(Constant &) = 0;
    virtual void visit_variable(Variable &) = 0;
    virtual void visit_sum(Sum &) = 0;
    virtual void visit_product(Product &) = 0;
    
    ExpressionVisitor() = default;
    ExpressionVisitor(ExpressionVisitor const &) = default;
    ExpressionVisitor(ExpressionVisitor &&) = default;
    ExpressionVisitor & operator=(ExpressionVisitor const &) = default;
    ExpressionVisitor & operator=(ExpressionVisitor &&) = default;
    
    virtual ~ExpressionVisitor() {}
};


class Constant final : public Expression {
  private:
    double const c_;

  public:
    Constant(double c): c_(c) {}

    virtual void accept_visitor(ExpressionVisitor &v) override {
        v.visit_constant(*this);
    }
    
    double get_value() const {
        return c_;
    }
};


class Variable final : public Expression {
  public:
    virtual void accept_visitor(ExpressionVisitor &v) override {
        v.visit_variable(*this);
    }
};


class TwoOperand : public Expression {
  public:
    TwoOperand(std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs)
        : lhs_(lhs), rhs_(rhs) {}

    std::shared_ptr<Expression> get_lhs() const {
        return lhs_;
    }
    std::shared_ptr<Expression> get_rhs() const {
        return rhs_;
    }
    
  private:
    std::shared_ptr<Expression> const lhs_, rhs_;
};


class Sum final : public TwoOperand {
  public:
    using TwoOperand::TwoOperand;

    virtual void accept_visitor(ExpressionVisitor &v) override {
        v.visit_sum(*this);
    }
};


class Product final : public TwoOperand {
  public:
    using TwoOperand::TwoOperand;

    virtual void accept_visitor(ExpressionVisitor &v) override {
        v.visit_product(*this);
    }
};


class ExpressionPrinter final: public ExpressionVisitor {
  private:
    std::ostream &os_;

    void print_twooperand(TwoOperand &t, char op) {
        os_ << '(';
        print(*t.get_lhs());
        os_ << op;
        print(*t.get_rhs());
        os_ << ')';
    }

  public:
    explicit ExpressionPrinter(std::ostream &os) : os_(os) {}
    
    void print(Expression &e) {
        e.accept_visitor(*this);
    }
    
    virtual void visit_constant(Constant &c) override {
        os_ << c.get_value();
    }
    
    virtual void visit_variable(Variable &v) override {
        os_ << 'x';
    }
    
    virtual void visit_sum(Sum &s) override {
        print_twooperand(s, '+');
    }
    
    virtual void visit_product(Product &p) override {
        print_twooperand(p, '*');
    }
};


std::ostream & operator<<(std::ostream & os, Expression &e) {
    ExpressionPrinter ep(os);
    ep.print(e);
    return os;
}


class ExpressionEvaluator final: public ExpressionVisitor {
  private:
    double x_;
    double result_;

    void evaluate_twooperand(TwoOperand &t, double (*do_op)(double, double)) {
        double left = evaluate(*t.get_lhs());
        double right = evaluate(*t.get_rhs());
        result_ = do_op(left, right);
    }

  public:
    explicit ExpressionEvaluator(double x) : x_(x) {}
    
    double evaluate(Expression &e) {
        e.accept_visitor(*this);
        return result_;
    }
    
    virtual void visit_constant(Constant &c) override {
        result_ = c.get_value();
    }
    
    virtual void visit_variable(Variable &v) override {
        result_ = x_;
    }
    
    virtual void visit_sum(Sum &s) override {
        evaluate_twooperand(s, [](double a, double b) { return a+b; });
    }
    
    virtual void visit_product(Product &p) override {
        evaluate_twooperand(p, [](double a, double b) { return a*b; });
    }
};


int main() {
    std::shared_ptr<Expression> c = 
                    std::make_shared<Product>(
                        std::make_shared<Constant>(5),
                        std::make_shared<Sum>(
                            std::make_shared<Constant>(3),
                            std::make_shared<Variable>()
                        )
                    );
    std::cout << "f(x) = " << *c << std::endl;
    ExpressionEvaluator ee(3);
    std::cout << "f(3) = " << ee.evaluate(*c) << std::endl;
}
