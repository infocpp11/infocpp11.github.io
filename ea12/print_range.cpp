#include <iostream>
#include <vector>
#include <iterator>


/*****************************************************************/
/* Szerializálhatóság (az előző részből). */
template <typename T>
concept bool Serializable =
    requires (std::ostream & os, std::istream & is, T obj) {
        { os << obj } -> std::ostream &;
        { is >> obj } -> std::istream &;
    };


/*****************************************************************/
/* Hogy hívják a tároló iterátorát? */
template <typename T>
struct iterator_of;

/* Általában egy osztályról van szó, aminek az ::iterator belső
 * típusa megadja. Ez a sablon csak akkor példányosítható, ha van T::iterator!
 * Így ez egy specializáció (!). Az alap sablonnál erősebb
 * megkötésekkel rendelkező sablon is specializációnak számít.
 * A requires szó nem véletlenül van itt kétszer; az első a megkötést
 * adja, a második pedig egy requires-expression része.
 * Ez egy ad-hoc concept. */
template <typename T>
  requires requires { typename T::iterator; }
struct iterator_of<T> {
    using type = typename T::iterator;
};

/* Tömböknél nincs ::iterator belső típus, viszont azokról tudjuk,
 * hogy a pointer az iterátoruk. */
template <typename T, size_t N>
struct iterator_of<T[N]> {
    using type = T*;
};

/* Iterator_of segédtípus: a feladat delegálása az iterator_of<T> osztálynak.
 * Erre azért volt szükség, mert a using-gal létrehozott alias nem specializálható,
 * egy osztály viszont igen. */
template <typename T>
    using Iterator_of = typename iterator_of<T>::type;


/*****************************************************************/
/* Milyen típust tárol? */
template <typename T>
struct value_type;

/* Általában egy osztályról van szó, aminek a ::value_type belső típusa megadja.
 * A fentihez hasonló módon ez is specializáció. */
template <typename T>
  requires requires { typename T::value_type; }
struct value_type<T> {
    using type = typename T::value_type;
};

/* De tömbnél nem lehet belső típus, ezért arra is specializálunk */
template <typename T, size_t N>
struct value_type<T[N]> {
    using type = T;
};

/* Pointer esetén a mutatott elem */
template <typename T>
struct value_type<T*> {
    using type = T;
};


/* Value_type segédtípus: a feladat delegálása a value_type<T> osztálynak. */
template <typename T>
    using Value_type = typename value_type<T>::type;


/*****************************************************************/
/* Input_iterator: az a típus, ami léptethető (++), dereferálhat (*),
 * és a dereferálás a meghatározott típusú elemet adja. */
template <typename IT>
concept bool Input_iterator =
    requires (IT it) {
        typename Value_type<IT>;
        { it++ }
        { ++it }
        { *it } -> Value_type<IT> const &;
    };


/*****************************************************************/
/* Szintén concept-ként használható: két egyforma típusról van-e szó.
 * Egyszerűen delegáljuk a feladatot az std::is_same osztálynak. */
template <typename T1, typename T2>
concept bool Same_type = std::is_same<T1, T2>::value;


/*****************************************************************/
/* Amiért a fentieket létrehoztuk: a tartomány fogalmának meghatározása. */
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


/*****************************************************************/
/* Tartomány elemeinek kiírása a kimenetre. A paraméter egy tartomány kell legyen,
 * amely szerializálható elemeket kell tartalmazzon. */
template <Range R>
    requires Serializable<Value_type<R>>
void print_range(R const & range)
{
    for (auto && val : range)
        std::cout << val;
}


int main() {
    std::vector<int> v = { 1, 2, 3 };
    print_range(v);
    int t[] = { 1, 2, 3 };
    print_range(t);
}
