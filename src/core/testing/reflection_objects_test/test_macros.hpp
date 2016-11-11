#ifndef REFLECTION_TEST_MACROS_HPP
#define REFLECTION_TEST_MACROS_HPP

#define GENERATE_COL_NAME(prefix, name, postFix) prefix##name##_##postFix##_

#define DEFINE_DATA_TYPE(DataType, name, prefix) \
	DataType prefix##name##_;                    \
	std::vector<DataType> GENERATE_COL_NAME(prefix, name, col_std); \
// BW::vector< DataType > GENERATE_COL_NAME( prefix, name, col_bw );

#define DEFINE_TEST_DATA_TYPES() _DEFINE_TEST_DATA_TYPES_PRE(, )

#define _DEFINE_TEST_DATA_TYPES_PRE(prefix, dummy)                                   \
	DEFINE_DATA_TYPE(bool, boolean, prefix);                                         \
	DEFINE_DATA_TYPE(int, signedInt, prefix);                                        \
	DEFINE_DATA_TYPE(uint32, unsignedInt, prefix);                                   \
	DEFINE_DATA_TYPE(int64, signedInt64, prefix);                                    \
	DEFINE_DATA_TYPE(uint64, unsignedInt64, prefix);                                 \
	DEFINE_DATA_TYPE(float, float, prefix);                                          \
	DEFINE_DATA_TYPE(double, double, prefix);                                        \
	DEFINE_DATA_TYPE(const char*, raw_string, prefix);                               \
	DEFINE_DATA_TYPE(std::string, string, prefix);                                   \
	DEFINE_DATA_TYPE(const wchar_t*, raw_wstring, prefix);                           \
	DEFINE_DATA_TYPE(std::wstring, wstring, prefix);                                 \
	DEFINE_DATA_TYPE(ObjectHandleT<TestPolyStruct>, testPolyStruct, prefix);         \
	DEFINE_DATA_TYPE(ObjectHandleT<TestEmptyStructure>, testEmptyStructure, prefix); \
	std::vector<int> GENERATE_COL_NAME(prefix, emptyCollection, col_std);

#define DEFINE_INHERITS_TEST_DATA_TYPES() _DEFINE_TEST_DATA_TYPES_PRE(Derived, )

#define _INIT_DATA_PRE(prefix, dummy)                      \
	: prefix##boolean_( false )\
	, prefix##signedInt_( -42 )\
	, prefix##unsignedInt_( 42 )\
	, prefix##signedInt64_( ( int64 ) INT_MIN - 1 )\
	, prefix##unsignedInt64_( ( uint64 ) UINT_MAX + 1 )\
	, prefix##float_( 22.0f / 7 )\
	, prefix##double_( 22.0 / 7 )\
	, prefix##raw_string_( "Test string" )\
	, prefix##string_( "Test BW::string" )\
	, prefix##raw_wstring_( L"Test wide string 你好吗？" )\
	, prefix##wstring_( L"Test BW::wstring 还好啦" )

#define INIT_COLLECTION_DATA(definitionManager, DataType, name, prefix) \
	TestObjectCommon::populateData<DataType>(definitionManager, GENERATE_COL_NAME(prefix, name, col_std)); \
//TestObjectCommon::populateData< DataType >(\
	//	definitionManager, GENERATE_COL_NAME( prefix, name, col_bw ) );

#define INIT_COL_PRE(definitionManager) INIT_COL_PRE_(definitionManager, )

#define INIT_COL_PRE_DERIVED(definitionManager) INIT_COL_PRE_(definitionManager, Derived)

#define INIT_COL_PRE_(definitionManger, prefix)                                  \
	INIT_COLLECTION_DATA(definitionManger, bool, boolean, prefix);               \
	INIT_COLLECTION_DATA(definitionManger, int, signedInt, prefix);              \
	INIT_COLLECTION_DATA(definitionManger, uint32, unsignedInt, prefix);         \
	INIT_COLLECTION_DATA(definitionManger, int64, signedInt64, prefix);          \
	INIT_COLLECTION_DATA(definitionManger, uint64, unsignedInt64, prefix);       \
	INIT_COLLECTION_DATA(definitionManger, float, float, prefix);                \
	INIT_COLLECTION_DATA(definitionManger, double, double, prefix);              \
	INIT_COLLECTION_DATA(definitionManger, const char*, raw_string, prefix);     \
	INIT_COLLECTION_DATA(definitionManger, std::string, string, prefix);         \
	INIT_COLLECTION_DATA(definitionManger, const wchar_t*, raw_wstring, prefix); \
	INIT_COLLECTION_DATA(definitionManger, std::wstring, wstring, prefix);       \
	INIT_COLLECTION_DATA(definitionManger, ObjectHandleT<TestPolyStruct>, testPolyStruct, prefix);

#define INIT_DATA_INHERITS() _INIT_DATA_PRE(Derived, )

#define INIT_DATA() _INIT_DATA_PRE(, )

#define DEFINE_TEST_DATA_TYPES_METADATA()                                                              \
	_DEFINE_TEST_DATA_TYPES_METADATA(, , TestObjectCommon::s_MemberVariableGroup)                      \
	_DEFINE_TEST_DATA_TYPES_METADATA(, _col_std, TestObjectCommon::s_MemberStdCollectionVariableGroup) \
	EXPOSE("Empty Collection", emptyCollection_col_std_,                                               \
	       MetaGroup(TestObjectCommon::s_MemberStdCollectionVariableGroup)) \
//_DEFINE_TEST_DATA_TYPES_METADATA( , _col_bw, s_MemberBWCollectionVariableGroup )

#define DEFINE_TEST_INHERITED_DATA_TYPES_METADATA()                                      \
	_DEFINE_TEST_DATA_TYPES_METADATA(Derived, , TestObjectCommon::s_MemberVariableGroup) \
	_DEFINE_TEST_DATA_TYPES_METADATA(Derived, _col_std, TestObjectCommon::s_MemberStdCollectionVariableGroup) \
//_DEFINE_TEST_DATA_TYPES_METADATA( Derived, _col_bw, s_MemberBWCollectionVariableGroup )

#define _DEFINE_TEST_DATA_TYPES_METADATA(prefix, postFix, groupName)                           \
	\
EXPOSE(#prefix "Bool" #postFix, prefix##boolean##postFix##_, MetaGroup(groupName))             \
	\
EXPOSE(#prefix "Signed int" #postFix, prefix##signedInt##postFix##_, MetaGroup(groupName))     \
	\
EXPOSE(#prefix "Unsigned int" #postFix, prefix##unsignedInt##postFix##_, MetaGroup(groupName)) \
	\
EXPOSE(#prefix "Signed int64" #postFix, prefix##signedInt64##postFix##_, MetaGroup(groupName)) \
	\
EXPOSE(#prefix "Unsigned int64" #postFix, prefix##unsignedInt64##postFix##_, MetaGroup(groupName)) \
EXPOSE(#prefix "float" #postFix, prefix##float##postFix##_,                                    \
	   MetaGroup(groupName) + MetaDecimals(5) + MetaStepSize(0.00001)) \
EXPOSE(#prefix "double" #postFix, prefix##double##postFix##_,                                  \
	   MetaGroup(groupName) + MetaDecimals(5) + MetaStepSize(0.00001)) \
EXPOSE(#prefix "const char *" #postFix, prefix##raw_string##postFix##_, MetaGroup(groupName)) \
EXPOSE(#prefix "BW::string" #postFix, prefix##string##postFix##_, MetaGroup(groupName)) \
EXPOSE(#prefix "const wchar_t *" #postFix, prefix##raw_wstring##postFix##_, MetaGroup(groupName)) \
EXPOSE(#prefix "BW::wstring" #postFix, prefix##wstring##postFix##_, MetaGroup(groupName)) \
EXPOSE(#prefix "TestPolyStruct" #postFix, prefix##testPolyStruct##postFix##_, MetaGroup(groupName)) \
EXPOSE(#prefix "TestEmptyStructure" #postFix, prefix##testEmptyStructure##postFix##_, MetaGroup(groupName))

#endif // REFLECTION_TEST_MACROS_HPP
