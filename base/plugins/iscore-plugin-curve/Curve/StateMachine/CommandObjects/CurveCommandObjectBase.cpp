#include "CurveCommandObjectBase.hpp"
#include "Curve/CurvePresenter.hpp"
#include "Curve/CurveModel.hpp"
#include "Curve/Segment/CurveSegmentModel.hpp"
#include "Curve/Point/CurvePointModel.hpp"
#include "Curve/Commands/UpdateCurve.hpp"
#include "Curve/Segment/CurveSegmentModelSerialization.hpp"

#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/document/DocumentInterface.hpp>

CurveCommandObjectBase::CurveCommandObjectBase(
        CurvePresenter* pres,
        iscore::CommandStack& stack):
    m_presenter{pres},
    m_dispatcher{stack}
{

}

void CurveCommandObjectBase::press()
{
    const auto& current = m_presenter->model()->segments();

    // Serialize the current state of the curve
    m_startSegments.clear();
    m_startSegments.resize(current.size());
    int i = 0;
    for(const auto& segment : current)
    {
        Serializer<DataStream> s{&m_startSegments[i++]};
        s.readFrom(*segment);
    }

    m_xmin = 0;
    m_xmax = 1;

    on_press();
}

void CurveCommandObjectBase::handleLocking()
{
    double current_x = m_state->currentPoint.x();
    double current_y = m_state->currentPoint.y();

    // In any case we lock between O - 1 in both axes.
    if(current_x < 0.)
        m_state->currentPoint.setX(0.);
    if(current_x > 1.)
        m_state->currentPoint.setX(1.);
    if(current_y < 0.)
        m_state->currentPoint.setY(0.);
    if(current_y > 1.)
        m_state->currentPoint.setY(1.);

    // And more specifically...
    if(m_presenter->lockBetweenPoints())
    {
        if(current_x <= m_xmin)
            m_state->currentPoint.setX(m_xmin + 0.001);

        if(current_x >= m_xmax)
            m_state->currentPoint.setX(m_xmax - 0.001);
    }
}

QVector<CurveSegmentModel*> CurveCommandObjectBase::deserializeSegments() const
{
    QVector<CurveSegmentModel*> segments;
    std::transform(m_startSegments.begin(), m_startSegments.end(), std::back_inserter(segments),
                   [] (QByteArray arr)
    {
        Deserializer<DataStream> des(arr);
        return createCurveSegment(des, nullptr);
    });

    return segments;
}

void CurveCommandObjectBase::submit(const QVector<CurveSegmentModel*> segments)
{
    QVector<QByteArray> newSegments;
    newSegments.resize(segments.size());
    int i = 0;
    for(const auto& segment : segments)
    {
        Serializer<DataStream> s(&newSegments[i++]);
        s.readFrom(*segment);
    }

    qDeleteAll(segments);

    m_dispatcher.submitCommand<UpdateCurve>(iscore::IDocument::path(m_presenter->model()),
                                            std::move(newSegments));
}
