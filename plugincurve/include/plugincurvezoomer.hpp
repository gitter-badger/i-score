#ifndef PLUGINCURVEZOOMER_HPP
#define PLUGINCURVEZOOMER_HPP

#include <QGraphicsObject>

/*!
 *  This class is the parent class of graphics item. Zoom transformation will be applied on it.
 *  @n
 *
 *  @brief Parent of graphics Item. Zoom methods.
 *  @author Simon Touchard, Myriam Desainte-Catherine
 *  @date 2014
 */


class PluginCurveZoomer : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit PluginCurveZoomer(QGraphicsObject *parent = 0);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;
    void zoom (QPointF origin, qreal delta);
    void translateX (qreal dx);
    void translateY (qreal dy);
signals:

public slots:

};

#endif // PLUGINCURVEZOOMER_HPP
