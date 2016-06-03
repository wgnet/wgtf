
#ifndef __GRAPHEDITOR_GRAPHNODE_H__
#define __GRAPHEDITOR_GRAPHNODE_H__

#include <core_common/signal.hpp>
#include <core_data_model/i_list_model.hpp>
#include <core_reflection/reflected_object.hpp>
#include <core_reflection/object_handle.hpp>

#include <string>
#include <QPointF>

namespace wgt
{
class ConnectionSlot;
class GraphNode
{
    DECLARE_REFLECTED
public:
    GraphNode();
    ~GraphNode();

    struct Params
    {
        typedef ObjectHandleT<ConnectionSlot> TSlotPtr;
        typedef std::vector<TSlotPtr> TSlotCollection;

        TSlotCollection inputSlots;
        TSlotCollection outputSlots;
        std::string typeId;
    };

    void Init(Params&& params);

    float GetPosX() const;
    void SetPosX(float posX);
    float GetPosY() const;
    void SetPosY(float posY);

    std::string const& GetTitle() const;
    void SetTitle(std::string const& title);

    size_t GetUID() const;
    std::string const& GetType()
    {
        return typeId;
    }

    void Shift(float modelShiftX, float modelShiftY);
    void ShiftImpl(float modelShiftX, float modelShiftY);

    Signal<void(float x, float y)> MoveNodes;
    Signal<void(GraphNode*)> Changed;

private:
    IListModel* GetInputSlots() const;
    IListModel* GetOutputSlots() const;

    /// we need this method to call it through NGT reflection system and signal qml that value changed
    void PosXChanged(const float& x);
    void PosYChanged(const float& y);

private:
    std::string title;
    float modelX, modelY;

    std::unique_ptr<IListModel> inputSlots;
    std::unique_ptr<IListModel> outputSlots;
    std::string typeId;
};
} // end namespace wgt
#endif // __GRAPHEDITOR_GRAPHNODE_H__
