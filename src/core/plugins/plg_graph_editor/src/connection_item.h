
#ifndef __GRAPHEDITOR_CONNECTIONITEM_H__
#define __GRAPHEDITOR_CONNECTIONITEM_H__

#include <QObject>
#include <QQuickItem>

namespace wgt
{
class ConnectionItem : public QQuickItem
{
    Q_OBJECT
public:
    ConnectionItem(QQuickItem* parent = 0);
    virtual ~ConnectionItem();

    size_t GetUID() const;

protected:
    QSGNode* updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* updatePaintNodeData) override;

private:
    Q_PROPERTY(QQuickItem* outputSlot READ GetOutputSlotItem WRITE SetOutputSlotItem);
    Q_PROPERTY(QQuickItem* inputSlot READ GetInputSlotItem WRITE SetInputSlotItem);
    Q_PROPERTY(QVariant uid READ GetQmlUID WRITE SetQmlUID)

    QQuickItem* GetOutputSlotItem() const;
    QQuickItem* GetInputSlotItem() const;
    void SetOutputSlotItem(QQuickItem* item);
    void SetInputSlotItem(QQuickItem* item);
    QVariant GetQmlUID() const;
    void SetQmlUID(QVariant const& uid);

private:
    QQuickItem* outputSlot;
    QQuickItem* inputSlot;

    size_t uid;
};

class InteractiveConnectionItem : public QQuickItem
{
    Q_OBJECT
public:
    InteractiveConnectionItem(QQuickItem* parent = 0);

protected:
    QSGNode* updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* updatePaintNodeData) override;

private:
    Q_PROPERTY(float startX READ GetStartX WRITE SetStartX);
    Q_PROPERTY(float startY READ GetStartY WRITE SetStartY);
    Q_PROPERTY(float endX READ GetEndX WRITE SetEndX);
    Q_PROPERTY(float endY READ GetEndY WRITE SetEndY);

    float GetStartX() const;
    float GetStartY() const;
    float GetEndX() const;
    float GetEndY() const;

    void SetStartX(float x);
    void SetStartY(float y);
    void SetEndX(float x);
    void SetEndY(float y);

private:
    QPointF startPt;
    QPointF endPt;
};
} // end namespace wgt
#endif // __GRAPHEDITOR_CONNECTIONITEM_H__
