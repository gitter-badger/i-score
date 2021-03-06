#pragma once
#include <QGraphicsItem>
class CurveSegmentModel;
class CurveSegmentView : public QGraphicsObject
{
        Q_OBJECT
    public:
        CurveSegmentView(CurveSegmentModel* model, QGraphicsItem* parent);
        int type() const override;
        QRectF boundingRect() const override;
        QPainterPath shape() const override;
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

        CurveSegmentModel& model() const
        { return *m_model; }
        void setRect(const QRectF& theRect);

        void setSelected(bool selected);

        void enable();
        void disable();

    signals:
        void contextMenuRequested(const QPoint&);

    protected:
        void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

    private:
        void updatePoints();
        void updateStroke();
        // Takes a table of points and draws them in a square given by the boundingRect
        // QGraphicsItem interface
        QRectF m_rect;

        CurveSegmentModel* m_model{};
        bool m_selected{};

        QPainterPath m_unstrockedShape;
        QPainterPath m_shape;
        QVector<QLineF> m_lines;

        bool m_enabled{true};
};
