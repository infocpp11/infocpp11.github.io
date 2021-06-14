/* mire jó: getter és setter helyettesítése egy publikus, fiktív adattaggal. */

#include <algorithm>
#include <iostream>

/* property: milyen CLASS-hoz, milyen T típusú adattag,
 * melyik SETTERPTR tagfüggvény a setter, melyik GETTERPTR tagfüggvény a getter */
template <typename CLASS, typename T,
          void (CLASS::*SETTERPTR)(T const& newvalue),
          T (CLASS::*GETTERPTR)() const>
class Property {
    CLASS& obj;
    /* copy ctor letiltása, hogy muszáj legyen megadni az új szülő
     * objektumot (különben másolódna a referencia!) */
    Property(Property const&) = delete;
    
  public:
    /* a property szülő objektuma. ezt azért kell paraméterként átvenni,
     * mert c++ban nem lehet sehogy kitalálni egy objektumról, hogy egy
     * másik objektum része-e (subobject) */
    Property(CLASS& obj) : obj(obj) {}
    
    /* property beállítása */
    Property& operator= (T const& newvalue) {
        (obj.*SETTERPTR)(newvalue);
        return *this;
    }
    
    /* property lekérdezése */
    operator T () const {
        return (obj.*GETTERPTR)();
    }
    
    /* property = property eset kezelése */
    Property& operator= (Property const& the_other) {
        *this = the_other.operator T const&();
        return *this;
    }
};


/* példa */
class Vektor {
  private:
    /* a memóriaterület eleje és vége. szándékosan két pointer,
     * hogy látszódjon, a getter is csinál valamit. */
    int* databegin;
    int* dataend;
    
    void sizesetter(size_t const& newsize) {
        size_t oldsize = dataend-databegin;
        int* newdata = new int[newsize];
        std::copy(databegin, databegin + std::min(newsize, oldsize), newdata);
        delete[] databegin;
        databegin = newdata;
        dataend = newdata + newsize;
        std::cout << "resizing to " << newsize << std::endl;
    }
    
    size_t sizegetter() const {
        return dataend - databegin;
    }

  public:
    Vektor() : databegin(nullptr), dataend(nullptr), size(*this) {}
    int& operator[] (size_t idx) {
        if (idx >= sizegetter())
            throw "index out of bounds";
        return databegin[idx];
    }

    /* így kell megadni egy propertyt: Vektor osztályhoz,
     * size_t fiktív adattag, amelynek a settere és a gettere ez.
     * a vektor kliense ezt egy adattagnak látja. */
    Property<Vektor, size_t, &Vektor::sizesetter, &Vektor::sizegetter> size;
};


/* példa */
int main() {
    Vektor v;

    v.size = 5;
    v[2] = 1;
    for (int i = 0; i < v.size; ++i)
        std::cout << v[i] << " ";
    std::cout << std::endl;

    v.size = 10;
    v[7] = 7;
    for (int i = 0; i < v.size; ++i)
        std::cout << v[i] << " ";
    std::cout << std::endl;
}
