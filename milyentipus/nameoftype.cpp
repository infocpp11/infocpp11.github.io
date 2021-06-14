#include <iostream>
#include <string>
#include <cmath>

template <typename T>
struct NameOfType {
};

/* beépített típusok (még lehetne párat) */
template <>
struct NameOfType<int> {
    static std::string get() { return "int"; }
};

template <>
struct NameOfType<char> {
    static std::string get() { return "char"; }
};

template <>
struct NameOfType<double> {
    static std::string get() { return "double"; }
};

template <>
struct NameOfType<void> {
    static std::string get() { return "void"; }
};

/* módosítók - tömb */
template <typename T, size_t S>
struct NameOfType<T[S]> {
    static std::string get() {
        char s[100];
        sprintf(s, "%d", (int)S);
        return "array[" + std::string(s) + "] of " + NameOfType<T>::get();
    }
};

template <typename T, size_t S>
struct NameOfType<T const[S]> {
    static std::string get() {
        char s[100];
        sprintf(s, "%d", (int)S);
        return "array[" + std::string(s) + "] of " + NameOfType<T const>::get();
    }
};

/* módosítók */
template <typename T>
struct NameOfType<T const> {
    static std::string get() { return NameOfType<T>::get() + " const" ; }
};

template <typename T>
struct NameOfType<T &> {
    static std::string get() { return "reference of " + NameOfType<T>::get(); }
};

template <typename T>
struct NameOfType<T &&> {
    static std::string get() { return "rvalue reference of " + NameOfType<T>::get(); }
};

template <typename T>
struct NameOfType<T *> {
    static std::string get() { return "pointer to " + NameOfType<T>::get(); }
};

/* nulla paraméterű függvény */
template <typename RET>
struct NameOfType<RET()> {
    static std::string get() { return "function of () returning " + NameOfType<RET>::get(); }
};

/* függvény egy paraméterrel */
template <typename RET, typename ARG>
struct NameOfType<RET(ARG)> {
    static std::string get() { return "function of (" + NameOfType<ARG>::get() + ") returning " + NameOfType<RET>::get(); }
};


int main() {
    char const** tomb[2];
    std::cout << NameOfType<decltype(tomb)>::get() << std::endl;
    std::cout << NameOfType<int (&)[20]>::get() << std::endl;
    std::cout << NameOfType<decltype(&sin)>::get() << std::endl;
    std::cout << NameOfType<decltype("hello")>::get() << std::endl;
}
