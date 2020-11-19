#include <iostream>
#include <vector>
#include <unordered_map>

class A
{

};

class BA
{

};

class BA1 : public BA
{
    virtual void goo() {};
};

class BA2 : public BA
{

};

class BA11 : public BA1
{

};

class BB
{

};

class BB1 : public BB
{
    virtual void foo() {};
};

class BB2 : public BB
{

};

class BB21 : public BB2
{

};


template <typename T>
struct Holder
{
    T _obj;
    size_t sz = sizeof(T);
};

template <typename T, typename Base>
struct DerHolder :
    public Base
{
    T _obj;
};




class Nulltype {};

template <typename ...Args>
struct TypeList
{
    using Head = Nulltype;
    using Tail = Nulltype;
};

template <typename H, typename ...T>
struct TypeList<H, T...>
{
    using Head = H;
    using Tail = TypeList<T...>;
};

typedef TypeList<> EmptyTypeList;

template <typename H>
struct TypeList<H>
{
    using Head = H;
    using Tail = EmptyTypeList;
};



//...................................................................//


template <typename T>
struct Length
{
    static const int value = 1 + Length<T::Tail>::value;
};

template <>
struct Length<Nulltype>
{
    static const int value = 0;
};

//...................................................................//

template <size_t ind, typename ...T>
struct TypeAt {
    using res = Nulltype;
};

template <size_t ind, typename ...T>
struct TypeAt<ind, TypeList<T...>>
{
    using res = typename TypeAt<ind - 1, typename TypeList<T...>::Tail>::res;
};

template <size_t ind>
struct TypeAt<ind, Nulltype>
{
    using res = Nulltype;
};

template <typename ...T>
struct TypeAt<0, TypeList<T...>>
{
    using res = typename TypeList<T...>::Head;
};

//...................................................................//

template <typename U, typename ...T>
struct IndexOf
{
    static const size_t value = 1;
};

template <typename ...K, typename U>
struct IndexOf<U, TypeList<K...>>
{
    static const size_t value = 1 + IndexOf<U, typename TypeList<K...>::Tail>::value;
};

template <typename T, typename ...K>
struct IndexOf<TypeList<T, K...>, T>
{
    static const size_t value = 0;
};

//...................................................................//

template < typename U, size_t ind, typename ...T>
struct Add;


template < typename U, size_t ind, typename ...K>
struct Add<U, ind, TypeList<K...> >
{
    using result = TypeList<typename TypeList<K...>::Head, typename Add< U, ind - 1, typename TypeList<K...>::Tail>::result>;
};

template <typename U, typename T>
struct Add<U, 0, T>
{
    using result = TypeList <U, T>;
};

template <typename U, size_t ind>
struct Add<U, ind, TypeList<Nulltype>>
{
    using result = U;
};

//...................................................................//

template <typename U, typename ...T>
struct Remove;

template <typename U, typename ...T>
struct Remove<U, TypeList<T...>>
{
    using result = TypeList<typename TypeList<T...>::Head, typename Remove<U, typename TypeList<T...>::Tail>::result>;
};

template <typename T, typename ...K>
struct Remove<T, TypeList<T, K...>>
{
    using result = typename Remove<T, typename TypeList<T, K...>::Tail>::result;
};

template <typename T>
struct Remove< T, EmptyTypeList>
{
    using result = Nulltype;
};

//...................................................................//

template <class T, class U, class V> struct Replace;

//template<class U, class V>
//struct Replace<Nulltype, U, V>
//{
//    using res = Nulltype;
//};

template <class U, class V>
struct Replace<TypeList<U>, U, V>
{
    using res = TypeList<V>;
};

template <class T, class U, class V>
struct Replace<TypeList<T>, U, V>
{
    using res = TypeList<T>;
};

template <class ...T, class U, class V>
struct Replace<TypeList<U, T...>, U, V>
{
    using res = TypeList<V, T...>;
};

template <class ...T, class U, class V>
struct Replace<TypeList<T...>, U, V>
{
    using res = TypeList< typename TypeList<T...>::Head,
        typename Replace<typename TypeList<T...>::Tail, U, V>::res >;
};



//...................................................................//

template<bool is, typename T, typename U> struct Select;

template<typename T, typename U>
struct Select<true, T, U>
{
    using res = T;
};

template<typename T, typename U>
struct Select<false, T, U>
{
    using res = U;
};

//...................................................................//

template <typename T, typename U>
class Conversion
{
    using Small = char;
    struct Big { char x[2]; };
    static Small Test(const U&);
    static Big Test(...);
    static T makeT();

public:
    static constexpr bool exists = sizeof(Small) == sizeof(Test(makeT()));
    static constexpr bool equal = false;
};

template <typename T>
class Conversion<T, T>
{
public:
    static constexpr bool exists = true;
    static constexpr bool equal = true;
};

template <bool one, bool two>
struct ANDNOT
{
    static constexpr bool res = false;
};

template <>
struct ANDNOT<true, false>
{
    static constexpr bool res = true;
};

template <typename T, typename U>
struct isBase
{
    static constexpr bool res = ANDNOT< Conversion<T, U>::exists, Conversion<T, U>::equal >::res;
};



//...................................................................//

template <class TList, class T>
struct MostDerived;

template <class T>
struct MostDerived<Nulltype, T>
{
    using res = T;
};

template <class ...TList, class T>
struct MostDerived<TypeList<TList...>, T>
{
    using Cand = typename MostDerived<typename TypeList<TList...>::Tail, T>::res;

    using res = typename Select<isBase<typename TypeList<TList...>::Head, Cand>::res,
        typename TypeList<TList...>::Head, Cand>::res;
};

template<class T> struct DerivedToFront
{
    using res = Nulltype;
};

template<>
struct DerivedToFront<TypeList<Nulltype>>
{
    using res = Nulltype;
};

template <typename H>
struct DerivedToFront<TypeList<H>>
{
    using res = H;
};

template <typename ...TList>
struct DerivedToFront<TypeList<TList...>>
{
private:

    using TheMostDerived = typename MostDerived<typename TypeList<TList...>::Tail, typename TypeList<TList...>::Head>::res;

    using Temp = typename Replace< typename TypeList<TList...>::Tail, TheMostDerived, typename TypeList<TList...>::Head >::res;

    using L = typename DerivedToFront<Temp>::res;

public:
    using res = TypeList<TheMostDerived, L>;

};

//...................................................................//

template <class TList, template <class> class Unit>
struct GenScatterHierarchy;

template <class ...TList, template <class> class Unit>
struct GenScatterHierarchy<TypeList<TList...>, Unit>:
    public Unit<typename TypeList<TList...>::Head>,
    public GenScatterHierarchy<typename TypeList<TList...>::Tail, Unit>    
{
    using TL = TypeList<TList...>;
    using LeftBase = Unit<typename TypeList<TList...>::Head>;
    using RightBase = GenScatterHierarchy<typename TypeList<TList...>::Tail, Unit>;
    template <typename T> struct Rebind
    {
        typedef Unit<T> Result;
    };
};

template <class T, template <class> class Unit>
struct GenScatterHierarchy<TypeList<T>, Unit> :
    public Unit<T>
{
    using LeftBase = T;
    using RightBase = Nulltype;
    template <typename T> struct Rebind
    {
        typedef Unit<T> Result;
    };
};

template <template <class> class Unit>
struct GenScatterHierarchy<TypeList<Nulltype>, Unit>
{
    using LeftBase = Nulltype;
    using RightBase = Nulltype;
    template <typename T> struct Rebind
    {
        typedef Unit<T> Result;
    };
};

//...................................................................//
template<class TList, template <class T, class Base> class Unit, class Root = Nulltype>
struct GenLinearHierarchy;

template<class ...TList, template <class T, class Base> class Unit, class Root>
struct GenLinearHierarchy<TypeList<TList...>, Unit, Root> :
    public Unit< typename TypeList<TList...>::Head, GenLinearHierarchy<typename TypeList<TList...>::Tail, Unit, Root>>
{

};

template<class TList, template <class T, class Base> class Unit, class Root>
struct GenLinearHierarchy<TypeList<TList>, Unit, Root> :
    public Unit<TList, Root>
{

};

template <class H, unsigned int i> struct FieldHelper;

template <class H>
struct FieldHelper<H, 0>
{
    typedef typename H::TL::Head ElementType;
    typedef typename H::template Rebind<ElementType>::Result UnitType;

    static UnitType& Do(H& obj)
    {
        UnitType& leftBase = obj;
        return leftBase;
    }
};

template <class H, unsigned int i>
struct FieldHelper
{
    typedef typename TypeAt<i, typename H::TList>::Result ElementType;
    typedef typename H::template Rebind<ElementType>::Result UnitType;

    typedef typename H::RightBase RightBase;

    static UnitType& Do(H& obj)
    {
        RightBase& rightBase = obj;
        return FieldHelper<RightBase, i - 1>::Do(rightBase);
    }
};

template <int i, class H>
typename FieldHelper<H, i>::UnitType&
Field(H& obj)
{
    return FieldHelper<H, i>::Do(obj);
}

template <class T, T v>
struct integral_constant
{
    static constexpr T value = v;
    using value_type = T;
    
    constexpr operator value_type() const noexcept { return value; }
    constexpr value_type operator()() const noexcept { return value; }
};

using true_type = integral_constant<bool, true>;
using false_type = integral_constant<bool, false>;

template <class Cond, class T, class U>
struct conditional { using type = T; };

template <class T, class U>
struct conditional<false_type, T, U>
{
    using type = U;
};

template <class Cond, class T, class U>
using conditional_t = typename conditional<Cond, T, U>::type;



template <class T, class U>
struct AND
{
    using res = true_type;
};

template <class U>
struct AND< false_type, U>
{
    using res = false_type;
};

template <class T>
struct AND<T, false_type>
{
    using res = false_type;
};



template <class H, class ...T>
struct conjunction
{
    using res = typename conditional_t < H, typename conjunction<T...>::res, false_type>;
};

template <class T>
struct conjunction<T>
{
    using res = T;
};

template < class T> struct remove_reference { using type = T; };
template < class T> struct remove_reference<T&> { using type = T; };
template < class T> struct remove_reference<T&&> { using type = T; };

template < class T>
using remove_reference_t = typename remove_reference<T>::type;

template <class H, class ...T>
struct disjunction
{
    using res = typename conditional_t< H, true_type, typename disjunction<T...>::res>;
};

template <class T>
struct disjunction<T>
{
    using res = T;
};

template <class T, class U>
struct is_same
{
    using res = false_type;
};

template <class T>
struct is_same<T,T>
{
    using res = true_type;
};

template < class T, class U>
using is_same_t = typename is_same<T, U>::res;


template <class T>
struct is_float
    : disjunction<is_same_t<T, float>, is_same_t<T, double>, is_same_t<T, long double>>::res
{};

template < class T>
constexpr bool is_float_v = is_float<T>::value;

template <class T>
std::true_type detect_poly(decltype(dynamic_cast<const volatile void*>(static_cast<T*>(nullptr))));

template <class T>
std::false_type detect_poly(...);

template <class T>
struct is_poly : decltype(detect_poly<T>(nullptr)){};

template < class T>
constexpr bool is_poly_t = is_poly<T>::value;

int main()
{
    TypeList<int, int, float, A> a;

    const int v = Length< TypeList<int, int, float, A, A>>::value;

    TypeAt<2, TypeList<int, int, float, A>>::res c = 1.f;

    size_t i = IndexOf<TypeList<int, float, double>, double>::value;

    Add<char, 1, TypeList<int, float, double>>::result d;

    Remove<char, TypeList<int, char, float, char, double>>::result e;

    Replace<TypeList<int, float, double>, double, char>::res rp;

    MostDerived<TypeList<BB, BA1, BB21, BA2, BA, BB2, BB1>, BB>::res md;

    //Replace<TypeList<BB, BA1, BB21, BA2, BA, BB2, BB1>, MostDerived<TypeList<BB, BA1, BB21, BA2, BA, BB2, BB1, BA>, BA>::res, BB>::res;

    constexpr bool isb = isBase<BA1, BA>::res;
    constexpr bool isc = Conversion<BA1, BA>::exists;

    std::cout << Conversion<BA1, BA>::exists << std::endl;

    DerivedToFront<TypeList<BB, BA1, BB21, BA2, BA, BB2, BB1>>::res dtf;

    GenScatterHierarchy<TypeList<int, float, double>, Holder> sch;

    GenLinearHierarchy<TypeList<int, float, double>, DerHolder> lh;

    (static_cast<Holder<float>&>(sch))._obj = 5.1;
    (static_cast<Holder<int>&>(sch))._obj = 13;


    float fgs = (static_cast<Holder<float>&>(sch))._obj;

    int igs = Field<0>(sch)._obj;
    std::cout << fgs << " " << igs << std::endl;

    integral_constant<int, 4> ic4;

    conditional_t<false_type, A, BA> cond; 

    AND<true_type, true_type>::res tr;
    AND<false_type, true_type>::res fs;

    disjunction<false_type, false_type, false_type, false_type>::res cj;

    bool isf = is_float<float>::value;
    std::cout << is_float_v<float> << " " << is_float_v<int> << " " << is_float_v<BA> << std::endl;

    std::cout << is_poly_t<A> << " " << is_poly_t<BA1> << " " << is_poly_t<BB1> << " " << is_poly_t<BB> << std::endl;

}