#include <iostream>

class Noisy {
  public:
    Noisy() : i_{0} {
        std::cout << "Noisy default ctor\n"; count++;
    }
    explicit Noisy(int i) : i_{i} {
        std::cout << "Noisy{" << i << "} ctor\n"; count++;
    }
    Noisy(Noisy const &o) : i_{o.i_} {
        std::cout << "Noisy copy ctor " << i_ << "\n"; count++;
    }
    ~Noisy() {
        std::cout << "Noisy dtor " << i_ << "\n"; count--;
    }
    Noisy& operator=(Noisy const &o) {
        i_ = o.i_;
        std::cout << "Noisy copy assignment " << i_ << "\n";
        return *this;
    }
    static void report() {
        std::cout << count << " instance(s).\n";
    }
  private:
    int i_;
    static int count;
};

int Noisy::count = 0;

int main() {
    Noisy::report();
    {
        Noisy y;
        Noisy::report();
    }
    Noisy::report();
}
