#include <iostream>


class Class0 {};
class Class1 {};
class Class2 {};
class Class3 {};
class Class4 {};
class Class5 {};
class Class6 {};
class Class7 {};
class Class8 {};
class Class9 {};
class Class10 {};
class Class11 {};
class Class12 {};
class Class13 {};
class Class14 {};
class Class15 {};
class Class16 {};
class Class17 {};
class Class18 {};
class Class19 {};
class Class20 {};
class Class21 {};
class Class22 {};
class Class23 {};
class Class24 {};



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


template <size_t level>
struct LevelWidth
{
    static const size_t width = 3 * LevelWidth<level - 2>::width;
};

template <>
struct LevelWidth<1>
{
    static const size_t width = 3;
};

template <>
struct LevelWidth<0>
{
    static const size_t width = 1;
};

template <size_t level>
struct LevelIndexes
{
    static const size_t first_index = LevelIndexes<level - 1>::last_index + 1;
    static const size_t last_index = LevelIndexes<level>::first_index + LevelWidth<level>::width - 1;
};


template <>
struct LevelIndexes<1>
{
    static const size_t first_index = 1;
    static const size_t last_index = 3;
};


template <size_t level, size_t index >
struct IndexOfSingleStep
{
    static const size_t res = LevelIndexes<level + 1>::first_index + (index - LevelIndexes<level>::first_index);
};

template <size_t level, size_t index>
struct IndexOfTripleStep
{
    static const size_t res = 3 * (index - LevelIndexes<level>::first_index) + LevelIndexes<level + 1>::first_index;
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

template <typename Type, class TypeList, int index, int level, template <typename> typename Unit>
struct SingleLevel;

template <class TypeList, int index, int level, template <typename> typename Unit>
struct SingleLevel<Nulltype, TypeList, level, index, Unit>
{
};

template <typename Type, class TypeList, int index, int level, template <typename> typename Unit>
struct TripleLevel;

template <class TypeList, int index, int level, template <typename> typename Unit>
struct TripleLevel<Nulltype, TypeList, level, index, Unit>
{
};

template <typename Type, class TypeList, int index, int level, template <typename> typename Unit>
struct SingleLevel : public TripleLevel<typename TypeAt<IndexOfTripleStep<level, index>::res, TypeList>::res, TypeList, IndexOfTripleStep<level, index>::res, level + 1, Unit>,
    public TripleLevel<typename TypeAt<IndexOfTripleStep<level, index>::res + 1, TypeList>::res, TypeList, IndexOfTripleStep<level, index>::res + 1, level + 1, Unit>,
    public TripleLevel<typename TypeAt<IndexOfTripleStep<level, index>::res + 2, TypeList>::res, TypeList, IndexOfTripleStep<level, index>::res + 2, level + 1, Unit>
{
    Unit<Type> unit;
};

template <typename Type, class TypeList, int index, int level, template <typename> typename Unit>
struct TripleLevel : public SingleLevel<typename TypeAt<IndexOfSingleStep<level, index>::res, TypeList>::res, TypeList, IndexOfSingleStep<level, index>::res, level + 1, Unit>
{
    Unit<Type> unit;
};

template <class TypeList, template <typename> typename Unit>
struct GenCustomHierarchy : public TripleLevel<typename TypeAt<1, TypeList>::res, TypeList, 1, 1, Unit>,
    public TripleLevel<typename TypeAt<2, TypeList>::res, TypeList, 2, 1, Unit>,
    public TripleLevel<typename TypeAt<3, TypeList>::res, TypeList, 3, 1, Unit>
{
    Unit<typename TypeAt<0, TypeList>::res> unit;
};