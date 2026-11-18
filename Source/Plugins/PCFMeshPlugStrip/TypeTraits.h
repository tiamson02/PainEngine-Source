// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/04 Bart Sekura
//----------------------------------------------------------------------------


#ifdef _MSC_VER
#endif

#if SUPPORTS_PARTIAL_TEMPLATE_SPECIALIZATION

template <class T>
struct IsPtr
{
	enum { Value=0 };
};

template <class U>
struct IsPtr<U*>
{
	enum { Value=1 };
};

#else

// a workaround lifted from boost library
// exploits the coolness of sizeof() operator
// here, sizeof() is applied to a static function and we
// take advantage of overloading to compile time determine if a type is ptr or not
template <typename T>
struct IsPtr
{
private:
    static T t;

    typedef char    T_Yes;
    typedef double  T_No;

    struct PointerHelper
    {
        PointerHelper(const volatile void*);
    };
    static T_Yes   IsPointerHelper(PointerHelper);
    static T_No    IsPointerHelper(...);

public:
    enum { Value=(1==sizeof(IsPointerHelper(t))) };
};

#endif

template <> struct IsPtr<void> { enum { Value=false }; };

template <typename T>
struct IsPrimitive
{
    enum { Value=IsPtr<T>::Value };
};

template <> struct IsPrimitive<char> 	{ enum { Value=true }; };
template <> struct IsPrimitive<short> 	{ enum { Value=true }; };
template <> struct IsPrimitive<int> 	{ enum { Value=true }; };
template <> struct IsPrimitive<long> 	{ enum { Value=true }; };
template <> struct IsPrimitive<float> 	{ enum { Value=true }; };
template <> struct IsPrimitive<double> 	{ enum { Value=true }; };

// by default, type is considered complex, unless it's primitive
template <typename T>
struct ComplexType
{
    enum { Value=!IsPrimitive<T>::Value };
};

/*
// specialization for primitive types
template <> struct ComplexType<char>			{ enum { Value=false }; };
template <> struct ComplexType<unsigned char>	{ enum { Value=false }; };
template <> struct ComplexType<short>			{ enum { Value=false }; };
template <> struct ComplexType<unsigned short>	{ enum { Value=false }; };
template <> struct ComplexType<int>				{ enum { Value=false }; };
template <> struct ComplexType<unsigned int>	{ enum { Value=false }; };
template <> struct ComplexType<long>			{ enum { Value=false }; };
template <> struct ComplexType<unsigned long>	{ enum { Value=false }; };
template <> struct ComplexType<float>			{ enum { Value=false }; };
template <> struct ComplexType<double>			{ enum { Value=false }; };
template <> struct ComplexType<long double>		{ enum { Value=false }; };
*/

template <int v>
struct Int2Type
{
	enum { Value = v };
};

typedef Int2Type<true>  TrueType;
typedef Int2Type<false> FalseType;
