#include <iostream>
#include <cstdlib>
#include <new>
#include <stdexcept>
#include <string>

template <typename T>
class Stack {
  public:
    explicit Stack(size_t max_size);
    Stack(Stack const &orig);
    Stack & operator=(Stack const &orig) = delete;
    ~Stack();
    void push(T const &what);
    T pop();
    bool empty() const;
  private:
    size_t size_;
    size_t max_size_;
    T *pData_;
};


template <typename T>
Stack<T>::Stack(size_t max_size) {
    size_ = 0;
    max_size_ = max_size;
    pData_ = (T*) malloc(sizeof(T) * max_size_);
}


template <typename T>
Stack<T>::Stack(Stack<T> const &orig) {
    size_ = orig.size_;
    max_size_ = orig.max_size_;
    pData_ = (T*) malloc(sizeof(T) * max_size_);
    for (size_t i = 0; i != size_; ++i)
        new (&pData_[i]) T{orig.pData_[i]};
}


template <typename T>
Stack<T>::~Stack() {
    for (size_t i = 0; i != size_; ++i)
        pData_[i].~T();
    free(pData_);
}


template <typename T>
void Stack<T>::push(T const &what) {
    size_++;
    new (&pData_[size_ - 1]) T{what};
}


template <typename T>
T Stack<T>::pop() {
    size_--;
    T saved{pData_[size_]};
    pData_[size_].~T();
    return saved;
}


template <typename T>
bool Stack<T>::empty() const {
    return size_ == 0;
}


int main() {
    std::cout << "Irj be szavakat fajl vege jelig!" << std::endl;
    Stack<std::string> s(100);
    std::string word;
    while (std::cin >> word)
        s.push(word);
    while (!s.empty())
        std::cout << s.pop() << std::endl;
}
