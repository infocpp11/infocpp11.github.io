#include <iostream>
#include <string>
#include <utility>


class Any {
  private:
    class ContainerBase {
      public:
        virtual ~ContainerBase() {}
        virtual ContainerBase *clone() const = 0;
    };
    
    template <typename T>
    class Container : public ContainerBase {
      public:
        T data;
        Container(T const &what) : data(what) {}
        ContainerBase *clone() const { return new Container<T>(*this); }
    };

    ContainerBase *pdata = nullptr;

  public:
    Any() = default;
    Any(Any const &to_copy) {
        pdata = to_copy.pdata->clone();
    }
    Any& operator=(Any const& to_copy) {
        ContainerBase* copy = to_copy.pdata->clone();
        delete pdata;
        pdata = copy;
        return *this;
    }
    ~Any() { delete pdata; }
    
    template <typename T>
    void set(T const &what) {
        ContainerBase *newpdata = new Container<T>(what);
        delete pdata;
        pdata = newpdata;
    }
    
    template <typename T>
    T & get() {
        return dynamic_cast<Container<T>&>(*pdata).data;
    }
    template <typename T>
    T const & get() const {
        return dynamic_cast<Container<T>&>(*pdata).data;
    }
    
};


int main() {
    Any a;
    
    a.set(5);
    std::cout << a.get<int>() << std::endl;
    
    a.set(std::string("hello"));
    std::cout << a.get<std::string>() << std::endl;
}
