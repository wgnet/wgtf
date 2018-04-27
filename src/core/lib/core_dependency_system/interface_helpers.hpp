#ifndef INTERFACE_HELPERS_HPP
#define INTERFACE_HELPERS_HPP

// Helpers to add whitespace to macros
#define MACRO_SPACE
#define ADD_SPACE(x) x MACRO_SPACE MACRO_SPACE

#define MERGE(Left, ...) MERGE_(Left, __VA_ARGS__)
#define MERGE_(Left, ...) Left##__VA_ARGS__

#define GENERATE_CLASSNAME(ClassName, MinorVersion) \
	: public Implements< ClassName##_v##MinorVersion >

// Helper to decrement versions
#define DECREMENT_VERSION(ClassName, x) \
	MERGE_(DECREMENT_, x)(ClassName)    \
	{                                   \
	public:

#define DECREMENT_0(ClassName)
#define DECREMENT_1(ClassName) GENERATE_CLASSNAME(ClassName, 0)
#define DECREMENT_2(ClassName) GENERATE_CLASSNAME(ClassName, 1)
#define DECREMENT_3(ClassName) GENERATE_CLASSNAME(ClassName, 2)
#define DECREMENT_4(ClassName) GENERATE_CLASSNAME(ClassName, 3)
#define DECREMENT_5(ClassName) GENERATE_CLASSNAME(ClassName, 4)
#define DECREMENT_6(ClassName) GENERATE_CLASSNAME(ClassName, 5)
#define DECREMENT_7(ClassName) GENERATE_CLASSNAME(ClassName, 6)
#define DECREMENT_8(ClassName) GENERATE_CLASSNAME(ClassName, 7)
#define DECREMENT_9(ClassName) GENERATE_CLASSNAME(ClassName, 8)
#define DECREMENT_10(ClassName) GENERATE_CLASSNAME(ClassName, 9)
#define DECREMENT_11(ClassName) GENERATE_CLASSNAME(ClassName, 10)
#define DECREMENT_12(ClassName) GENERATE_CLASSNAME(ClassName, 11)
#define DECREMENT_13(ClassName) GENERATE_CLASSNAME(ClassName, 12)
#define DECREMENT_14(ClassName) GENERATE_CLASSNAME(ClassName, 13)
#define DECREMENT_15(ClassName) GENERATE_CLASSNAME(ClassName, 14)
#define DECREMENT_16(ClassName) GENERATE_CLASSNAME(ClassName, 15)
#define DECREMENT_17(ClassName) GENERATE_CLASSNAME(ClassName, 16)
#define DECREMENT_18(ClassName) GENERATE_CLASSNAME(ClassName, 17)
#define DECREMENT_19(ClassName) GENERATE_CLASSNAME(ClassName, 18)
#define DECREMENT_20(ClassName) GENERATE_CLASSNAME(ClassName, 19)

// Helper to declare interfaces according to convention
#define DECLARE_INTERFACE_BEGIN(ClassName, Major, Minor) \
	namespace major_v##Major                             \
	{                                                    \
		class ClassName##_v##Minor DECREMENT_VERSION(ClassName, Minor)

#define DECLARE_INTERFACE_END() \
	}                           \
	;                           \
	}

#define INTERFACE_VERSION(ClassName, MajorVersion, MinorVersion) major_v##MajorVersion::ClassName##_v##MinorVersion

#define DECLARE_LATEST_VERSION(ClassName, MajorVersion, MinorVersion) \
	typedef ADD_SPACE(INTERFACE_VERSION(ClassName, MajorVersion, MinorVersion)) ClassName;

#endif // INTERFACE_HELPERS_HPP
