#ifndef __INT_TO_STRING_NODE_H__
#define __INT_TO_STRING_NODE_H__

#include "core_dependency_system/i_interface.hpp"
#include "core_data_model/collection_model.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_object/i_object_manager.hpp"

#include "plugins/plg_node_editor/interfaces/i_node.hpp"

#include "StringSlot.h"
#include "IntegerSlot.h"
#include "CustomGraph.h"

namespace wgt
{
class IntToStringNode : public Implements<INode>, public Depends<IObjectManager, IDefinitionManager>
{
    DECLARE_REFLECTED
public:
    IntToStringNode(const std::string &nodeClass);
    ~IntToStringNode();

	void Init() override;

    size_t Id() const override         { return m_id; }
    std::string Class() const override      { return m_class; }
    std::string Title() const override      { return m_title; }
    std::string SubTitle() const override   { return m_subTitle; }
    std::string Category() const override   { return m_category; }
    std::string Icon() const override       { return m_icon; }
    std::string Color() const override      { return m_color; }

    float X() const override                { return m_x; }
    float Y() const override                { return m_y; }
    void SetPos(float x, float y) override;

	ObjectHandleT<ISlot> GetSlotById(size_t slotId) const override;

    bool Enabled() const override;
    void SetEnabled(bool enabled) override;

    bool Minimized() const override;
    void SetMinimized(bool minimized) override;

    bool CanConnect(ObjectHandleT<ISlot> mySlot, ObjectHandleT<ISlot> otherSlot) override;
    bool Validate(std::string &errorMessage) override;

    void OnConnect(ObjectHandleT<ISlot> mySlot, ObjectHandleT<ISlot> otherSlot) override;
    void OnDisconnect(ObjectHandleT<ISlot> mySlot, ObjectHandleT<ISlot> otherSlot) override;

    const CollectionModel* GetInputSlots() const override { return &inputSlotsModel_; }
    const CollectionModel* GetOutputSlots() const override { return &outputSlotsModel_; }

private:
    const AbstractListModel* GetInputSlotsModel() const override { return &inputSlotsModel_; }
    const AbstractListModel* GetOutputSlotsModel() const override { return &outputSlotsModel_; }

	mutable ObjectHandleT<INode> thisNode_;
	ObjectHandleT<INode> getThis() const
	{
		if (thisNode_ != nullptr)
		{
			return thisNode_;
		}

		auto thisObject = get<IObjectManager>()->getObject(this);
		assert(thisObject != nullptr);
		thisNode_ = reflectedCast<INode>(thisObject, *get<IDefinitionManager>());
		assert(thisNode_ != nullptr);
		return thisNode_;
	}

private:
    float m_x;
    float m_y;

    size_t m_id;
    std::string m_class;
    std::string m_title;
    std::string m_subTitle;
    std::string m_category;
    std::string m_icon;
    std::string m_color;

    bool m_enabled;
    bool m_minimized;
    
	std::vector<ManagedObject<ISlot>> ownedSlots_;
	std::vector<ObjectHandleT<ISlot>> inputSlots_;
    std::vector<ObjectHandleT<ISlot>> outputSlots_;
	CollectionModel inputSlotsModel_;
	CollectionModel outputSlotsModel_;
};
} // end namespace wgt
#endif // __INT_TO_STRING_NODE_H__
