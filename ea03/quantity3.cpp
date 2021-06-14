#include <iostream>


namespace Units {
    /* quantity in SI unit */
    template <int M, int KG, int S>
    class Quantity {
    public:
        double magnitude;
        constexpr explicit Quantity(double magnitude): magnitude{magnitude} {}
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
    using Torque = Quantity<2, 1, -2>;         /* forgatónyomaték, Nm=m^2*kg/s^2 */
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


    /* user-defined literals for SI quantities */
    namespace Literals {
        constexpr Length operator "" _m (long double magnitude) {
            return Length(magnitude);
        }

        constexpr Length operator "" _km (long double magnitude) {
            return Length(magnitude * 1000.0);
        }

        constexpr Time operator "" _s (long double magnitude) {
            return Time(magnitude);
        }

        constexpr Time operator "" _h (long double magnitude) {
            return Time(magnitude * 3600);
        }

        constexpr Mass operator "" _kg (long double magnitude) {
            return Mass(magnitude);
        }

        constexpr Force operator "" _N (long double magnitude) {
            return Force(magnitude);
        }
    } /* namespace Literals */
} /* namespace Units */


int main() {
    using namespace Units::Literals;

    std::cout << "Ferrari ===" << std::endl;
    Units::Speed v = 100.0_km / 1.0_h;
    std::cout << "100 km/h = " << v << std::endl;
    
    Units::Acceleration a = v / 4.0_s;
    Units::Force f = 1450.0_kg * a;          /* Newton */
    std::cout << "F = " << f << std::endl;
}
