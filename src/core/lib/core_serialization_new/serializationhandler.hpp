#pragma once
#include "serializationnode.hpp"
#include "core_variant/variant.hpp"

#include <memory>

namespace wgt
{
class SerializationNode;

class SerializationHandler
{
	friend class SerializationHandlerManager;

public:
	typedef std::unique_ptr<SerializationNode> NodePtr;

	struct ReservedNames
	{
		const char* variantInternalType = "__TFSVariantType";
		const char* reflectedInternalType = "__TFSReflectedType";
		const char* collectionContainerType = "__TFSColContainerType";
		const char* collectionKeyVariantType = "__TFSColKeyVariantType";
		const char* collectionValueVariantType = "__TFSColValueVariantType";
		const char* collectionKeyInternalType = "__TFSColKeyInternalType";
		const char* collectionValueInternalType = "__TFSColValueInternalType";
		const char* collectionKeyHandler = "__TFSColKeyHandler";
		const char* collectionValueHandler = "__TFSColValueHandler";
	};

public:
	SerializationHandler() = delete;
	SerializationHandler(const char* handlerName);
	virtual ~SerializationHandler()
	{
	}

	virtual bool write(const Variant& v, const NodePtr& node, bool writeType = true) = 0;
	virtual bool read(Variant& v, const NodePtr& node, const char* typeName = nullptr) = 0;

	const char* getName() const;

	virtual const char* getInternalTypeOf(const Variant& v, const NodePtr& node) const = 0;

	const ReservedNames& getReservedNames() const;

protected:
	/**Allows the handler to test if it can handle the given variant type. The name of the handler to be used can be
	specified to speed up checks, as well as testing the variant's type. This cannot be completely relied upon, however,
	 as the names could be outdated or even missing in some cases.*/
	bool canHandleWrite(const Variant& v, const char* handlerName = nullptr, bool writeType = true);
	bool canHandleRead(const NodePtr& node, const char* handlerName = nullptr, const char* typeName = nullptr);
	std::string handlerName_;

private:
	virtual bool canHandleWriteInternal(const Variant& v, bool writeType = true) = 0;
	virtual bool canHandleReadInternal(const NodePtr& node, const char* typeName = nullptr) = 0;
	const ReservedNames reservedNames_;
};
}