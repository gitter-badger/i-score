#include "ChangeGroup.hpp"
#include "source/Document/Constraint/ConstraintModel.hpp"
#include "source/Document/Event/EventModel.hpp"
#include <boost/range/algorithm/find_if.hpp>
GroupMetadata* getGroupMetadata(QObject* obj)
{
    using namespace boost::range;
    if(auto cstr = dynamic_cast<ConstraintModel*>(obj))
    {
        auto& plugs = cstr->pluginModelList.list();
        auto plug_it = find_if(plugs, [] (iscore::ElementPluginModel* elt)
        { return elt->metaObject()->className() == QString{"GroupMetadata"}; });
        Q_ASSERT(plug_it != plugs.end());

        return static_cast<GroupMetadata*>(*plug_it);
    }
    else if(auto ev = dynamic_cast<EventModel*>(obj))
    {
        auto& plugs = ev->pluginModelList.list();
        auto plug_it = find_if(plugs, [] (iscore::ElementPluginModel* elt)
        { return elt->metaObject()->className() == QString{"GroupMetadata"}; });
        Q_ASSERT(plug_it != plugs.end());

        return static_cast<GroupMetadata*>(*plug_it);
    }

    Q_ASSERT(false);
    return nullptr;
}

ChangeGroup::ChangeGroup(ObjectPath &&path, id_type<Group> newGroup):
    iscore::SerializableCommand{
        "NetworkControl",
        this->commandName(),
        this->description()},
    m_path{path},
    m_newGroup{newGroup}
{
    m_oldGroup = getGroupMetadata(&m_path.find<QObject>())->group();
}

void ChangeGroup::undo()
{
    getGroupMetadata(&m_path.find<QObject>())->setGroup(m_oldGroup);
}

void ChangeGroup::redo()
{
    getGroupMetadata(&m_path.find<QObject>())->setGroup(m_newGroup);
}

void ChangeGroup::serializeImpl(QDataStream &s) const
{
    s << m_path << m_newGroup << m_oldGroup;
}

void ChangeGroup::deserializeImpl(QDataStream &s)
{
    s >> m_path >> m_newGroup >> m_oldGroup;
}
