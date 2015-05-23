
#include "Remove.hpp"

#include "Panel/BinaryReader.hpp"
#include "Panel/BinaryWriter.hpp"

using namespace DeviceExplorer::Command;

const char* Remove::className() { return "Remove"; }
QString Remove::description() { return "Remove Node"; }

Remove::Remove(ObjectPath &&device_tree, Path nodePath):
    iscore::SerializableCommand{"DeviceExplorerControl",
                            className(),
                            description()},
    m_deviceTree{device_tree},
    m_nodePath{nodePath},
    m_nodeIndex{m_nodePath.back()}
{
    auto& explorer = m_deviceTree.find<DeviceExplorerModel>();
    Node *node = m_nodePath.toNode(explorer.rootNode());

    if (! node->isDevice())
    {
        m_parentPath = Path{node->parent()};
    }
    else
    {
        m_parentPath.clear();
    }
    m_addressSettings = node->addressSettings();
}

void
Remove::undo()
{
    auto& explorer = m_deviceTree.find<DeviceExplorerModel>();
    explorer.addAddress(m_parentPath.toNode(explorer.rootNode()), m_node, m_nodeIndex);
}

void
Remove::redo()
{

    auto& explorer = m_deviceTree.find<DeviceExplorerModel>();
    Node *node = m_nodePath.toNode(explorer.rootNode());
    m_node = node->clone();
    Node* parent = m_parentPath.toNode(explorer.rootNode());

    explorer.removeNode(parent->childAt(m_nodeIndex));
}

void
Remove::serializeImpl(QDataStream& d) const
{
    d << m_deviceTree;
    m_parentPath.serializePath(d);
    m_nodePath.serializePath(d);

    d << m_nodeIndex;
    BinaryWriter bw{d};
    bw.write(m_node);
}

void
Remove::deserializeImpl(QDataStream& d)
{
    d >> m_deviceTree;
    m_parentPath.deserializePath(d);
    m_nodePath.deserializePath(d);

    d >> m_nodeIndex;

    BinaryReader br{d};
    br.read(m_node->parent());
}