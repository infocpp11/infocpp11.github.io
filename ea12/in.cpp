#include <iostream>
#include <vector>
#include <iterator>
#include <set>


/*****************************************************************/
/* mindenféle már megismert és újabb segéd típus, a következő ***** kommentig */
template <typename T>
struct iterator_of;

template <typename T>
  requires requires { typename T::iterator; }
struct iterator_of<T> {
    using type = typename T::iterator;
};

template <typename T, size_t N>
struct iterator_of<T[N]> {
    using type = T*;
};

template <typename T>
    using Iterator_of = typename iterator_of<T>::type;


template <typename T>
struct value_type;

template <typename T>
  requires requires { typename T::value_type; }
struct value_type<T> {
    using type = typename T::value_type;
};

template <typename T, size_t N>
struct value_type<T[N]> {
    using type = T;
};

template <typename T>
struct value_type<T*> {
    using type = T;
};

template <typename T>
    using Value_type = typename value_type<T>::type;


template <typename IT>
concept bool Input_iterator =
    requires (IT it) {
        typename Value_type<IT>;
        { it++ }
        { ++it }
        { *it } -> Value_type<IT> const &;
    };


template <typename T1, typename T2>
concept bool Same_type = std::is_same<T1, T2>::value;


template <typename R>
concept bool Range =
    requires (R & range) {
        typename Value_type<R>;
        typename Iterator_of<R>;

        { std::begin(range) } -> Iterator_of<R>;
        { std::end(range) } -> Iterator_of<R>;

        requires Input_iterator<Iterator_of<R>>;
        requires Same_type<Value_type<R>, Value_type<Iterator_of<R>>>;
    };


template <typename T>
concept bool Equality_comparable =
    requires (T a, T b) {
        { a == b } -> bool;
        { a != b } -> bool;
    };


template <typename T>
struct key_type;

template <typename T>
  requires requires { typename T::key_type; }
struct key_type<T> {
    using type = typename T::key_type;
};

template <typename T>
using Key_type = typename key_type<T>::type;
/*****************************************************************/


/* Asszociatív tároló: olyan tartomány, amiben kulcs szerint lehet keresni. */
template <typename AC>
concept bool Associative_container =
    Range<AC>
    && requires (AC & ac, Key_type<AC> & k) {
        { ac.find(k) } -> Iterator_of<AC>;
        { ac.count(k) } -> int;
    };

/* Sorozat: olyan tartomány, amely megtartja az elemek sorrendjét. */
template <typename S>
concept bool Sequence =
    Range<S>
    && requires (S & s) {
        { s.front() } -> Value_type<S> &;
        { s.back() } -> Value_type<S> &;
    };


/* Keresés sorozatban */
template<Sequence S, Equality_comparable T>
  requires Same_type<T, Value_type<S>>
bool in(const S & seq, const T & value) {
    std::cout << "linear search in()\n";
    for (const auto & x : seq)
        if (x == value)
            return true;
    return false;
}


/* Keresés asszociatív tárolóban */
template<Associative_container AC, Equality_comparable T>
  requires Same_type<T, Value_type<AC>>
bool in(const AC & assoc, const T & value) {
    std::cout << "associative container in()\n";
    return assoc.find(value) != assoc.end();
}


int main() {
    std::vector<int> v = { 1, 2, 3 };
    std::cout << in(v, 2) << std::endl;

    std::set<int> s = { 1, 2, 3 };
    std::cout << in(s, 2) << std::endl;
}
