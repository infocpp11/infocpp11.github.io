#include <iostream>
#include <new>
#include <initializer_list>


template <typename T>
class MyVector {
  public:
    explicit MyVector(size_t size = 0);
    MyVector(std::initializer_list<T> list);
    MyVector(MyVector const &other);
    MyVector& operator=(MyVector const &other);
    ~MyVector();
    void reserve(size_t newcapacity);
    void push_back(T const &what);
    T & operator[](size_t idx) { return pData_[idx]; }
    T const & operator[](size_t idx) const { return pData_[idx]; }
    using iterator = T*;
    iterator begin() {
        return pData_;
    }
    iterator end() {
        return pData_ + size_;
    }
    using const_iterator = T const *;
    const_iterator cbegin() const {
        return pData_;
    }
    const_iterator cend() const {
        return pData_ + size_;
    }
  private:
    T *pData_;
    size_t size_;
    size_t capacity_;
};


/* Constructor for MyVector.
 * Allocate memory, then call default constructor of elements. */
template <typename T>
MyVector<T>::MyVector(size_t size) {
    size_ = size;
    capacity_ = size;
    pData_ = static_cast<T*>(::operator new(sizeof(T) * capacity_));
    for (size_t i = 0; i != size_; ++i)
        new (&pData_[i]) T{};
}


/* Destructor for MyVector. */
/* Call destructor of elements, then deallocate memory. */
template <typename T>
MyVector<T>::~MyVector() {
    for (size_t i = 0; i != size_; ++i)
        pData_[i].~T();
    ::operator delete(pData_);
}


/* Copy constructor for MyVector. */
/* Allocate memory, then copy elements.
 * Capacity of new vector can be independent of the one copied. */
template <typename T>
MyVector<T>::MyVector(MyVector<T> const &other) {
    size_ = other.size_;
    capacity_ = size_;
    pData_ = static_cast<T*>(::operator new(sizeof(T) * capacity_));
    for (size_t i = 0; i != size_; ++i)
        new (&pData_[i]) T{other.pData_[i]};
}


/* Copy assignment operator for MyVector. */
/* Delete old elements.
 * Resize allocated memory, if needed.
 * (Maybe we should also resize if new size is significantly smaller.)
 * Copy new elements. */
template <typename T>
MyVector<T> & MyVector<T>::operator=(MyVector<T> const &other) {
    if (this == &other)
        return *this;
    for (size_t i = 0; i != size_; ++i)
        pData_[i].~T();
    size_ = other.size_;
    if (capacity_ < size_) {
        ::operator delete(pData_);
        capacity_ = other.capacity_;
        pData_ = static_cast<T*>(::operator new(sizeof(T) * capacity_));
    }
    for (size_t i = 0; i != size_; ++i)
        new (&pData_[i]) T{other.pData_[i]};
    return *this;
}


/* Reserve space for adding more elements later. */
template <typename T>
void MyVector<T>::reserve(size_t newcapacity) {
    if (newcapacity < size_)
        return;

    T *newPData = static_cast<T*>(::operator new(sizeof(T) * newcapacity));
    for (size_t i = 0; i != size_; ++i)
        new (&newPData[i]) T{pData_[i]};

    for (size_t i = 0; i != size_; ++i)
        pData_[i].~T();
    ::operator delete(pData_);
    capacity_ = newcapacity;
    pData_ = newPData;
}


/* Append new element. */
template <typename T>
void MyVector<T>::push_back(T const &what) {
    if (size_ + 1 > capacity_)
        reserve(size_ + 10);
    new (&pData_[size_]) T{what};
    ++size_;
}


/* Construct using values from initializer list. */
template <typename T>
MyVector<T>::MyVector(std::initializer_list<T> list)
 : MyVector() {
    reserve(list.size());
    for (auto it = list.begin(); it != list.end(); ++it)
        push_back(*it);
}


int main() {
    MyVector<int> v = { 3, 4, 5 };
    
    for (auto i : v)
        std::cout << i << std::endl;
}
