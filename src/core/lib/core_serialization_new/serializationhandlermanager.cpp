#include "serializationhandlermanager.hpp"
#include "serializationhandler.hpp"
#include "serializationhandlers/variantstreamhandler.hpp"
#include "serializationhandlers/reflectedhandler.hpp"
#include "serializationhandlers/primitivehandlers/primitiveinthandler.hpp"
#include "serializationhandlers/primitivehandlers/primitiveuinthandler.hpp"
#include "serializationhandlers/primitivehandlers/primitivedoublehandler.hpp"
#include "serializationhandlers/primitivehandlers/primitivestringhandler.hpp"
#include "serializationhandlers/collectionhandler.hpp"

namespace wgt
{
SerializationHandlerManager::SerializationHandlerManager(const IDefinitionManager& definitionManager)
    : handlers_(), definitionManager_(definitionManager)
{
	// Create internal handlers
	variantHandler_ = std::make_shared<VariantStreamHandler>();
	primitiveIntHandler_ = std::make_shared<PrimitiveIntHandler>();
	primitiveUintHandler_ = std::make_shared<PrimitiveUintHandler>();
	primitiveDoubleHandler_ = std::make_shared<PrimitiveDoubleHandler>();
	primitiveStringHandler_ = std::make_shared<PrimitiveStringHandler>();
	reflectedHandler_ = std::make_shared<ReflectedHandler>(definitionManager_);
	collectionHandler_ = std::make_shared<CollectionHandler>(*this);

	// Register internal handlers
	// VariantStream should be last
	// Collection should be after primitive
	handlers_.push_back(reflectedHandler_);
	handlers_.push_back(primitiveIntHandler_);
	handlers_.push_back(primitiveUintHandler_);
	handlers_.push_back(primitiveDoubleHandler_);
	handlers_.push_back(primitiveStringHandler_);
	handlers_.push_back(collectionHandler_);
	handlers_.push_back(variantHandler_);
}

SerializationHandlerManager::~SerializationHandlerManager()
{
	// Delete all handler shared_ptrs
	for (auto h : handlers_)
	{
		h.reset();
	}
	handlers_.clear();
}

void SerializationHandlerManager::registerHandler(std::shared_ptr<SerializationHandler> handler, int64_t priority)
{
	if (handlers_.empty())
	{
		handlers_.push_back(handler);
		return;
	}

	// Place the handler at index @priority in the list, unless it reaches the end, in which case add it to the end.
	// The internal handler types should always come last (reflected, primitive, collection, and variantstream).
	// If priority isn't specified, it will just go to the end.
	// NOTE: Priority might not actually be too useful in reality, as users won't know how many handlers will be
	// registered in total at compile time, and must either go with 0, default, or guess/write some code to choose
	// how far into the list to place it. Users could just use -1 or 0, and we can't make them use it responsibly.
	// It doesn't really do any harm, but it might not be worth keeping around.
	int64_t count = 0;
	for (auto currentHandler = handlers_.begin();; ++currentHandler)
	{
		// ReflectedHandler should always be second-last
		if (*currentHandler == reflectedHandler_ || count == priority)
		{
			// Emplace it before currentHandler
			handlers_.emplace(currentHandler, handler);
			return;
		}
		++count;
	}
}

void SerializationHandlerManager::deregisterHandler(std::shared_ptr<SerializationHandler> handler)
{
	for (auto h = handlers_.begin(); h != handlers_.end(); ++h)
	{
		if (*h == handler)
		{
			(*h).reset();
			handlers_.erase(h);
			return;
		}
	}
}

size_t SerializationHandlerManager::getHandlerCount() const
{
	return handlers_.size();
}

std::shared_ptr<SerializationHandler> SerializationHandlerManager::findHandlerWrite(const Variant& v, const char* name,
                                                                                    bool writeType) const
{
	for (auto h : handlers_)
	{
		if (h->canHandleWrite(v, name, writeType))
		{
			return h;
		}
	}

	// Didn't find it - if we're using a name, maybe the name is outdated?
	// This *shouldn't* happen unless there is outdated data, when it's saved again it'll be updated. So while this
	// means we're doing the entire process, it's for the sake of trying to save data and will only happen once.
	if (name != nullptr)
	{
		// If we were using a handlerName the first time, try again without it
		for (auto h : handlers_)
		{
			if (h->canHandleWrite(v, nullptr, writeType))
			{
				return h;
			}
		}
	}

	// Couldn't find a handler!
	return nullptr;
}

std::shared_ptr<SerializationHandler> SerializationHandlerManager::findHandlerRead(const NodePtr& node,
                                                                                   const char* name,
                                                                                   const char* typeName) const
{
	for (auto h : handlers_)
	{
		if (h->canHandleRead(node, name, typeName))
		{
			return h;
		}
	}

	// Didn't find it - if we're using a name, maybe the name is outdated?
	// This *shouldn't* happen unless there is outdated data, when it's saved again it'll be updated. So while this
	// means we're doing the entire process, it's for the sake of trying to save data and will only happen once.
	if (name != nullptr)
	{
		// If we were using a handlerName the first time, try again without it
		for (auto h : handlers_)
		{
			if (h->canHandleRead(node, nullptr, typeName))
			{
				return h;
			}
		}
	}

	// Couldn't find a handler!
	return nullptr;
}

} // end namespace wgt