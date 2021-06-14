#include <iostream>
#include <stdexcept>
#include <string>


struct Point;


struct PointMemberBase {
    char const* name;
    
    PointMemberBase(char const* name) : name(name) {}
    virtual ~PointMemberBase() = 0;
};

PointMemberBase::~PointMemberBase() {
}


template <typename T>
struct PointMember : public PointMemberBase {
    T Point::* mptr_;

    PointMember(char const* name, T Point::* mptr)
      : PointMemberBase(name), mptr_(mptr) {}
};


struct Point {
    double x, y;
    std::string label;
};


PointMemberBase* point_members[] = {
    new PointMember<double>("x", &Point::x),      // konverzió!
    new PointMember<double>("y", &Point::y),
    new PointMember<std::string>("label", &Point::label),
    nullptr,
};


int main() {
    Point p1 = { 2.3, 4.5, "hello" };

    for (size_t i = 0; point_members[i] != nullptr; ++i) {
        auto memberbase = point_members[i];

        std::cout << memberbase->name << " = ";
       
        if (auto member = dynamic_cast<PointMember<double>*>(memberbase)) {
            std::cout << p1.*(member->mptr_) << " (double)";
        }
        else if (auto member = dynamic_cast<PointMember<std::string>*>(memberbase)) {
            std::cout << p1.*(member->mptr_) << " (std::string)";
        }
        else {
            throw std::logic_error("Unknown data type in object");
        }

        std::cout << std::endl;
    }
}
