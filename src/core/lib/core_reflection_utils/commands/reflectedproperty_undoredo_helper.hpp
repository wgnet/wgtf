#ifndef REFLECTEDPROPERTY_UNDOREDO_HELPER_HPP
#define REFLECTEDPROPERTY_UNDOREDO_HELPER_HPP

#include "core_variant/variant.hpp"
#include "core_reflection/ref_object_id.hpp"
#include "core_reflection/reflected_method_parameters.hpp"

#include <vector>
#include <memory>
#include <string>

namespace wgt
{
class IObjectManager;
class PropertyAccessor;
class ObjectHandle;
class ISerializer;
class IDefinitionManager;
class IClassDefinition;

namespace ReflectedPropertyUndoRedoUtility
{
struct ReflectedClassMemberUndoRedoHelper
{
	virtual ~ReflectedClassMemberUndoRedoHelper()
	{
	}

	RefObjectId objectId_;
	std::string path_;

	virtual bool isMethod() const = 0;
};

struct ReflectedPropertyUndoRedoHelper : public ReflectedClassMemberUndoRedoHelper
{
	virtual ~ReflectedPropertyUndoRedoHelper()
	{
	}

	std::string typeName_;
	Variant preValue_;
	Variant postValue_;

	bool isMethod() const override
	{
		return false;
	}
};

struct ReflectedMethodUndoRedoHelper : public ReflectedClassMemberUndoRedoHelper
{
	virtual ~ReflectedMethodUndoRedoHelper()
	{
	}

	ReflectedMethodParameters parameters_;
	Variant result_;

	bool isMethod() const override
	{
		return true;
	}
};

typedef std::vector<std::unique_ptr<ReflectedClassMemberUndoRedoHelper>> UndoRedoHelperList;

const char* getUndoStreamHeaderTag();
const char* getRedoStreamHeaderTag();
const char* getPropertyHeaderTag();
const char* getMethodHeaderTag();

bool performReflectedUndo(ISerializer& serializer, IObjectManager& objectManager,
                          IDefinitionManager& definitionManager);
bool performReflectedRedo(ISerializer& serializer, IObjectManager& objectManager,
                          IDefinitionManager& definitionManager);

void saveUndoData(ISerializer& serializer, const ReflectedClassMemberUndoRedoHelper& helper);
void saveRedoData(ISerializer& serializer, const ReflectedClassMemberUndoRedoHelper& helper);

void saveUndoData(ISerializer& serializer, const ReflectedPropertyUndoRedoHelper& helper);
void saveRedoData(ISerializer& serializer, const ReflectedPropertyUndoRedoHelper& helper);

void saveUndoData(ISerializer& serializer, const ReflectedMethodUndoRedoHelper& helper);
void saveRedoData(ISerializer& serializer, const ReflectedMethodUndoRedoHelper& helper);

/**
*	Reads reflected properties from a data stream into a cache.
*	@param outPropertyCache cache to fill.
*	@param undoStream data stream from which to read.
*	@param redoStream data stream from which to read.
*	@return success.
*/
bool loadReflectedProperties(UndoRedoHelperList& outPropertyCache, ISerializer& undoSerializer,
                             ISerializer& redoSerializer, IObjectManager& objectManager,
                             IDefinitionManager& definitionManager);

/**
 *	Resolve the property path for context object by a given property path
 *  resolve strategy:
 *  suppose the property path stored in macro is a.b.c.d.e
 *  if context object has a property which path is one of
 *  the following value, we could bind it
 *  $X.a.b.c.d.e or $X.b.c.d.e or $X.c.d.e or $X.d.e or$X.e
 *  $X could be any of property path including empty string
 *	@param contextObject: a context object which try to bind property with.
 *	@param propertyPath: the property path which context object try to bind to.
 *	@return a new property path that context object can bind to.
 */
std::string resolveContextObjectPropertyPath(const ObjectHandle& contextObject, const char* propertyPath,
                                             IDefinitionManager& definitionManager);

void resolveProperty(const ObjectHandle& handle, const IClassDefinition& classDef, const char* propertyPath,
                     PropertyAccessor& o_Pa, IDefinitionManager& definitionManager);
}
} // end namespace wgt
#endif // REFLECTEDPROPERTY_UNDOREDO_HELPER_HPP
