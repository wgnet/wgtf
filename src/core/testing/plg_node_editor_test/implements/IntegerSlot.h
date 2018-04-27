#ifndef __INTEGER_SLOT_H__
#define __INTEGER_SLOT_H__

#include "core_dependency_system/i_interface.hpp"
#include "core_dependency_system/depends.hpp"
#include "plugins/plg_node_editor/interfaces/i_slot.hpp"
#include "plugins/plg_node_editor/interfaces/i_connection.hpp"
#include "core_object/i_object_manager.hpp"
#include "core_reflection/utilities/object_handle_reflection_utils.hpp"

namespace wgt
{
class IntegerSlot : public Implements<ISlot>, public Depends<IObjectManager, IDefinitionManager>
{
    DECLARE_REFLECTED
public:
    IntegerSlot(ObjectHandleT<INode> node, bool isInput);
    virtual ~IntegerSlot(){}

    size_t Id() const override { return m_id; }
    bool IsInput() const override { return m_isInput; }
    std::string Icon() const override;
    std::string Color() const override;
    MetaType* Type() const override;
    bool Editable() const override;
	ObjectHandleT<INode> Node() const override;

    std::string Label() const override;
    void setLabel(const std::string &label) override;

    const std::set<size_t>* GetConnectionIds() const  override { return &m_connectionIds; }
    const CollectionModel* GetConnectedSlots() const override;

    bool CanConnect(ObjectHandleT<ISlot> slot) override;
    bool IsConnected() const override { return !m_connectedSlots.empty(); }

    bool Connect(size_t connectionID, ObjectHandleT<ISlot> slot) override;
    bool Disconnect(size_t connectionID, ObjectHandleT<ISlot> slot) override;
private:

	mutable ObjectHandleT<ISlot> thisSlot_;
	ObjectHandleT<ISlot> getThis() const
	{
		if (thisSlot_ != nullptr)
		{
			return thisSlot_;
		}

		auto thisObject = get<IObjectManager>()->getObject(this);
		assert(thisObject != nullptr);
		thisSlot_ = reflectedCast<ISlot>(thisObject, *get<IDefinitionManager>());
		assert(thisSlot_ != nullptr);
		return thisSlot_;
	}

    size_t m_id;
    std::string m_label;
    std::string m_icon;
    std::string m_color;

    bool m_editable;
    bool m_isInput;

	ObjectHandleT<INode> m_pNode;
    std::set<size_t> m_connectionIds;
    std::vector<ObjectHandleT<ISlot>> m_connectedSlots;
	CollectionModel connectedSlotsModel;
};
} // end namespace wgt
#endif //__STRING_SLOT_H__
