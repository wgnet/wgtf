#ifndef I_ENUM_GENERATOR_HPP
#define I_ENUM_GENERATOR_HPP

namespace wgt
{
class ObjectHandle;
class Collection;

class IEnumGenerator
{
public:
	virtual ~IEnumGenerator() {}
	virtual Collection getCollection( const ObjectHandle & provider, const IDefinitionManager & definitionManager ) = 0;
};

typedef std::unique_ptr<IEnumGenerator> IEnumGeneratorPtr;
} // end namespace wgt
#endif //I_ENUM_GENERATOR_HPP
