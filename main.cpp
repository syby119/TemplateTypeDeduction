#include <iostream>
#include <type_traits>
#include <regex>

#define ENABLE_BOOST

#ifdef ENABLE_BOOST
#include <boost/type_index.hpp>
#else
namespace boost {
    namespace typeindex {
        template <typename T>
        struct type_id_with_cvr {
            std::string pretty_name() const {
                return "";
            }
        };
    }
}
#endif

struct TemplateTypeInfos {
    std::string param;
    std::string T;
};

std::string remove__ptr64(const std::string& s) {
    std::regex re(R"(\b(__ptr64)\b)");
    return std::regex_replace(s, re, "");
}

template <typename T>
TemplateTypeInfos passByValue(T t) {
    return {
        remove__ptr64(boost::typeindex::type_id_with_cvr<decltype(t)>().pretty_name()),
        remove__ptr64(boost::typeindex::type_id_with_cvr<T>().pretty_name()),
    };
}

template <typename T>
TemplateTypeInfos passByRef(T& t) {
    return {
        remove__ptr64(boost::typeindex::type_id_with_cvr<decltype(t)>().pretty_name()),
        remove__ptr64(boost::typeindex::type_id_with_cvr<T>().pretty_name()),
    };
}

template <typename T>
TemplateTypeInfos passByURef(T&& t) {
    return {
        remove__ptr64(boost::typeindex::type_id_with_cvr<decltype(t)>().pretty_name()),
        remove__ptr64(boost::typeindex::type_id_with_cvr<T>().pretty_name()),
    };
}

template <typename T>
TemplateTypeInfos passByPtr(T* t) {
    return {
        remove__ptr64(boost::typeindex::type_id_with_cvr<decltype(t)>().pretty_name()),
        remove__ptr64(boost::typeindex::type_id_with_cvr<T>().pretty_name()),
    };
}

template <typename T>
TemplateTypeInfos passByCPtr(T const* t) {
    return {
        remove__ptr64(boost::typeindex::type_id_with_cvr<decltype(t)>().pretty_name()),
        remove__ptr64(boost::typeindex::type_id_with_cvr<T>().pretty_name()),
    };
}

template <typename T>
TemplateTypeInfos passByCRef(T const& t) {
    return {
        remove__ptr64(boost::typeindex::type_id_with_cvr<decltype(t)>().pretty_name()),
        remove__ptr64(boost::typeindex::type_id_with_cvr<T>().pretty_name()),
    };
}

template <typename T>
TemplateTypeInfos passByCRRef(T const&& t) {
    return {
        remove__ptr64(boost::typeindex::type_id_with_cvr<decltype(t)>().pretty_name()),
        remove__ptr64(boost::typeindex::type_id_with_cvr<T>().pretty_name()),
    };
}

std::ostream& operator<<(std::ostream& os, const TemplateTypeInfos& infos) {
    std::cout << "  + param type: " << infos.param << std::endl;
    std::cout << "  + T:          " << infos.T << std::endl;
    return os;
}

void f(void) { }

#define PRINT_INFO(var, func) do { \
    std::cout << remove__ptr64(boost::typeindex::type_id_with_cvr<decltype(var)>().pretty_name()) \
            << '\n' << func(var) << std::endl; \
} while(0);

void printPassByValueInfos() {
    std::cout << "################################## Pass By Value ##################################" << std::endl;
    std::cout << "template <typename T>" << std::endl;
    std::cout << "void f(T param);\n" << std::endl;

    {
        int a = 10;
        PRINT_INFO(a, passByValue); // Param: int, T: int

        int* pa = nullptr;
        PRINT_INFO(pa, passByValue); // Param: int*, T: int*

        int& ra = a;
        PRINT_INFO(ra, passByValue); // Param: int, T: int

        int&& rra = std::move(a);
        PRINT_INFO(rra, passByValue); // Param: int, T: int
    }

    {
        int const ca = 10;
        PRINT_INFO(ca, passByValue); // Param: int, T: int

        int const* cpa = nullptr;
        PRINT_INFO(cpa, passByValue); // Param: int const*, T: int const*

        int const* const pca = &ca;
        PRINT_INFO(pca, passByValue); // Param: int const*, T: int const*

        int const& cra = ca;
        PRINT_INFO(cra, passByValue); // Param: int, T: int

        int const&& crra = 10;
        PRINT_INFO(crra, passByValue); // Param: int, T: int

        // const int& const crca = a; // <- warning C4220: qualifiers on reference are ignored
        // const int&& const crrca = 10; // <- warning C4220: qualifiers on reference are ignored
    }

    {
        int as[2]{};
        PRINT_INFO(as, passByValue); // Param: int*, T: int*

        int const cas[2]{};
        PRINT_INFO(cas, passByValue); // Param: int const*, T: int const*

        int(*pas)[2] = nullptr;
        PRINT_INFO(pas, passByValue); // Param: int(*)[2], T: int(*)[2]

        int(* const pcas)[2] = nullptr; // Param: int(*)[2], T: int(*)[2]
        PRINT_INFO(pcas, passByValue);

        int const(*cpas)[2] = nullptr; // Param: int const(*)[2], T: int const(*)[2]
        PRINT_INFO(cpas, passByValue);

        int const(* const cpcas)[2] = nullptr; // Param: int const(*)[2], T: int const(*)[2]
        PRINT_INFO(cpcas, passByValue);

        int(&ras)[2] = as;
        PRINT_INFO(ras, passByValue); // Param: int*, T: int*

        int const(&cras)[2] = as;
        PRINT_INFO(cras, passByValue); // Param: int const*, T: int const*

        int(&& rras)[2] = std::move(as); // Param: int*, T: int*
        PRINT_INFO(rras, passByValue);

        int const(&& crras)[2] = std::move(as); // Param: int const*, T: int const*
        PRINT_INFO(crras, passByValue);

        //int(const* cpa)[2] = &a; // error: grammer...
        //int(& const rca)[2] = a; // <- warning C4220: qualifiers on reference are ignored
        //const int(& const rca)[2] = a; // <- warning C4220: qualifiers on reference are ignored
    }

    {
        PRINT_INFO(f, passByValue); // Param: void(*)(), T: void(*)()

        void (*fp)() = nullptr;
        PRINT_INFO(fp, passByValue); // Param: void(*)(), T: void(*)()

        void (* const fcp)() = nullptr;
        PRINT_INFO(fcp, passByValue); // Param: void(*)(), T: void(*)()

        void(&fr)() = f;
        PRINT_INFO(fr, passByValue); // Param: void(*)(), T: void(*)()

        void(&& frr)() = f;
        PRINT_INFO(frr, passByValue); // Param: void(*)(), T: void(*)()
    }
}

void printPassByRefInfos() {
    std::cout << "################################## Pass By Ref ##################################" << std::endl;
    std::cout << "template <typename T>" << std::endl;
    std::cout << "void f(T& param);\n" << std::endl;

    {
        int a = 10;
        PRINT_INFO(a, passByRef); // Param: int&, T: int

        int* pa = nullptr;
        PRINT_INFO(pa, passByRef); // Param: int*&, T: int*

        int& ra = a;
        PRINT_INFO(ra, passByRef); // Param: int&, T: int

        int&& rra = std::move(a);
        PRINT_INFO(rra, passByRef); // Param: int&, T: int
    }

    {
        int const ca = 10;
        PRINT_INFO(ca, passByRef); // Param: int const&, T: int const

        int const* cpa = nullptr;
        PRINT_INFO(cpa, passByRef); // Param: int const*&, T: int const*

        int const* const pca = &ca;
        PRINT_INFO(pca, passByRef); // Param: int const* const&, T: int const* const

        int const& cra = ca;
        PRINT_INFO(cra, passByRef); // Param: int const&, T: int const

        int const&& crra = 10;
        PRINT_INFO(crra, passByRef); // Param: int const&, T: int const

        // const int& const crca = a; // <- warning C4220: qualifiers on reference are ignored
        // const int&& const crrca = 10; // <- warning C4220: qualifiers on reference are ignored
    }

    {
        int as[2]{};
        PRINT_INFO(as, passByRef);   // Param: int(&)[2], T: int[2]

        int const cas[2]{};
        PRINT_INFO(cas, passByRef);  // Param: const int(&)[2], T: const int[2]

        int(*pas)[2] = nullptr;
        PRINT_INFO(pas, passByRef);  // Param: int(*&)[2], T: int(*)[2]

        int(* const pcas)[2] = nullptr;
        PRINT_INFO(pcas, passByRef); // Param: int(* const &)[2], T: int(* const)[2]

        int const(*cpas)[2] = nullptr;
        PRINT_INFO(cpas, passByRef); // Param: int const(*&)[2], T: int const(*)[2]

        int const(* const cpcas)[2] = nullptr;
        PRINT_INFO(cpcas, passByRef); // Param: int const(* const &)[2], T: int const(* const)[2]

        int(&ras)[2] = as;
        PRINT_INFO(ras, passByRef); // Param: int(&)[2], T: int[2]

        int const(&cras)[2] = as;
        PRINT_INFO(cras, passByRef); // Param: int const(&)[2], T: int const[2]

        int(&& rras)[2] = std::move(as);
        PRINT_INFO(rras, passByRef); // Param: int(&)[2], T: int[2]

        int const(&& crras)[2] = std::move(as);
        PRINT_INFO(crras, passByRef); // Param: int const(&)[2], T: int const[2]

        //int(const* cpa)[2] = &a; // error: grammer...
        //int(& const rca)[2] = a; // <- warning C4220: qualifiers on reference are ignored
        //const int(& const rca)[2] = a; // <- warning C4220: qualifiers on reference are ignored
    }

    {
        PRINT_INFO(f, passByRef); // Param: void(&)(), T: void(void)

        void (*fp)() = nullptr;
        PRINT_INFO(fp, passByRef); // Param: void(*&)(), T: void(*)()

        void (* const fcp)() = nullptr;
        PRINT_INFO(fcp, passByRef); // Param: void(* const &)(), T: void(* const)()

        void(&fr)() = f;
        PRINT_INFO(fr, passByRef); // Param: void(&)(), T: void(void)

        void(&& frr)() = f;
        PRINT_INFO(frr, passByRef); // Param: void(&)(), T: void(void)
    }
}

void printPassByURefInfos() {
    std::cout << "################################## Pass By URef ##################################" << std::endl;
    std::cout << "template <typename T>" << std::endl;
    std::cout << "void f(T&& param);\n" << std::endl;

    {
        int a = 10;
        PRINT_INFO(a, passByURef); // Param: int&, T: int&
        PRINT_INFO(std::move(a), passByURef); // Param: int&&, T: int

        int* pa = nullptr;
        PRINT_INFO(pa, passByURef); // Param: int*&, T: int*&
        PRINT_INFO(std::move(pa), passByURef); // Param: int*&&, T: int*

        int& ra = a;
        PRINT_INFO(ra, passByURef); // Param: int&, T: int&
        PRINT_INFO(std::move(ra), passByURef); // Param: int&&, T: int

        int&& rra = std::move(a);
        PRINT_INFO(rra, passByURef); // Param: int&, T: int&
        PRINT_INFO(std::move(rra), passByURef); // Param: int&&, T: int
    }

    {
        int const ca = 10;
        PRINT_INFO(ca, passByURef); // Param: int const&, T: int const&
        PRINT_INFO(std::move(ca), passByURef); // Param: int const&&, T: int const

        int const* cpa = nullptr;
        PRINT_INFO(cpa, passByURef); // Param: int const*&, T: int const*&
        PRINT_INFO(std::move(cpa), passByURef); // Param: int const*&&, T: int const*

        int const* const pca = &ca;
        PRINT_INFO(pca, passByURef); // Param: int const* const&, T: int const* const&
        PRINT_INFO(std::move(pca), passByURef); // Param: int const* const&&, T: int const* const

        int const& cra = ca;
        PRINT_INFO(cra, passByURef); // Param: int const&, T: int const&
        PRINT_INFO(std::move(cra), passByURef); // Param: int const&&, T: int const

        int const&& crra = 10;
        PRINT_INFO(crra, passByURef); // Param: int const&, T: int const&
        PRINT_INFO(std::move(crra), passByURef); // Param: int const&&, T: int const

        // const int& const crca = a; // <- warning C4220: qualifiers on reference are ignored
        // const int&& const crrca = 10; // <- warning C4220: qualifiers on reference are ignored
    }

    {
        int as[2]{};
        PRINT_INFO(as, passByURef); // Param: int(&)[2], T: int(&)[2]
        PRINT_INFO(std::move(as), passByURef); // Param: int(&&)[2], T: int[2]

        int const cas[2]{};
        PRINT_INFO(cas, passByURef); // Param: int const(&)[2], T: int const(&)[2]
        PRINT_INFO(std::move(cas), passByURef); // Param: int const(&&)[2], T: int const[2]

        int(*pas)[2] = nullptr;
        PRINT_INFO(pas, passByURef);  // Param: int (*&)[2], T: int (*&)[2]
        PRINT_INFO(std::move(pas), passByURef); // Param: int(*&&)[2], T: int(*)[2]

        int(* const pcas)[2] = nullptr;
        PRINT_INFO(pcas, passByURef); // Param: int (* const &)[2], T: int (* const &)[2]
        PRINT_INFO(std::move(pcas), passByURef); // Param: int(* const&&)[2], T: int(* const)[2]

        int const(*cpas)[2] = nullptr;
        PRINT_INFO(cpas, passByURef); // Param: int const(*&)[2], T: int const(*&)[2]
        PRINT_INFO(std::move(cpas), passByURef); // Param: int const(*&&)[2], T: int const(*)[2]

        int const(* const cpcas)[2] = nullptr;
        PRINT_INFO(cpcas, passByURef); // Param: int const(* const &)[2], T: int const(* const&)[2]
        PRINT_INFO(std::move(cpcas), passByURef); // Param: int const(* const&&)[2], T: int const(* const)[2]

        int(&ras)[2] = as;
        PRINT_INFO(ras, passByURef); // Param: int(&)[2], T: int(&)[2]
        PRINT_INFO(std::move(ras), passByURef); // Param: int(&&)[2], T: int[2]

        int const(&cras)[2] = as;
        PRINT_INFO(cras, passByURef); // Param: int const(&)[2], T: int const(&)[2]
        PRINT_INFO(std::move(cras), passByURef); // Param: int const(&&)[2], T: int const[2]

        int(&& rras)[2] = std::move(as);
        PRINT_INFO(rras, passByURef); // Param: int(&)[2], T: int(&)[2]
        PRINT_INFO(std::move(rras), passByURef); // Param: int (&&)[2], T: int[2]

        int const(&& crras)[2] = std::move(as);
        PRINT_INFO(crras, passByURef);// Param: int const(&)[2], T: int const(&)[2]
        PRINT_INFO(std::move(crras), passByURef); // Param: int const(&&)[2], T: int const[2]

        //int(const* cpa)[2] = &a; // error: grammer...
        //int(& const rca)[2] = a; // <- warning C4220: qualifiers on reference are ignored
        //const int(& const rca)[2] = a; // <- warning C4220: qualifiers on reference are ignored
    }

    {
        PRINT_INFO(f, passByURef); // Param: void(&)(void), T: void(&)(void)
        PRINT_INFO(std::move(f), passByURef); // Param void(&)(void), T: void(&)(void), MSVC is wrong

        void (*fp)() = nullptr;
        PRINT_INFO(fp, passByURef); // Param: void(*&)(void), T: void(*&)()
        PRINT_INFO(std::move(fp), passByURef); // Param: void(*&&)(void), T: void(*)(void)

        void (* const fcp)() = nullptr;
        PRINT_INFO(fcp, passByURef); // Param: void(* const&)(void), T: void(* const&)()
        PRINT_INFO(std::move(fcp), passByURef); // Param: void(* const &&)(void), T: void(* const)(void)

        void(&fr)() = f;
        PRINT_INFO(fr, passByURef); // Param: void(&)(void), T: void(&)()
        PRINT_INFO(std::move(fr), passByURef); // Param void(&)(void), T: void(&)(void), MSVC is wrong

        void(&& frr)() = f;
        PRINT_INFO(frr, passByURef); // Param: void(&)(void), T: void(&)()
        PRINT_INFO(std::move(frr), passByURef); // Param void(&)(void), T: void(&)(void), MSVC is wrong
    }
}

void printPassByPtrInfos() {
    std::cout << "################################## Pass By Pointer ##################################" << std::endl;
    std::cout << "template <typename T>" << std::endl;
    std::cout << "void f(T* param);\n" << std::endl;

    {
        int* pa = nullptr;
        PRINT_INFO(pa, passByPtr); // Param: int*, T: int
    }

    {
        int const* cpa = nullptr;
        PRINT_INFO(cpa, passByPtr); // Param: int const*, T: int const

        const int* const cpca = nullptr;
        PRINT_INFO(cpca, passByPtr); // Param: int const*, T: int const
    }

    {
        int as[2]{};
        PRINT_INFO(as, passByPtr); // Param: int*, T: int

        int const cas[2]{};
        PRINT_INFO(cas, passByPtr); // Param: int const*, T: int const

        int(*pas)[2] = nullptr;
        PRINT_INFO(pas, passByPtr); // Param: int(*)[2], T: int[2]

        int(* const pcas)[2] = nullptr;
        PRINT_INFO(pcas, passByPtr); // Param: int(*)[2], T: int[2]

        int const(*cpas)[2] = nullptr;
        PRINT_INFO(cpas, passByPtr); // Param: int const(*)[2], T: int const[2]

        int const(* const cpcas)[2] = nullptr;
        PRINT_INFO(cpcas, passByPtr); // Param: int const(*)[2], T: int const[2]

        int(&ras)[2] = as;
        PRINT_INFO(ras, passByPtr); // Param: int*, T: int

        int const(&cras)[2] = as;
        PRINT_INFO(cras, passByPtr); // Param: int const*, T: int const

        int(&& rras)[2] = std::move(as);
        PRINT_INFO(rras, passByPtr); // Param: int*, T: int

        int const(&& crras)[2] = std::move(as);
        PRINT_INFO(crras, passByPtr); // Param: int const*, T: int const

        //int(const* cpa)[2] = &a; // error: grammer...
        //int(& const rca)[2] = a; // <- warning C4220: qualifiers on reference are ignored
        //const int(& const rca)[2] = a; // <- warning C4220: qualifiers on reference are ignored
    }

    {
        PRINT_INFO(f, passByPtr); // Param: void(*)(), T: void(void)

        void (*fp)() = nullptr;
        PRINT_INFO(fp, passByPtr); // Param: void(*)(), T: void(void)

        void (* const fcp)() = nullptr;
        PRINT_INFO(fcp, passByPtr); // Param: void(*), T: void(void)

        void(&fr)() = f;
        PRINT_INFO(fr, passByPtr); // Param: void(*), T: void(void)

        void(&& frr)() = f;
        PRINT_INFO(frr, passByPtr); // Param: void(*), T: void(void)
    }
}

void printPassByCPtrInfos() {
    std::cout << "################################## Pass By Const Pointer ##################################" << std::endl;
    std::cout << "template <typename T>" << std::endl;
    std::cout << "void f(T const* param);\n" << std::endl;

    {
        int* pa = nullptr;
        PRINT_INFO(pa, passByCPtr); // Param: int const*, T: int
    }

    {
        int const* cpa = nullptr;
        PRINT_INFO(cpa, passByCPtr); // Param: int const*, T: int

        const int* const cpca = nullptr;
        PRINT_INFO(cpca, passByCPtr); // Param: int const*, T: int
    }

    {
        int as[2]{};
        PRINT_INFO(as, passByCPtr); // Param: int const*, T: int

        int const cas[2]{};
        PRINT_INFO(cas, passByCPtr); // Param: int const*, T: int

        int(*pas)[2] = nullptr;
        PRINT_INFO(pas, passByCPtr); // Param: int const(*)[2], T: int[2]

        int(* const pcas)[2] = nullptr;
        PRINT_INFO(pcas, passByCPtr); // Param: int const(*)[2], T: int[2]

        int const(*cpas)[2] = nullptr;
        PRINT_INFO(cpas, passByCPtr); // Param: int const(*)[2], T: int[2]

        int const(* const cpcas)[2] = nullptr;
        PRINT_INFO(cpcas, passByCPtr); // Param: int const(*)[2], T: int[2]

        int(&ras)[2] = as;
        PRINT_INFO(ras, passByCPtr); // Param: int const*, T: int

        int const(&cras)[2] = as;
        PRINT_INFO(cras, passByCPtr); // Param: int const*, T: int

        int(&& rras)[2] = std::move(as);
        PRINT_INFO(rras, passByCPtr); // Param: int const*, T: int

        int const(&& crras)[2] = std::move(as);
        PRINT_INFO(crras, passByCPtr); // Param: int const*, T: int

        //int(const* cpa)[2] = &a; // error: grammer...
        //int(& const rca)[2] = a; // <- warning C4220: qualifiers on reference are ignored
        //const int(& const rca)[2] = a; // <- warning C4220: qualifiers on reference are ignored
    }

    {
        // no matching function for call to 'passByCPtr'
        // could not match 'const T *' against 'void (*)()
        //PRINT_INFO(f, passByCPtr);

        //void (*fp)() = nullptr;
        //PRINT_INFO(fp, passByCPtr);

        //void (* const fcp)() = nullptr;
        //PRINT_INFO(fcp, passByCPtr);

        //void(&fr)() = f;
        //PRINT_INFO(fr, passByCPtr);

        //void(&& frr)() = f;
        //PRINT_INFO(frr, passByCPtr);
    }
}

void printPassByCRefInfos() {
    std::cout << "################################## Pass By Const Reference ##################################" << std::endl;
    std::cout << "template <typename T>" << std::endl;
    std::cout << "void f(T const& param);\n" << std::endl;

    {
        int a = 10;
        PRINT_INFO(a, passByCRef); // Param: int const&, T: int

        int* pa = nullptr;
        PRINT_INFO(pa, passByCRef); // Param: int* const&, T: int*

        int& ra = a;
        PRINT_INFO(ra, passByCRef); // Param: int const&, T: int

        int&& rra = std::move(a);
        PRINT_INFO(rra, passByCRef); // Param: int const&, T: int
    }

    {
        int const ca = 10;
        PRINT_INFO(ca, passByCRef); // Param: int const &, T: int

        int const* cpa = nullptr;
        PRINT_INFO(cpa, passByCRef); // Param: int const *const &, T: int const *

        int const* const pca = &ca;
        PRINT_INFO(pca, passByCRef); // Param: int const* const &, T: int const*

        int const& cra = ca;
        PRINT_INFO(cra, passByCRef); // Param: int const &, T: int

        int const&& crra = 10;
        PRINT_INFO(crra, passByCRef); // Param: int const &, T: int

        // const int& const crca = a; // <- warning C4220: qualifiers on reference are ignored
        // const int&& const crrca = 10; // <- warning C4220: qualifiers on reference are ignored
    }

    {
        int as[2]{};
        PRINT_INFO(as, passByCRef); // Param: int const(&)[2], T: int[2]

        int const cas[2]{};
        PRINT_INFO(cas, passByCRef); // Param: int const(&)[2], T: int [2]

        int(*pas)[2] = nullptr;
        PRINT_INFO(pas, passByCRef); // Param: int (*const &)[2], T: int(*)[2]

        int(* const pcas)[2] = nullptr;
        PRINT_INFO(pcas, passByCRef); // Param: int (*const &)[2], T: int(*)[2]

        int const(*cpas)[2] = nullptr;
        PRINT_INFO(cpas, passByCRef); // Param: int const(* const&)[2], T: int const(*)[2]

        int const(* const cpcas)[2] = nullptr;
        PRINT_INFO(cpcas, passByCRef); // Param: int const(* const&)[2], T: int const(*)[2]

        int(&ras)[2] = as;
        PRINT_INFO(ras, passByCRef); // Param: int const(&)[2], T: int[2]

        int const(&cras)[2] = as;
        PRINT_INFO(cras, passByCRef); // Param: int const(&)[2], T: int[2]

        int(&& rras)[2] = std::move(as);
        PRINT_INFO(rras, passByCRef); // Param: int const(&)[2], T: int[2]

        int const(&& crras)[2] = std::move(as);
        PRINT_INFO(crras, passByCRef); // Param: int const(&)[2], T: int[2]

        //int(const* cpa)[2] = &a; // error: grammer...
        //int(& const rca)[2] = a; // <- warning C4220: qualifiers on reference are ignored
        //const int(& const rca)[2] = a; // <- warning C4220: qualifiers on reference are ignored
    }

    {
        PRINT_INFO(f, passByCRef); // Param: void(&)(void), T: void(void)

        void (*fp)() = nullptr;
        PRINT_INFO(fp, passByCRef); // Param: void(*const&)(void), T: void(*)()

        void (* const fcp)() = nullptr;
        PRINT_INFO(fcp, passByCRef); // Param: void(*const&)(void), T: void(*)()

        void(&fr)() = f;
        PRINT_INFO(fr, passByCRef); // Param: void(&)(), T: void(void)

        void(&& frr)() = f;
        PRINT_INFO(frr, passByCRef); // Param void(&)(), T: void(void)
    }
}

void printPassByCRRefInfos() {
    std::cout << "################################## Pass By Const Rvalue Reference ##################################" << std::endl;
    std::cout << "template <typename T>" << std::endl;
    std::cout << "void f(T const&& param);\n" << std::endl;

    {
        int a = 10;
        PRINT_INFO(std::move(a), passByCRRef); // Param: int const &&, T: int

        int* pa = nullptr;
        PRINT_INFO(std::move(pa), passByCRRef); // Param: int* const&&, T: int*

        int& ra = a;
        PRINT_INFO(std::move(ra), passByCRRef); // Param: int const&&, T: int

        int&& rra = std::move(ra);
        PRINT_INFO(std::move(rra), passByCRRef); // Param: int const&&, T: int

        // Cannot bind lvalue to rvalue reference
        //PRINT_INFO(a, passByCRRef);
        //PRINT_INFO(pa, passByCRRef);
        //PRINT_INFO(ra, passByCRRef);
        //PRINT_INFO(rra, passByCRRef);

    }

    {
        int const ca = 10;
        PRINT_INFO(std::move(ca), passByCRRef); // Param: int const &&, T: int

        int const* cpa = nullptr;
        PRINT_INFO(std::move(cpa), passByCRRef); // Param: int const* const&&, T: int const*

        int const* const pca = &ca;
        PRINT_INFO(std::move(pca), passByCRRef); // Param: int const* const&&, T: int const*

        int const& cra = ca;
        PRINT_INFO(std::move(cra), passByCRRef); // Param: int const &&, T: int

        int const&& crra = 10;
        PRINT_INFO(std::move(crra), passByCRRef); // Param: int const &&, T: int

        // Cannot bind lvalue to rvalue reference
        //PRINT_INFO(ca, passByCRRef);
        //PRINT_INFO(cpa, passByCRRef);
        //PRINT_INFO(pca, passByCRRef);
        //PRINT_INFO(cra, passByCRRef);
        //PRINT_INFO(crra, passByCRRef);

        // const int& const crca = a; // <- warning C4220: qualifiers on reference are ignored
        // const int&& const crrca = 10; // <- warning C4220: qualifiers on reference are ignored
    }

    {
        int as[2]{};
        PRINT_INFO(std::move(as), passByCRRef); // Param: int const(&&)[2], T: int[2]

        int const cas[2]{};
        PRINT_INFO(std::move(cas), passByCRRef); // Param: int const(&&)[2], T: int[2]

        int(*pas)[2] = nullptr;
        PRINT_INFO(std::move(pas), passByCRRef); // Param: int(* const&&)[2], T: int(*)[2]

        int(* const pcas)[2] = nullptr;
        PRINT_INFO(std::move(pcas), passByCRRef); // Param: int(* const&&)[2], T: int(*)[2]
        
        int const(*cpas)[2] = nullptr;
        PRINT_INFO(std::move(cpas), passByCRRef); // Param: int const(* const&&)[2], T: int const(*)[2]
        
        int const(* const cpcas)[2] = nullptr;
        PRINT_INFO(std::move(cpcas), passByCRRef); // Param: int const(*const &&)[2], T: int const(*)[2]

        int(&ras)[2] = as;
        PRINT_INFO(std::move(ras), passByCRRef); // Param: int const(&&)[2], T: int[2]
        
        int const(&cras)[2] = as;
        PRINT_INFO(std::move(as), passByCRRef); // Param: int const(&&)[2], T: int const[2]

        // Cannot bind lvalue to rvalue reference
        //PRINT_INFO(as, passByCRRef);
        //PRINT_INFO(cas, passByCRRef);
        //PRINT_INFO(pas, passByCRRef);
        //PRINT_INFO(pcas, passByCRRef);
        //PRINT_INFO(cpas, passByCRRef);
        //PRINT_INFO(cpcas, passByCRRef);
        //PRINT_INFO(ras, passByCRRef);
        //PRINT_INFO(cras, passByCRRef);
        // 
        //int(const* cpa)[2] = &a; // error: grammer...
        //int(& const rca)[2] = a; // <- warning C4220: qualifiers on reference are ignored
        //const int(& const rca)[2] = a; // <- warning C4220: qualifiers on reference are ignored
    }

    {
        PRINT_INFO(f, passByCRRef); // Param: void(&&)(void), T: void(void)
        PRINT_INFO(std::move(f), passByCRRef); // Param: void(&&)(void), T: void(void)

        void (*fp)() = nullptr;
        PRINT_INFO(std::move(fp), passByCRRef); // Param: void(* const &&)(void), T: void(*)(void)

        void (* const fcp)() = nullptr;
        PRINT_INFO(std::move(fcp), passByCRRef); // Param: void(* const &&)(void), T: void(*)(void)

        void(&fr)() = f;
        PRINT_INFO(fr, passByCRRef); // Param: void(&&)(void), T: void(void)
        PRINT_INFO(std::move(fr), passByCRRef); // Param: void(&&)(void), T: void(void)

        void(&& frr)() = f;
        PRINT_INFO(frr, passByCRRef); // Param: void(&&)(void), T: void(void)
        PRINT_INFO(std::move(frr), passByCRRef); // Param: void(&&)(void), T: void(void)


        // Cannot bind lvalue to rvalue reference
        //PRINT_INFO(fp, passByCRRef);
        //PRINT_INFO(fcp, passByCRRef);
    }
}

int main() {
    printPassByValueInfos();
    printPassByRefInfos();
    printPassByURefInfos();
    printPassByPtrInfos();
    printPassByCPtrInfos();
    printPassByCRefInfos();
    printPassByCRRefInfos();
}