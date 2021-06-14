#include <iostream>
#include <vector>

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
    {
        std::vector<Noisy> v(3);
        std::cout << "=====\n";
        v.push_back(Noisy{5});
        std::cout << "=====\n";
    }
    std::cout << "=============\n";
    
    {
        std::vector<Noisy> v2;
        v2.reserve(100);
        v2.push_back(Noisy{6});
        v2.push_back(Noisy{7});
        Noisy::report();
        std::cout << "=====\n";
    }
    std::cout << "=============\n";

    {
        std::vector<Noisy> v2;
        v2.reserve(100);
        v2.emplace_back(8);     // Noisy(8) jön létre a végén
        v2.emplace_back(9);     // Noisy(9) jön létre a végén
        Noisy::report();
        std::cout << "=====\n";
    }
    std::cout << "=============\n";
}
