#ifndef __CONNECTION_CURVE_H__
#define __CONNECTION_CURVE_H__

#include <QtQuick/QQuickPaintedItem>
#include <QPainterPath>
#include "core_qt_common/qt_new_handler.hpp"

namespace wgt
{
class ConnectionCurve : public QQuickPaintedItem
{
	
    Q_OBJECT

	DECLARE_QT_MEMORY_HANDLER

    Q_PROPERTY(QPointF fromPoint READ fromPoint WRITE setFromPoint NOTIFY fromPointChanged)
    Q_PROPERTY(QPointF toPoint READ toPoint WRITE setToPoint NOTIFY toPointChanged)

    Q_PROPERTY(QPointF contentTranslate READ contentTranslate WRITE setContentTranslate NOTIFY contentTranslateChanged)
    Q_PROPERTY(qreal contentScale READ contentScale WRITE setContentScale NOTIFY contentScaleChanged)

    Q_PROPERTY(QRectF fromNode READ fromNode WRITE setFromNode)
    Q_PROPERTY(QRectF toNode READ toNode WRITE setToNode)

    Q_PROPERTY(QColor connectionColor READ color WRITE setColor)
public:
    ConnectionCurve(QQuickItem *parent = 0);

    void setFromPoint(const QPointF &p);
    QPointF fromPoint() const { return m_fromPoint; }

    void setToPoint(const QPointF &p);
    QPointF toPoint() const { return m_toPoint; }

    void setFromNode(const QRectF &nodeRect);
    QRectF fromNode() const { return  m_fromNode; }

    void setToNode(const QRectF &nodeRect);
    QRectF toNode() const { return  m_toNode; }

    void setContentTranslate(const QPointF &translate);
    QPointF contentTranslate() const { return m_contentTranslate; }

    void setContentScale(const qreal &scale);
    qreal contentScale() const { return m_contentScale; }

    void setColor(const QColor &color);
    QColor color() const { return m_color; }

    void paint(QPainter *painter) override;

signals:
    void fromPointChanged(const QPointF &p);
    void toPointChanged(const QPointF &p);
    void contentTranslateChanged(const QPointF &translate);
    void contentScaleChanged(const qreal &scale);
    void connectionClicked();

private:
    void mousePressEvent(QMouseEvent *event) override;

private:
    QPointF m_fromPoint;
    QPointF m_toPoint;

    QRectF m_fromNode;
    QRectF m_toNode;

    QPointF m_contentTranslate;
    qreal m_contentScale;

    QColor m_color;
    QPainterPath m_path;
};
} // end namespace wgt
#endif // __CONNECTION_CURVE_H__
