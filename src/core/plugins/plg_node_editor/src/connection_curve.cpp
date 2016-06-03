#include "connection_curve.hpp"
#include <qpainter.h>

namespace wgt
{
ConnectionCurve::ConnectionCurve(QQuickItem *parent) : QQuickPaintedItem(parent)
{
    setAntialiasing(true);
    setFlag(ItemHasContents, true);
    setAcceptedMouseButtons(Qt::RightButton);
}

void ConnectionCurve::setFromPoint(const QPointF &p)
{
    m_fromPoint = p;
    emit fromPointChanged(m_fromPoint);
    update();
}

void ConnectionCurve::setToPoint(const QPointF &p)
{
    m_toPoint = p;
    emit toPointChanged(m_toPoint);
    update();
}

void ConnectionCurve::setContentTranslate(const QPointF &p)
{
    m_contentTranslate = p;
    emit contentTranslateChanged(m_contentTranslate);
    update();
}

void ConnectionCurve::setContentScale(const qreal &p)
{
    m_contentScale = p;
    emit contentScaleChanged(m_contentScale);
    update();
}

void ConnectionCurve::setFromNode(const QRectF &nodeRect)
{
    m_fromNode = nodeRect;
}

void ConnectionCurve::setToNode(const QRectF &nodeRect)
{
    m_toNode = nodeRect;
}

void ConnectionCurve::setColor(const QColor &color)
{
    m_color = color;
}

void ConnectionCurve::paint(QPainter *painter)
{
    #define CONNECTION_ROUNDING 5 * m_contentScale

    if (m_fromPoint.isNull() || m_toPoint.isNull())
        return;

    QPainterPath path;
    path.moveTo(m_fromPoint);

    bool useDefault = true;

    // For reverse connection (where OUT if to the right of IN), use more complex algorithm
    if (m_fromPoint.x() >= m_toPoint.x() - (32 * m_contentScale) && !m_fromNode.isNull() && !m_toNode.isNull())
    {
        // Determine the vertical segments of the connector
        float outPartX = m_fromPoint.x() + (28 * m_contentScale);
        float inPartX = m_toPoint.x() - (28 * m_contentScale);

        float arcWidth = 2 * CONNECTION_ROUNDING;
        
        if (m_toNode.bottom() < m_fromNode.top())
        {
            float dividerY = 0.5 * (m_toNode.bottom() + m_fromNode.top());
            dividerY = dividerY * m_contentScale + m_contentTranslate.y();

            path.lineTo(outPartX - CONNECTION_ROUNDING, m_fromPoint.y());
            path.arcTo(outPartX - arcWidth, m_fromPoint.y() - arcWidth, arcWidth, arcWidth, 270, 90);
            path.lineTo(outPartX, dividerY + CONNECTION_ROUNDING);
            path.arcTo(outPartX - arcWidth, dividerY, arcWidth, arcWidth, 0, 90);

            path.lineTo(inPartX + CONNECTION_ROUNDING, dividerY);
            path.arcTo(inPartX, dividerY - arcWidth, arcWidth, arcWidth, 270, -90);
            path.lineTo(inPartX, m_toPoint.y() + CONNECTION_ROUNDING);
            path.arcTo(inPartX, m_toPoint.y(), arcWidth, arcWidth, 180, -90);

            path.lineTo(m_toPoint);
            useDefault = false;
        }
        else if (m_fromNode.bottom() < m_toNode.top())
        {
            float dividerY = 0.5 * (m_fromNode.bottom() + m_toNode.top());
            dividerY = dividerY * m_contentScale + m_contentTranslate.y();

            path.lineTo(outPartX - CONNECTION_ROUNDING, m_fromPoint.y());
            path.arcTo(outPartX - arcWidth, m_fromPoint.y(), arcWidth, arcWidth, 90, -90);
            path.lineTo(outPartX, dividerY - CONNECTION_ROUNDING);
            path.arcTo(outPartX - arcWidth, dividerY - arcWidth, arcWidth, arcWidth, 0, -90);

            path.lineTo(inPartX + CONNECTION_ROUNDING, dividerY);
            path.arcTo(inPartX, dividerY, arcWidth, arcWidth, 90, 90);
            path.lineTo(inPartX, m_toPoint.y() - CONNECTION_ROUNDING);
            path.arcTo(inPartX, m_toPoint.y() - arcWidth, arcWidth, arcWidth, 180, 90);

            path.lineTo(m_toPoint);
            useDefault = false;
        }
    }

    if (useDefault)
    {
        qreal dx = m_toPoint.x() - m_fromPoint.x();
        qreal dy = m_toPoint.y() - m_fromPoint.y();

        QPointF ctr1(m_fromPoint.x() + dx * 0.5, m_fromPoint.y() + dy * 0.05);
        QPointF ctr2(m_fromPoint.x() + dx * 0.5, m_fromPoint.y() + dy * 0.95);

        path.cubicTo(ctr1, ctr2, m_toPoint);
    }

    QColor maskColor = Qt::black;

    painter->setPen(QPen(maskColor, 4 * m_contentScale, Qt::SolidLine,
        Qt::RoundCap, Qt::RoundJoin));
    painter->drawPath(path);

    painter->setPen(QPen(m_color, 2 * m_contentScale, Qt::SolidLine,
        Qt::RoundCap, Qt::RoundJoin));
    painter->drawPath(path);

    m_path = path;
}

void ConnectionCurve::mousePressEvent(QMouseEvent *event)
{
    QPainterPath path;
    path.moveTo(event->pos());
    path.addEllipse(event->pos(), 4, 4);

    bool result = m_path.intersects(path);
    event->setAccepted(result);

    if (result)
    {
        emit connectionClicked();
    }        
}
} // end namespace wgt
