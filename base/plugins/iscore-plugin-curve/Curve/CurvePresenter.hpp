#pragma once
#include <QStateMachine>
#include <QPointF>
#include <QVector>
#include <iscore/command/Dispatchers/CommandDispatcher.hpp>
#include <iscore/selection/SelectionDispatcher.hpp>

class CurveModel;
class CurveView;
class CurvePointView;
class CurveSegmentView;
class CurveStateMachine;
class QAction;
class QMenu;
class QActionGroup;
class CurvePresenter : public QObject
{
        Q_OBJECT
    public:
        enum class AddPointBehaviour
        { LinearBefore, LinearAfter, DuplicateSegment };
        CurvePresenter(CurveModel*, CurveView*, QObject* parent);
        virtual ~CurvePresenter();

        const QList<CurvePointView*>& points() const
        { return m_points; }
        const QList<CurveSegmentView*>& segments() const
        { return m_segments; }

        // Removes all the points & segments
        void clear();

        CurveModel* model() const;
        CurveView& view() const;

        // Taken from the view. First set this,
        // then send signals to the state machine.
        QPointF pressedPoint() const;

        void setRect(const QRectF& rect);

        bool lockBetweenPoints() const;
        void setLockBetweenPoints(bool lockBetweenPoints);

        bool suppressOnOverlap() const;
        void setSuppressOnOverlap(bool suppressOnOverlap);

        bool stretchBothBounds() const;
        void setStretchBothBounds(bool stretchBothBounds);

        AddPointBehaviour addPointBehaviour() const;
        void setAddPointBehaviour(const AddPointBehaviour &addPointBehaviour);

        void enableActions(bool);

        // Changes the colors
        void enable();
        void disable();


    private:
        // Context menu actions
        void removeSelection();
        void updateSegmentsType(const QString& segment);

        // Setup utilities
        void setPos(CurvePointView*);
        void setPos(CurveSegmentView*);
        void setupSignals();
        void setupView();
        void setupStateMachine();
        void setupContextMenu();

        // Adding
        void addPoint(CurvePointView*);
        void addSegment(CurveSegmentView*);

        CurveStateMachine* m_sm{};

        CurveModel* m_model{};
        CurveView* m_view{};

        QList<CurvePointView*> m_points;
        QList<CurveSegmentView*> m_segments;

        // Boolean values that keep the editing state. Should they go here ?
        // Maybe in the settings, instead ?
        bool m_lockBetweenPoints{};
        bool m_suppressOnOverlap{};
        bool m_stretchBothBounds{};
        AddPointBehaviour m_addPointBehaviour{};

        // Required dispatchers
        CommandDispatcher<> m_commandDispatcher;
        iscore::SelectionDispatcher m_selectionDispatcher;

        QMenu* m_contextMenu{};
        QActionGroup* m_actions{};
};
