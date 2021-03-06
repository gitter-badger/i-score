#include "RackView.hpp"

#include <iscore/tools/NamedObject.hpp>

#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QApplication>
#include <QPalette>

RackView::RackView(QGraphicsObject* parent) :
    QGraphicsObject {parent}
{
    this->setZValue(parent->zValue() + 1);
}

QRectF RackView::boundingRect() const
{
    return {0,
            0,
            qreal(m_width),
            qreal(m_height)
           };
}

void RackView::paint(QPainter* painter,
                    const QStyleOptionGraphicsItem* option,
                    QWidget* widget)
{
    painter->setPen(Qt::white);

    QRectF header {0, 0, boundingRect().width(), 20};
    QFont f("Ubuntu");
    f.setPixelSize(16);
    f.setBold(true);
    painter->setFont(f);

    painter->drawText(header, Qt::AlignCenter, m_text);
}
