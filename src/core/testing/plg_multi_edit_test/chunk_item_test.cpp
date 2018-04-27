#include "chunk_item_test.hpp"

#include "core_object/managed_object.hpp"
#include "core_reflection/object_handle.hpp"

namespace wgt
{

namespace MultiEditTest
{

ChunkItem::ChunkItem(const std::string& chunk,
	const std::string& selectionInfo)
	: chunk_(chunk)
	, selectionInfo_(selectionInfo)
{
}

ChunkItemPosition::ChunkItemPosition()
	: pivotPosition_(1.0f, 2.0f, 3.0f)
	, position_(1.0f, 2.0f, 3.0f)
{
}

ChunkItemRotation::ChunkItemRotation()
	: rotation_(0.0f, 0.0f, 0.0f)
{
}

ChunkItemScale::ChunkItemScale()
	: scale_(1.0f, 1.0f, 1.0f)
{
}

Dye::Dye()
	: name_("red dye")
{
}

ChunkModel::ChunkModel()
	: ChunkItem("11110000o",
		"selected")
	, modelName_("model")
	, animationSpeed_(1.0f)
{
}

ChunkForestTree::ChunkForestTree()
	: ChunkItem("0000ffffo",
		"selected")
	, fileName_("tree file")
{
}

ChunkItemTestContext::ChunkItemTestContext(IComponentContext& context)
{
	auto pDefinitionManager = context.queryInterface<IDefinitionManager>();
	if (pDefinitionManager == nullptr)
	{
		return;
	}
	auto& definitionManager = (*pDefinitionManager);

	chunkModel_ = ManagedObject<ChunkModel>::make();
	chunkTree_ = ManagedObject<ChunkForestTree>::make();

	modelDataModel_ = std::make_unique<proto::PropertyTreeModel>(chunkModel_.getHandle());
	treeDataModel_ = std::make_unique<proto::PropertyTreeModel>(chunkTree_.getHandle());
}

AbstractTreeModel* ChunkItemTestContext::getChunkModel()
{
	return modelDataModel_.get();
}

AbstractTreeModel* ChunkItemTestContext::getChunkTree()
{
	return treeDataModel_.get();
}

} // end namespace MultiEditTest

} // end namespace wgt
