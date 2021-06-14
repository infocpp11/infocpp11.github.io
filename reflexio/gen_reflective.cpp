#include <iostream>
#include <stdexcept>


class Reflective;


/* Ősosztály az MptrType<T>-k számára. */
struct Mptr {
    char const *name;

    Mptr(char const *name) : name(name) {}
    virtual ~Mptr() = 0;
};

Mptr::~Mptr() {
}


/* Valamilyen reflektív objektum MEMBER_TYPE típusú
 * adattagját lekérdezni tudó segédobjektum. A reflektív
 * objektum pontos típusa itt még nem ismert, ezért a
 * get() tisztán virtuális. */
template <typename MEMBER_TYPE>
struct MptrType : public Mptr {
    using Mptr::Mptr;
    
    virtual MEMBER_TYPE& get(Reflective& obj) const = 0;
};


/* CLASS_NAME típusú reflektív objektum MEMBER_TYPE típusú
 * adattagját lekérdezni tudó segédobjektum. MptrType<MEMBER_TYPE>
 * megvalósítása. */
template <typename CLASS_NAME, typename MEMBER_TYPE>
struct MptrTypeClass : public MptrType<MEMBER_TYPE> {
    MEMBER_TYPE CLASS_NAME::* mptr;

    MptrTypeClass(char const *name, MEMBER_TYPE CLASS_NAME::* mptr)
      : MptrType<MEMBER_TYPE>(name), mptr(mptr) { }
      
    virtual MEMBER_TYPE& get(Reflective& obj) const {
        return dynamic_cast<CLASS_NAME&>(obj).*mptr;
    }
};


/* Elvárás a reflektív objektumokkal szemben:
 *  - származzanak le ebből az osztályból,
 *  - valósítsák meg a get_memberdata() virtuális függvényt
 *    úgy, hogy az visszaadja az adattagjaikat leíró tömböt. */
class Reflective {
  public:
    virtual Mptr** get_memberdata() const = 0;
    virtual ~Reflective() = default;
};


/* Példa a reflektív interfész használatára:
 * Tetszőleges reflektív objektum adattagjait kiíró függvény */
void print_data_in_obj(Reflective& obj) {
    Mptr** memberdata = obj.get_memberdata();
    for (size_t i = 0; memberdata[i] != nullptr; ++i) {
        if (auto member = dynamic_cast<MptrType<double>*>(memberdata[i])) {
            std::cout << memberdata[i]->name << " = " << member->get(obj) << " double " << std::endl;
        }
        else if (auto member = dynamic_cast<MptrType<std::string>*>(memberdata[i])) {
            std::cout << memberdata[i]->name << " = '" << member->get(obj) << "' std::string " << std::endl;
        }
        else {
            throw std::logic_error("Unknown data type");
        }
    }
}


/* Példa reflektív osztályra. */
class Point : public Reflective {
  public:
    double x, y;
    std::string label;

    /* Az interfész megvalósítása - triviális */
    virtual Mptr** get_memberdata() const {
        static Mptr* memberdata[] = {
            new MptrTypeClass<Point, double>("x", &Point::x) ,
            new MptrTypeClass<Point, double>("y", &Point::y),
            new MptrTypeClass<Point, std::string>("label", &Point::label),
            nullptr,
        };
        return memberdata;
    }
};


int main() {
    Point p1;
    p1.x = 2.3;
    p1.y = 4.5;
    p1.label = "hello";

    print_data_in_obj(p1);
}
