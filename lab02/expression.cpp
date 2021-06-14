#include <iostream>


class Expression {
  public:
    /* returns value of expression at x */
    virtual double evaluate(double x) const = 0;

    /* return dynamically allocated derivative expression */
    virtual Expression * derivative() const = 0;
    
    /* return dynamically allocated copy of expression */
    virtual Expression * clone() const = 0;
    
    /* return dynamically allocated, simplified version */
    virtual Expression * simplify() const {
        return clone();
    }

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

    virtual Expression * derivative() const override {
        return new Constant{0};
    }

    virtual Expression * clone() const override {
        return new Constant{*this};
    }

    double get_value() const {
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

    virtual Expression * derivative() const override {
        return new Constant{1};
    }

    virtual Expression * clone() const override {
        return new Variable{*this};
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

    /* copy constructor */
    TwoOperand(TwoOperand const & the_other)
        : Expression{*this}
        , lhs_{the_other.lhs_->clone()}, rhs_{the_other.rhs_->clone()} {
    }
    
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

  protected:
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

    virtual Expression * derivative() const override {
        return new Sum{lhs_->derivative(), rhs_->derivative()};
    }

    virtual Expression * clone() const override {
        return new Sum{*this};
    }
    
    virtual Expression * simplify() const override {
        Expression * lhs_simpl = lhs_->simplify();
        Expression * rhs_simpl = rhs_->simplify();
        Constant * lhs_cons = dynamic_cast<Constant *>(lhs_simpl);
        Constant * rhs_cons = dynamic_cast<Constant *>(rhs_simpl);
        if (lhs_cons != nullptr && lhs_cons->get_value() == 0.0) {  /* 0 + a = a */
            delete lhs_simpl;   /* = 0 */
            return rhs_simpl;
        }
        if (rhs_cons != nullptr && rhs_cons->get_value() == 0.0) {  /* a + 0 = a */
            delete rhs_simpl;   /* = 0 */
            return lhs_simpl;
        }
        if (lhs_cons != nullptr && rhs_cons != nullptr) {   /* c + c = c */
            double new_value = lhs_cons->get_value() + rhs_cons->get_value();
            delete lhs_simpl;
            delete rhs_simpl;
            return new Constant{new_value};
        }
        return new Sum{lhs_simpl, rhs_simpl};
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

    virtual Expression * derivative() const override {
        return new Sum{
                   new Product{lhs_->derivative(), rhs_->clone()},
                   new Product{lhs_->clone(), rhs_->derivative()}
               };
    }

    virtual Expression * clone() const override {
        return new Product{*this};
    }

    virtual Expression * simplify() const override {
        Expression * lhs_simpl = lhs_->simplify();
        Expression * rhs_simpl = rhs_->simplify();
        Constant * lhs_cons = dynamic_cast<Constant *>(lhs_simpl);
        Constant * rhs_cons = dynamic_cast<Constant *>(rhs_simpl);
        if (lhs_cons != nullptr && lhs_cons->get_value() == 0.0) {  /* 0 * a = 0 */
            delete rhs_simpl;
            return lhs_simpl;   /* = 0 */
        }
        if (rhs_cons != nullptr && rhs_cons->get_value() == 0.0) {  /* a * 0 = a */
            delete lhs_simpl;
            return rhs_simpl;   /* = 0 */
        }
        if (lhs_cons != nullptr && lhs_cons->get_value() == 1.0) {  /* 1 * a = a */
            delete lhs_simpl;   /* = 1 */
            return rhs_simpl;
        }
        if (rhs_cons != nullptr && rhs_cons->get_value() == 1.0) {  /* a * 1 = a */
            delete rhs_simpl;   /* = 1 */
            return lhs_simpl;
        }
        if (lhs_cons != nullptr && rhs_cons != nullptr) {   /* c * c = c */
            double new_value = lhs_cons->get_value() * rhs_cons->get_value();
            delete lhs_simpl;
            delete rhs_simpl;
            return new Constant{new_value};
        }
        return new Product{lhs_simpl, rhs_simpl};
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
    
    Expression *cd = c->derivative();
    std::cout << "f'(x) = " << *cd << std::endl;
    
    delete c;
    delete cd;
}
