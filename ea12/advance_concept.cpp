#include <iostream>
#include <vector>
#include <list>

template <typename T>
struct value_type;

/* Milyen típust tárol? Általában egy osztályról
 * van szó, aminek a ::value_type belső típusa megadja.
 * Ez a sablon csak akkor példányosítható, ha van T::value_type!
 * Így ez egy specializáció (!). Az alap sablonnál erősebb
 * megkötésekkel rendelkező sablon is specializációnak számít. */
template <typename T>
  requires requires { typename T::value_type; }
struct value_type<T> {
    using type = typename T::value_type;
};

/* de tömbnél nem lehet belső típus, ezért ott specializálunk */
template <typename T, size_t N>
struct value_type<T[N]> {
    using type = T;
};

/* pointer esetén a mutatott elem*/
template <typename T>
struct value_type<T*> {
    using type = T;
};

/* Value_type típus: a feladat delegálása
 * a value_type<T> osztálynak (az specializálható!) */
template <typename T>
    using Value_type = typename value_type<T>::type;




template <typename IT>
concept bool Forward_iterator =
    requires (IT it) {
        typename Value_type<IT>;
        { *it } -> Value_type<IT> const &;
        { ++it } -> IT &;
        { it++ } -> IT;
    };

template <typename IT>
concept bool Random_access_iterator =
    Forward_iterator<IT>
    && requires (IT it, int n) {
        { it + n } -> IT;
        { it - it } -> int;
        { it += n } -> IT &;
    };

template <Forward_iterator IT>
void advance(IT & it, int n) {
    for (int i = 0; i < n; ++i)
        ++it;
}

template <Random_access_iterator IT>
void advance(IT & it, int n) {
    it += n;
}


int main() {
    std::list<int> l = { 1, 2, 3 };
    auto li = l.begin();
    advance(li, 2);

    std::vector<int> v = { 1, 2, 3 };
    auto vi = v.begin();
    advance(vi, 2);
}
