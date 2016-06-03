
#ifndef __GRAPHEDITOR_GRAPHEDITOR_H__
#define __GRAPHEDITOR_GRAPHEDITOR_H__

#include <core_data_model/i_list_model.hpp>
#include <core_reflection/reflected_object.hpp>

#include <QObject>

#include <memory>

namespace wgt
{
class GraphEditor : public QObject
{
    Q_OBJECT

    DECLARE_REFLECTED
public:
    GraphEditor();
    ~GraphEditor();

private:
    IListModel* GetConnectorsModel() const;
    IListModel* GetNodeModel() const;
    IListModel* GetRootContextMenuModel() const;
    IListModel* GetNodeContextMenuModel() const;
    IListModel* GetSlotContextMenuModel() const;

    void CreateConnection(size_t outputUID, size_t inputUID);
};
} // end namespace wgt
#endif // __GRAPHEDITOR_GRAPHEDITOR_H__
