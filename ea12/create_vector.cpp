#include <iostream>
#include <iterator>
#include <vector>
#include <type_traits>

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


/* Iterator_category */
template <typename IT>
    using Iterator_category = typename std::iterator_traits<IT>::iterator_category;

/* Derived */
template <typename DERIVED, typename BASE>
concept bool Derived_from =
    std::is_base_of<BASE, DERIVED>::value;



template <typename IT>
concept bool Input_iterator =
    requires (IT it) {
        typename Value_type<IT>;
        { *it } -> Value_type<IT> const &;
        { ++it } -> IT &;
    };

template <typename IT>
concept bool Forward_iterator =
    Input_iterator<IT>
    && requires {
        typename Iterator_category<IT>;
        requires Derived_from<Iterator_category<IT>, std::forward_iterator_tag>;
    };


template <Input_iterator IT>
auto create_vector_from_range(IT begin, IT end) {
    std::cout << "input iterator" << std::endl;
    std::vector<Value_type<IT>> v;
    while (begin != end) {
        v.push_back(*begin);
        ++begin;
    }
    return v;
}

template <Forward_iterator IT>
auto create_vector_from_range(IT begin, IT end) {
    std::cout << "forward iterator\n";
    std::vector<Value_type<IT>> v;
    size_t num_elements = std::distance(begin, end);
    v.reserve(num_elements);
    while (begin != end) {
        v.push_back(*begin);
        ++begin;
    }
    return v;
}


int main() {
    std::istream_iterator<int> cin_begin(std::cin), cin_end;

    auto v1 = create_vector_from_range(cin_begin, cin_end);
    auto v2 = create_vector_from_range(v1.begin(), v1.end());
}
