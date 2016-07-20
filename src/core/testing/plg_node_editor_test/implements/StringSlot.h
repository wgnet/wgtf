#ifndef __STRING_SLOT_H__
#define __STRING_SLOT_H__

#include "core_dependency_system/i_interface.hpp"
#include "plugins/plg_node_editor/interfaces/i_slot.hpp"
#include "plugins/plg_node_editor/interfaces/i_connection.hpp"

namespace wgt
{
class StringSlot : public Implements<ISlot>
{
    DECLARE_REFLECTED
public:
    StringSlot(INode *node, bool isInput);
    virtual ~StringSlot(){}

    size_t Id() const override { return m_id; }
    bool IsInput() const override { return m_isInput; }
    std::string Icon() const override;
    std::string Color() const override;
    MetaType* Type() const override;
    bool Editable() const override;
    INode* Node() const override;

    std::string Label() const override;
    void setLabel(const std::string &label) override;

    const std::set<size_t>* GetConnectionIds() const  override { return &m_connectionIds; }
    const GenericListT<ISlot*>* GetConnectedSlots() const override;

    bool CanConnect(ObjectHandleT<ISlot> slot) override;
    bool IsConnected() const override { return !m_connectedSlots.empty(); }

    bool Connect(size_t connectionID, ObjectHandleT<ISlot> slot) override;
    bool Disconnect(size_t connectionID, ObjectHandleT<ISlot> slot) override;
private:
    size_t m_id;
    std::string m_label;
    std::string m_icon;
    std::string m_color;

    bool m_editable;
    bool m_isInput;

    INode *m_pNode;
    std::set<size_t> m_connectionIds;
    GenericListT<ISlot*> m_connectedSlots;
};
} // end namespace wgt
#endif //__STRING_SLOT_H__
