#include "TemporalScenarioView.hpp"

#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QMenu>

#include "TemporalScenarioPresenter.hpp"

TemporalScenarioView::TemporalScenarioView(QGraphicsObject* parent) :
    Layer {parent}
{
    this->setFlags(ItemClipsChildrenToShape | ItemIsSelectable | ItemIsFocusable); // TODO should not be ?
    this->setCursor(Qt::ArrowCursor);
    setAcceptDrops(true);

    this->setZValue(parent->zValue() + 1);
}

TemporalScenarioView::~TemporalScenarioView()
{
}

void TemporalScenarioView::paint(QPainter* painter,
                                 const QStyleOptionGraphicsItem* option,
                                 QWidget* widget)
{
    if(m_lock)
    {
        painter->setBrush({Qt::red, Qt::DiagCrossPattern});
        painter->drawRect(boundingRect());
    }

    if(m_selectArea != QRectF{})
    {
        painter->setCompositionMode(QPainter::CompositionMode_Xor);
        painter->setPen(QPen{QColor{0, 0, 0, 127}, 2, Qt::DashLine, Qt::SquareCap, Qt::BevelJoin});

        painter->drawRect(m_selectArea);
    }
}


void TemporalScenarioView::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if(event->button() == Qt::LeftButton)
        emit scenarioPressed(event->scenePos());
}

void TemporalScenarioView::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    emit scenarioMoved(event->scenePos());
}

void TemporalScenarioView::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    emit scenarioReleased(event->scenePos());
}

void TemporalScenarioView::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    emit askContextMenu(event->screenPos());
}

void TemporalScenarioView::keyPressEvent(QKeyEvent* event)
{
    QGraphicsObject::keyPressEvent(event);
    if(event->key() == Qt::Key_Escape)
    {
        emit escPressed();
    }

    emit keyPressed(event->key());

    if(event->key() == Qt::Key_Shift || event->key() == Qt::Key_Control)
    {
        emit keyPressed(event->key());
    }
}

void TemporalScenarioView::keyReleaseEvent(QKeyEvent *event)
{
    QGraphicsObject::keyReleaseEvent(event);
    if(event->key() == Qt::Key_Shift || event->key() == Qt::Key_Control)
    {
        emit keyReleased(event->key());
    }
}


#include <QMimeData>
void TemporalScenarioView::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    emit dropReceived(event->scenePos(), event->mimeData());
}
