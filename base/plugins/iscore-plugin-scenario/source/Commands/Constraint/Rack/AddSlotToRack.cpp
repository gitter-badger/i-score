#include "AddSlotToRack.hpp"

#include "Document/Constraint/Rack/RackModel.hpp"
#include "Document/Constraint/Rack/Slot/SlotModel.hpp"
#include <iscore/tools/SettableIdentifierGeneration.hpp>

using namespace iscore;
using namespace Scenario::Command;

AddSlotToRack::AddSlotToRack(ObjectPath&& rackPath) :
    SerializableCommand {"ScenarioControl",
                         commandName(),
                         description()},
    m_path {rackPath}
{
    auto& rack = m_path.find<RackModel>();
    m_createdSlotId = getStrongId(rack.getSlots());
}

void AddSlotToRack::undo()
{
    auto& rack = m_path.find<RackModel>();
    rack.removeSlot(m_createdSlotId);
}

void AddSlotToRack::redo()
{
    auto& rack = m_path.find<RackModel>();
    rack.addSlot(new SlotModel {m_createdSlotId,
                               &rack});
}

void AddSlotToRack::serializeImpl(QDataStream& s) const
{
    s << m_path << m_createdSlotId;
}

void AddSlotToRack::deserializeImpl(QDataStream& s)
{
    s >> m_path >> m_createdSlotId;
}
