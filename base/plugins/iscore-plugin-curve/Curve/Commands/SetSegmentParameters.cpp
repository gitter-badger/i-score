#include "SetSegmentParameters.hpp"
#include "Curve/CurveModel.hpp"
#include "Curve/Segment/CurveSegmentModel.hpp"

SetSegmentParameters::SetSegmentParameters(
        ObjectPath&& model,
        SegmentParameterMap&& parameters):
    iscore::SerializableCommand{
        "AutomationControl", commandName(), description()},
    m_model{std::move(model)},
    m_new{std::move(parameters)}
{
    const auto& curve = m_model.find<CurveModel>();
    for(const auto& elt : m_new.keys())
    {
        CurveSegmentModel* seg = curve.segments().at(elt);
        m_old.insert(elt, {seg->verticalParameter(), seg->horizontalParameter()});
    }
}

void SetSegmentParameters::undo()
{
    auto& curve = m_model.find<CurveModel>();
    for(const auto& elt : m_old.keys())
    {
        CurveSegmentModel* seg = curve.segments().at(elt);

        if(m_old.value(elt).first)
            seg->setVerticalParameter(*m_old.value(elt).first);
        if(m_old.value(elt).second)
            seg->setHorizontalParameter(*m_old.value(elt).second);
    }

    curve.changed();
}

void SetSegmentParameters::redo()
{
    auto& curve = m_model.find<CurveModel>();
    for(const auto& elt : m_new.keys())
    {
        CurveSegmentModel* seg = curve.segments().at(elt);

        seg->setVerticalParameter(m_new.value(elt).first);
        seg->setHorizontalParameter(m_new.value(elt).second);
    }

    curve.changed();
}

void SetSegmentParameters::update(ObjectPath&& model, SegmentParameterMap &&segments)
{
    m_new = std::move(segments);
}

void SetSegmentParameters::serializeImpl(QDataStream& s) const
{
   s << m_model << m_old << m_new;
}

void SetSegmentParameters::deserializeImpl(QDataStream& s)
{
    s >> m_model >> m_old >> m_new;
}
