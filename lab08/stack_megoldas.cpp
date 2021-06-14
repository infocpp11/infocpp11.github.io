#include <iostream>
#include <cstdlib>
#include <new>
#include <stdexcept>
#include <algorithm>

template <typename T>
class Stack {
  public:
    Stack(size_t max_size);
    Stack(Stack const &orig);
    Stack & operator=(Stack copy);
    ~Stack();
    void push(T const &what);
    T & top();
    void pop();
    bool empty() const noexcept;
  private:
    size_t size_;
    size_t max_size_;
    T *pData_;
};


template <typename T>
Stack<T>::Stack(size_t max_size) {
    size_ = 0;
    max_size_ = max_size;
    pData_ = (T*) ::operator new(sizeof(T) * max_size_);    /* kivételt dob */
}


/*
 * Copy ctor, delegálással.
 * 
 * A konstruktor delegálás lefoglalja a szükséges memóriaterületet,
 * és egyben biztosítja azt is, hogy fusson a destruktor. Ezután
 * bedobáljuk az objektumokat a lemásolandó veremből; pont olyan
 * sorrendben, ahogy kell. Közben a this->size_ mindvégig helyes
 * értéket tartalmaz, mert a konstruktor kezdetben 0-ra állította,
 * a push() pedig csak akkor növeli majd, ha keletkezett is
 * objektum. Így a destruktor helyes adatot lát, pont annyi darab
 * objektumot szüntet meg szükség esetén, amennyit a push
 * létrehozott.
 */

template <typename T>
Stack<T>::Stack(Stack<T> const &orig) : Stack(orig.max_size_) {
    for (size_t i = 0; i != orig.size_; ++i)
        push(orig.pData_[i]);
}

/*
 * Copy ctor, nyers verzió.
 */
// template <typename T>
// Stack<T>::Stack(Stack<T> const &orig) {
//     size_ = orig.size_;
//     max_size_ = orig.max_size_;
//     pData_ = (T*) ::operator new(sizeof(T) * max_size_);    /* kivételt dob */
// 
//     size_t i;
//     try {
//         for (i = 0; i != size_; ++i)
//             new (&pData_[i]) T{orig.pData_[i]};
//     } catch (...) {
//         /* ha valamelyik kivételt dobott, visszacsináljuk a konstruktorokat... */
//         for (size_t j = 0; j != i; ++j)
//             pData_[j].~T();
//         /* és mivel konstruktorban vagyunk, figyelünk a memóriára is! */
//         ::operator delete(pData_);
//         /* meg persze a hívónak is szólni kell */
//         throw;
//     }
// }

template <typename T>
Stack<T> & Stack<T>::operator=(Stack<T> copy) {
    std::swap(this->size_, copy.size_);
    std::swap(this->max_size_, copy.max_size_);
    std::swap(this->pData_, copy.pData_);
    return *this;
}


template <typename T>
Stack<T>::~Stack() {
    for (size_t i = 0; i != size_; ++i)
        pData_[i].~T();
    ::operator delete(pData_);
}


template <typename T>
void Stack<T>::push(T const &what) {
    if (size_ + 1 > max_size_)
        throw std::length_error("stack tele");
    new (&pData_[size_]) T{what};
    size_++;
}


template <typename T>
void Stack<T>::pop() {
    if (size_ == 0)
        throw std::length_error("stack ures");
    pData_[size_-1].~T();
    size_--;
}


/* Return element on top of stack. */
template <typename T>
T & Stack<T>::top() {
    if (size_ == 0)
        throw std::length_error("stack ures");
    return pData_[size_-1];
}


template <typename T>
bool Stack<T>::empty() const noexcept {
    return size_ == 0;
}


int main() {
    try {
        std::cout << "Irj be szavakat fajl vege jelig!" << std::endl;
        Stack<std::string> s(100);
        std::string word;
        while (std::cin >> word)
            s.push(word);
        while (!s.empty()) {
            std::cout << s.top() << std::endl;
            s.pop();
        }
    } catch (std::exception & e) {
        std::cout << e.what();
    }
}
