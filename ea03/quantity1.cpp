#include <iostream>


/* quantity in SI unit */
template <int M, int KG, int S>
class Quantity {
  public:
    double magnitude;
    explicit Quantity(double magnitude): magnitude{magnitude} {}
};

/* short names for quantities */
using Length = Quantity<1, 0, 0>;          /* hossz, m */
using Area = Quantity<2, 0, 0>;            /* terület, m^2 */
using Mass = Quantity<0, 1, 0>;            /* tömeg, kg */
using Time = Quantity<0, 0, 1>;            /* idő, s */
using Speed = Quantity<1, 0, -1>;          /* sebesség, m/s */
using Acceleration = Quantity<1, 0, -2>;   /* gyorsulás, m/s^2 */
using Force = Quantity<1, 1, -2>;          /* erő, N=m*kg/s^2 */
using Energy = Quantity<2, 1, -2>;         /* energia, J=m^2*kg/s^2 */
using Power = Quantity<2, 1, -3>;          /* teljesítmény, Watt = Joule/s = m^2*kg/s^3 */


/* operators for quantities */
template <int M, int KG, int S>
Quantity<M, KG, S> operator+(Quantity<M, KG, S> a, Quantity<M, KG, S> b) {
    return Quantity<M, KG, S>{a.magnitude + b.magnitude};
}

template <int M, int KG, int S>
Quantity<M, KG, S> operator-(Quantity<M, KG, S> a, Quantity<M, KG, S> b) {
    return Quantity<M, KG, S>{a.magnitude - b.magnitude};
}

template <int M1, int KG1, int S1, int M2, int KG2, int S2>
Quantity<M1+M2, KG1+KG2, S1+S2> operator*(Quantity<M1, KG1, S1> a, Quantity<M2, KG2, S2> b) {
    return Quantity<M1+M2, KG1+KG2, S1+S2>{a.magnitude * b.magnitude};
}

template <int M1, int KG1, int S1, int M2, int KG2, int S2>
Quantity<M1-M2, KG1-KG2, S1-S2> operator/(Quantity<M1, KG1, S1> a, Quantity<M2, KG2, S2> b) {
    return Quantity<M1-M2, KG1-KG2, S1-S2>{a.magnitude / b.magnitude};
}

template <int M, int KG, int S>
bool operator<(Quantity<M, KG, S> a, Quantity<M, KG, S> b) {
    return a.magnitude < b.magnitude;
}


/* generic stream inserter operator for quantities */
template <int M, int KG, int S>
std::ostream & operator<<(std::ostream & os, Quantity<M, KG, S> m) {
    os << m.magnitude << ' ';
    bool elso = true;
    if (M != 0) {
        elso = false;
        os << "m^" << M;
    }
    if (KG != 0) {
        if (!elso) os << '*';
        elso = false;
        os << "kg^" << KG;
    }
    if (S != 0) {
        if (!elso) os << '*';
        elso = false;
        os << "s^" << S;
    }
    return os;
}

/* specialized stream inserter operator for force */
template <>
std::ostream & operator<<(std::ostream & os, Force m) {
    os << m.magnitude << " N";
    return os;
}


int main() {
    std::cout << "Ferrari ===" << std::endl;
    Length l{100 * 1000};   /* 100 km */
    Time hour{3600};        /* óra = 3600 s */
    Time t{4};              /* gyorsulás: 4 s alatt */
    Mass m{1450};           /* az autó tömege: 1450 kg */
    
    Speed v = l/hour;
    std::cout << "100 km/h = " << v << std::endl;
    
    Acceleration a = v/t;
    Force f = m*a;          /* Newton */
    std::cout << "F = " << f << std::endl;
}
