#pragma once

#include "core_reflection/reflected_object.hpp"
#include "core_data_model/reflection_proto/property_tree_model.hpp"
#include "core_object/managed_object.hpp"

#include "wg_types/vector3.hpp"

#include <string>
#include <vector>

/**
 *	Test data structures that emulate WoWS World Editor ChunkItem data structures.
 */

namespace wgt
{

class IComponentContext;

namespace MultiEditTest
{

class ChunkItem
{
	DECLARE_REFLECTED
public:
	ChunkItem(const std::string& chunk,
		const std::string& selectionInfo);
	std::string chunk_;
	std::string selectionInfo_;
};

class ChunkItemPosition
{
	DECLARE_REFLECTED
public:
	ChunkItemPosition();
	Vector3 pivotPosition_;
	Vector3 position_;
};

class ChunkItemRotation
{
	DECLARE_REFLECTED
public:
	ChunkItemRotation();
	Vector3 rotation_;
};

class ChunkItemScale
{
	DECLARE_REFLECTED
public:
	ChunkItemScale();
	Vector3 scale_;
};

class Dye
{
	DECLARE_REFLECTED
public:
	Dye();
	std::string name_;
};

class ChunkModel
	: public ChunkItem
	, public ChunkItemPosition
	, public ChunkItemRotation
	, public ChunkItemScale
{
	DECLARE_REFLECTED
public:
	ChunkModel();
	std::string modelName_;
	ObjectHandleT<Dye> dye_;
	float animationSpeed_;
};

class ChunkForestTree
	: public ChunkItem
	, public ChunkItemPosition
	, public ChunkItemRotation
	, public ChunkItemScale
{
	DECLARE_REFLECTED
public:
	ChunkForestTree();
	std::string fileName_;
};

class ChunkItemTestContext
{
	DECLARE_REFLECTED

public:
	ChunkItemTestContext(IComponentContext& context);

	AbstractTreeModel* getChunkModel();
	AbstractTreeModel* getChunkTree();

	ManagedObject<ChunkModel> chunkModel_;
	ManagedObject<ChunkForestTree> chunkTree_;

	std::unique_ptr<proto::PropertyTreeModel> modelDataModel_;
	std::unique_ptr<proto::PropertyTreeModel> treeDataModel_;

};

} // end namespace MultiEditTest

} // end namespace wgt
