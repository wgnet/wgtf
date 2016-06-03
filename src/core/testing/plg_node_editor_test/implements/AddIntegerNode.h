#ifndef __ADD_INTEGER_NODE_H__
#define __ADD_INTEGER_NODE_H__

#include "core_data_model/i_list_model.hpp"
#include "core_dependency_system/i_interface.hpp"
#include "core_data_model/generic_list.hpp"
#include "core_reflection/object_handle.hpp"

#include "plugins/plg_node_editor/interfaces/i_node.hpp"

#include "IntegerSlot.h"
#include "CustomGraph.h"

namespace wgt
{
class AddIntegerNode : public Implements<INode>
{
    DECLARE_REFLECTED
public:
    AddIntegerNode(const std::string &nodeClass);
    ~AddIntegerNode();

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

    const GenericListT<ObjectHandleT<ISlot>>* GetInputSlots() const override { return &m_inputSlotsModel; }
    const GenericListT<ObjectHandleT<ISlot>>* GetOutputSlots() const override { return &m_outputSlotsModel; }

private:
    const IListModel* GetInputSlotsModel() const override { return &m_inputSlotsModel; }
    const IListModel* GetOutputSlotsModel() const override { return &m_outputSlotsModel; }

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

    GenericListT<ObjectHandleT<ISlot>> m_inputSlotsModel;
    GenericListT<ObjectHandleT<ISlot>> m_outputSlotsModel;
};
} // end namespace wgt
#endif // __ADD_INTEGER_NODE_H__
