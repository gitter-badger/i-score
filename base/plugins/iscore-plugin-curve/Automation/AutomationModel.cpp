#include "AutomationModel.hpp"
#include "AutomationViewModel.hpp"
#include "State/AutomationState.hpp"

#include "Curve/CurveModel.hpp"
#include "Curve/Segment/LinearCurveSegmentModel.hpp"
#include "Curve/Segment/GammaCurveSegmentModel.hpp"
#include "Curve/Segment/SinCurveSegmentModel.hpp"
#include "Curve/Point/CurvePointModel.hpp"

AutomationModel::AutomationModel(
        const TimeValue& duration,
        const id_type<ProcessModel>& id,
        QObject* parent) :
    ProcessModel {duration, id, processName(), parent}
{
    // Named shall be enough ?
    m_curve = new CurveModel{id_type<CurveModel>(45345), this};

    auto s1 = new LinearCurveSegmentModel(id_type<CurveSegmentModel>(1), m_curve);
    s1->setStart({0., 0.0});
    s1->setEnd({0.2, 1.});

    auto s2 = new GammaCurveSegmentModel(id_type<CurveSegmentModel>(2), m_curve);
    s2->setStart({0.2, 0.9});
    s2->setEnd({0.4, 0.5});
    s2->setPrevious(s1->id());
    s1->setFollowing(s2->id());

    auto s3 = new GammaCurveSegmentModel(id_type<CurveSegmentModel>(3), m_curve);
    s3->setStart({0.4, 0.5});
    s3->setEnd({0.6, 1.0});
    s3->setPrevious(s2->id());
    s2->setFollowing(s3->id());
    s3->gamma = 10;


    auto s4 = new SinCurveSegmentModel(id_type<CurveSegmentModel>(4), m_curve);
    s4->setStart({0.7, 0.0});
    s4->setEnd({1.0, 1.});

    m_curve->addSegment(s1);
    m_curve->addSegment(s2);
    m_curve->addSegment(s3);
    m_curve->addSegment(s4);
}

ProcessModel* AutomationModel::clone(
        const id_type<ProcessModel>& newId,
        QObject* newParent)
{
    auto autom = new AutomationModel {this->duration(), newId, newParent};
    autom->setAddress(address());
    autom->setMin(min());
    autom->setMax(max());
    autom->setCurve(m_curve->clone(m_curve->id(), autom));

    return autom;
}

QString AutomationModel::processName() const
{
    return "Automation";
}

void AutomationModel::setDurationAndScale(const TimeValue& newDuration)
{
    // We only need to change the duration.
    setDuration(newDuration);
}

void AutomationModel::setDurationAndGrow(const TimeValue& newDuration)
{
    // If there are no segments, nothing changes
    if(m_curve->segments().size() == 0)
    {
        setDuration(newDuration);
        return;
    }

    // Else, scale all the segments by the increase.
    double scale = duration() / newDuration;
    for(auto& segment : m_curve->segments())
    {
        CurvePoint pt = segment->start();
        pt.setX(pt.x() * scale);
        segment->setStart(pt);

        pt = segment->end();
        pt.setX(pt.x() * scale);
        segment->setEnd(pt);
    }

    setDuration(newDuration);
}

void AutomationModel::setDurationAndShrink(const TimeValue& newDuration)
{
    // If there are no segments, nothing changes
    if(m_curve->segments().size() == 0)
    {
        setDuration(newDuration);
        return;
    }

    // Else, scale all the segments by the increase.
    double scale = duration() / newDuration;
    for(auto& segment : m_curve->segments())
    {
        CurvePoint pt = segment->start();
        pt.setX(pt.x() * scale);
        segment->setStart(pt);

        pt = segment->end();
        pt.setX(pt.x() * scale);
        segment->setEnd(pt);
    }

    // Since we shrink, scale > 1. so we have to cut.
    // Note:  this will certainly change how some functions do look.
    auto segments = m_curve->segments(); // Make a copy since we will change the map.
    for(auto& segment : segments)
    {
        if(segment->start().x() >= 1.)
        {
            // bye
            m_curve->removeSegment(segment);
        }
        else if(segment->end().x() >= 1.)
        {
            auto end = segment->end();
            end.setX(1.);
            segment->setEnd(end);
        }
    }

    setDuration(newDuration);
}

Selection AutomationModel::selectableChildren() const
{
    Selection s;
    for(auto& segment : m_curve->segments())
        s.insert(segment);
    for(auto& point : m_curve->points())
        s.insert(point);
    return s;
}

Selection AutomationModel::selectedChildren() const
{
    return m_curve->selectedChildren();
}

void AutomationModel::setSelection(const Selection & s) const
{
    m_curve->setSelection(s);
}

ProcessViewModel* AutomationModel::makeViewModel_impl(
        const id_type<ProcessViewModel>& viewModelId,
        const QByteArray& constructionData,
        QObject* parent)
{
    auto vm = new AutomationViewModel{*this, viewModelId, parent};
    return vm;
}

ProcessViewModel* AutomationModel::cloneViewModel_impl(
        const id_type<ProcessViewModel>& newId,
        const ProcessViewModel& source,
        QObject* parent)
{
    auto vm = new AutomationViewModel {
              static_cast<const AutomationViewModel&>(source), *this, newId, parent};
    return vm;
}

void AutomationModel::setCurve(CurveModel* newCurve)
{
    delete m_curve;
    m_curve = newCurve;

    emit curveChanged();
}


// TODO fix memory leak
ProcessStateDataInterface* AutomationModel::startState() const
{
    return new AutomationState{this, 0.};
}

ProcessStateDataInterface* AutomationModel::endState() const
{
    return new AutomationState{this, 1.};
}

iscore::Address AutomationModel::address() const
{
    return m_address;
}

double AutomationModel::value(const TimeValue& time)
{
    return -1;
}

double AutomationModel::min() const
{
    return m_min;
}

double AutomationModel::max() const
{
    return m_max;
}

void AutomationModel::setAddress(const iscore::Address &arg)
{
    if(m_address == arg)
    {
        return;
    }

    m_address = arg;
    emit addressChanged(arg);
}

void AutomationModel::setMin(double arg)
{
    if (m_min == arg)
        return;

    m_min = arg;
    emit minChanged(arg);
}

void AutomationModel::setMax(double arg)
{
    if (m_max == arg)
        return;

    m_max = arg;
    emit maxChanged(arg);
}
