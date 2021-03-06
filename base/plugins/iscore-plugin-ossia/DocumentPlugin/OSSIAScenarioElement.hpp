#pragma once
#include <iscore/tools/SettableIdentifier.hpp>
#include <iscore/tools/IdentifiedObjectMap.hpp>
#include <memory>

#include <Process/ScenarioModel.hpp>

#include "OSSIAProcessElement.hpp"
#include "OSSIAConstraintElement.hpp"
#include "OSSIATimeNodeElement.hpp"
#include "OSSIAStateElement.hpp"
#include "OSSIAEventElement.hpp"
#include <QPointer>
class EventModel;
class ConstraintModel;
class TimeNodeModel;
class ScenarioModel;
class DeviceList;
class OSSIAConstraintElement;

namespace OSSIA
{
    class Scenario;
}

class OSSIAScenarioElement : public OSSIAProcessElement
{
    public:
        OSSIAScenarioElement(
                OSSIAConstraintElement* parentConstraint,
                const ScenarioModel* element,
                QObject* parent);
        ~OSSIAScenarioElement();

        std::shared_ptr<OSSIA::TimeProcess> process() const override;
        std::shared_ptr<OSSIA::Scenario> scenario() const;

        const auto& states() const
        { return m_ossia_states; }

        const ProcessModel* iscoreProcess() const override;

        void stop() override;

    private:
        void on_constraintCreated(const id_type<ConstraintModel>& id);
        void on_stateCreated(const id_type<StateModel>& id);
        void on_eventCreated(const id_type<EventModel>& id);
        void on_timeNodeCreated(const id_type<TimeNodeModel>& id);

        void on_constraintRemoved(const id_type<ConstraintModel>& id);
        void on_stateRemoved(const id_type<StateModel>& id);
        void on_eventRemoved(const id_type<EventModel>& id);
        void on_timeNodeRemoved(const id_type<TimeNodeModel>& id);


        QPointer<OSSIAConstraintElement> m_parent_constraint;

        std::map<id_type<ConstraintModel>, OSSIAConstraintElement*> m_ossia_constraints;
        std::map<id_type<StateModel>, OSSIAStateElement*> m_ossia_states;
        std::map<id_type<TimeNodeModel>, OSSIATimeNodeElement*> m_ossia_timenodes;
        std::map<id_type<EventModel>, OSSIAEventElement*> m_ossia_timeevents;
        std::shared_ptr<OSSIA::Scenario> m_ossia_scenario;
        const ScenarioModel* m_iscore_scenario{};

        IdContainer<ConstraintModel> m_executingConstraints;

        TimeValue m_previousExecutionDate;

        const DeviceList& m_deviceList;
};
