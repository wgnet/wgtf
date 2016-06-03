
#include "connection_item.h"
#include "quick_items_manager.h"

#include <QColor>

#include <QSGGeometryNode>
#include <QSGGeometry>
#include <QSGFlatColorMaterial>
#include <QVector2D>

#include <stdint.h>

namespace wgt
{
ConnectionItem::ConnectionItem(QQuickItem* parent)
    : QQuickItem(parent)
	, outputSlot( nullptr )
	, inputSlot( nullptr )
	, uid( 0 )
{
    setFlag(QQuickItem::ItemHasContents);
}

ConnectionItem::~ConnectionItem()
{
    QuickItemsManager::Instance().UnregisterObject(this);
}

QQuickItem* ConnectionItem::GetOutputSlotItem() const
{
    return outputSlot;
}

QQuickItem* ConnectionItem::GetInputSlotItem() const
{
    return inputSlot;
}

void ConnectionItem::SetOutputSlotItem(QQuickItem* item)
{
    outputSlot = item;
}

void ConnectionItem::SetInputSlotItem(QQuickItem* item)
{
    inputSlot = item;
}

size_t ConnectionItem::GetUID() const
{
    return uid;
}

QVariant ConnectionItem::GetQmlUID() const
{
    return static_cast< uint64_t >( uid );
}

void ConnectionItem::SetQmlUID(QVariant const& uid_)
{
    if (uid_.canConvert<size_t>())
    {
        size_t uidValue = uid_.value<size_t>();
        QuickItemsManager& mng = QuickItemsManager::Instance();
        mng.UnregisterObject(this);
        uid = uidValue;
        mng.RegisterObject(this);
    }
}

namespace
{
class SGLineGeometryNode : public QSGGeometryNode
{
public:
    SGLineGeometryNode(QColor const& color)
        : geometry(QSGGeometry::defaultAttributes_Point2D(), 4 * 3)
    {
        material.setColor(color);
        setMaterial(&material);
        setGeometry(&geometry);
    }

    void SetPoints(QPointF const& startPt, QPointF const& endPt)
    {
        QVector2D start(startPt);
        QVector2D end(endPt);
        float controlPointOffset = std::min((end - start).length(), 100.0f);
        QVector2D firstControl(start.x() + controlPointOffset, start.y());
        QVector2D secondControl(end.x() - controlPointOffset, end.y());

        float length = (end - secondControl).length() +
        (secondControl - firstControl).length() +
        (firstControl - start).length();

        float segments = length / 20.0f;
        int segmentCount = ceilf(sqrt(segments * segments * 0.6f + 125.0f));

        double delta = 1.0 / segmentCount;

        geometry.allocate(4 * segmentCount);
        QSGGeometry::Point2D* points = geometry.vertexDataAsPoint2D();

        QVector2D prevPt(start);
        double t = 0.0f;
        for (int i = 1; i <= segmentCount; ++i)
        {
            t += delta;
            double t2 = t * t;
            double t3 = t * t * t;
            double nt = 1.0 - t;
            double nt2 = nt * nt;
            double nt3 = nt * nt * nt;

            QVector2D nextPt(start.x() * nt3 + 3.0 * firstControl.x() * nt2 * t + 3.0 * secondControl.x() * nt * t2 + end.x() * t3,
                             start.y() * nt3 + 3.0 * firstControl.y() * nt2 * t + 3.0 * secondControl.y() * nt * t2 + end.y() * t3);
            ApplySegment(points, prevPt, nextPt);
            points += 4;
            prevPt = nextPt;
        }

        markDirty(QSGNode::DirtyGeometry);
    }

private:
    void ApplySegment(QSGGeometry::Point2D* data, QVector2D const& startPt, QVector2D const& endPt)
    {
        QVector2D tangent = endPt - startPt;
        QVector2D normal = 1.5 * QVector2D(-tangent.y(), tangent.x()).normalized();

        QVector2D anchors[4] =
        {
          QVector2D(startPt + normal),
          QVector2D(startPt - normal),
          QVector2D(endPt + normal),
          QVector2D(endPt - normal)
        };

        data[0].x = anchors[0].x();
        data[0].y = anchors[0].y();
        data[1].x = anchors[1].x();
        data[1].y = anchors[1].y();
        data[2].x = anchors[2].x();
        data[2].y = anchors[2].y();
        data[3].x = anchors[3].x();
        data[3].y = anchors[3].y();
    }

private:
    QSGFlatColorMaterial material;
    QSGGeometry geometry;
};
}

QSGNode* ConnectionItem::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* updatePaintNodeData)
{
    QSGNode* node = oldNode;
    if (node == nullptr)
    {
        node = new SGLineGeometryNode(Qt::blue);
    }

    if (inputSlot != nullptr && outputSlot != nullptr)
    {
        QQuickItem* parent = parentItem();
        QPointF startPt = parent->mapFromItem(inputSlot, QPointF(inputSlot->width() / 2.0f, inputSlot->height() / 2.0f));
        QPointF endPt = parent->mapFromItem(outputSlot, QPointF(outputSlot->width() / 2.0f, outputSlot->height() / 2.0f));
        SGLineGeometryNode* lineNode = static_cast<SGLineGeometryNode*>(node);

        lineNode->SetPoints(startPt, endPt);
    }

    return node;
}

InteractiveConnectionItem::InteractiveConnectionItem(QQuickItem* parent /*= 0*/)
    : QQuickItem(parent)
{
    setFlag(QQuickItem::ItemHasContents);
    setAntialiasing(true);
}

float InteractiveConnectionItem::GetStartX() const
{
    return startPt.x();
}

float InteractiveConnectionItem::GetStartY() const
{
    return startPt.y();
}

float InteractiveConnectionItem::GetEndX() const
{
    return endPt.x();
}

float InteractiveConnectionItem::GetEndY() const
{
    return endPt.y();
}

void InteractiveConnectionItem::SetStartX(float x)
{
    startPt.setX(x);
    update();
}

void InteractiveConnectionItem::SetStartY(float y)
{
    startPt.setY(y);
    update();
}

void InteractiveConnectionItem::SetEndX(float x)
{
    endPt.setX(x);
    update();
}

void InteractiveConnectionItem::SetEndY(float y)
{
    endPt.setY(y);
    update();
}

QSGNode* InteractiveConnectionItem::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* updatePaintNodeData)
{
    QSGNode* node = oldNode;
    if (node == nullptr)
    {
        node = new SGLineGeometryNode(Qt::blue);
    }

    SGLineGeometryNode* lineNode = static_cast<SGLineGeometryNode*>(node);
    lineNode->SetPoints(startPt, endPt);

    return node;
}
} // end namespace wgt
