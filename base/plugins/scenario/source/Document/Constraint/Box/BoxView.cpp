#include "BoxView.hpp"

#include <tools/NamedObject.hpp>

#include <QPainter>
#include <QGraphicsScene>
#include <QDebug>

BoxView::BoxView(QGraphicsObject* parent):
	QGraphicsObject{parent}
{
	this->setParentItem(parent);

	this->setZValue(parent->zValue() + 1);
	//parentItem()->scene()->addItem(this);
}

QRectF BoxView::boundingRect() const
{
	return {parentItem()->boundingRect().x() + DEMO_PIXEL_SPACING_TEST,
			parentItem()->boundingRect().y() + 50 + DEMO_PIXEL_SPACING_TEST,
			parentItem()->boundingRect().width()  - 2 * DEMO_PIXEL_SPACING_TEST,
			parentItem()->boundingRect().height() - 50 - 2 * DEMO_PIXEL_SPACING_TEST};
}

void BoxView::paint(QPainter* painter,
								const QStyleOptionGraphicsItem* option,
								QWidget* widget)
{
	painter->drawText(boundingRect(), "Box");
	painter->drawRect(boundingRect());
}