#pragma once

#include "core_reflection/reflected_object.hpp"

#include <memory>

namespace wgt
{
class AbstractTreeModel;
class ObjectHandle;
class Variant;

/**
 *	Stores variables that can be accessed by QML.
 */
class TestUIContext
{
public:
	DECLARE_REFLECTED
	TestUIContext();
	~TestUIContext();

	void initialize(std::unique_ptr<AbstractTreeModel>&& model);
	AbstractTreeModel* treeModel() const;
	void updateValues();
	void undoUpdateValues(const ObjectHandle&, Variant);
	void redoUpdateValues(const ObjectHandle&, Variant);

private:
	std::unique_ptr<AbstractTreeModel> model_;
};
} // end namespace wgt
