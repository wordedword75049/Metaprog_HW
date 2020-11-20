#include "GenCustomHierarchy.h"

int main()
{
    GenCustomHierarchy<TypeList<
        Class1, Class2, Class3, Class4, Class5,
        Class6, Class7, Class8, Class9, Class10,
        Class11, Class12, Class13, Class14, Class15,
        Class16, Class17, Class18, Class19, Class20,
        Class21, Class22, Class23, Class24>,
        Holder> MyCustomHierarchy;
}