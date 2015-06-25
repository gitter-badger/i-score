#include "iscore2OSSIA.hpp"

using namespace boost;
using namespace mpl;
#include <boost/mpl/pair.hpp>
#include <boost/mpl/map.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/at.hpp>

namespace iscore
{
namespace convert
{
OSSIA::Node *createNodeFromPath(const QStringList &path, OSSIA::Device *dev)
{
    using namespace OSSIA;
    // Find the relevant node to add in the device
    OSSIA::Node* node = dev;
    for(int i = 0; i < path.size(); i++)
    {
        const auto& children = node->children();
        auto it = boost::range::find_if(
                      children,
                      [&] (const auto& ossia_node) { return ossia_node->getName() == path[i].toStdString(); });
        if(it == children.end())
        {
            // We have to start adding sub-nodes from here.
            OSSIA::Node* parentnode = node;
            for(int k = i; k < path.size(); k++)
            {
                auto newNodeIt = parentnode->emplace(parentnode->children().begin(), path[k].toStdString());
                if(k == path.size() - 1)
                {
                    node = newNodeIt->get();
                }
                else
                {
                    parentnode = newNodeIt->get();
                }
            }

            break;
        }
        else
        {
            node = it->get();
        }
    }

    return node;
}


OSSIA::Node *getNodeFromPath(const QStringList &path, OSSIA::Device *dev)
{
    using namespace OSSIA;
    // Find the relevant node to add in the device
    OSSIA::Node* node = dev;
    for(int i = 0; i < path.size(); i++)
    {
        const auto& children = node->children();
        auto it = boost::range::find_if(children,
                      [&] (const auto& ossia_node)
                    { return ossia_node->getName() == path[i].toStdString(); });
        Q_ASSERT(it != children.end());

        node = it->get();
    }

    Q_ASSERT(node);
    return node;
}


void updateOSSIAAddress(const FullAddressSettings &settings, const std::shared_ptr<OSSIA::Address> &addr)
{
    using namespace OSSIA;
    switch(settings.ioType)
    {
        case IOType::In:
            addr->setAccessMode(OSSIA::Address::AccessMode::GET);
            break;
        case IOType::Out:
            addr->setAccessMode(OSSIA::Address::AccessMode::SET);
            break;
        case IOType::InOut:
            addr->setAccessMode(OSSIA::Address::AccessMode::BI);
            break;
        case IOType::Invalid:
            // TODO There shouldn't be an address!!
            break;
    }
}


void createOSSIAAddress(const FullAddressSettings &settings, OSSIA::Node *node)
{
    using namespace OSSIA;
    std::shared_ptr<OSSIA::Address> addr;

    // Read the Qt docs on QVariant::type for the relationship with QMetaType::Type
    QMetaType::Type t = QMetaType::Type(settings.value.type());

    if(t == QMetaType::Float)
    { addr = node->createAddress(Value::Type::FLOAT); }

    else if(t == QMetaType::Int)
    { addr = node->createAddress(Value::Type::INT); }

    else if(t == QMetaType::QString)
    { addr = node->createAddress(Value::Type::STRING); }

    else if(t == QMetaType::Bool)
    { addr = node->createAddress(Value::Type::BOOL); }

    else if(t == QMetaType::Char)
    { addr = node->createAddress(Value::Type::CHAR); }

    else if(t == QMetaType::QVariantList)
    { addr = node->createAddress(Value::Type::TUPLE); }

    else if(t == QMetaType::QByteArray)
    { addr = node->createAddress(Value::Type::GENERIC); }

    updateOSSIAAddress(settings, addr);
}

void updateOSSIAValue(const QVariant& data, OSSIA::Value& val)
{
    switch(val.getType())
    {
        case OSSIA::Value::Type::IMPULSE:
            break;
        case OSSIA::Value::Type::BOOL:
            dynamic_cast<OSSIA::Bool&>(val).value = data.value<bool>();
            break;
        case OSSIA::Value::Type::INT:
            dynamic_cast<OSSIA::Int&>(val).value = data.value<int>();
            break;
        case OSSIA::Value::Type::FLOAT:
            dynamic_cast<OSSIA::Float&>(val).value = data.value<float>();
            break;
        case OSSIA::Value::Type::CHAR:
            dynamic_cast<OSSIA::Char&>(val).value = data.value<char>();
            break;
        case OSSIA::Value::Type::STRING:
            dynamic_cast<OSSIA::String&>(val).value = data.value<QString>().toStdString();
            break;
        case OSSIA::Value::Type::TUPLE:
        {
            QVariantList tuple = data.value<QVariantList>();
            const auto& vec = dynamic_cast<OSSIA::Tuple&>(val).value;
            Q_ASSERT(tuple.size() == (int)vec.size());
            for(int i = 0; i < (int)vec.size(); i++)
            {
                updateOSSIAValue(tuple[i], *vec[i]);
            }

            break;
        }
        case OSSIA::Value::Type::GENERIC:
        {
            const auto& array = data.value<QByteArray>();
            auto& generic = dynamic_cast<OSSIA::Generic&>(val);

            generic.size = array.size();

            delete generic.start;
            generic.start = new char[generic.size];

            boost::range::copy(array, generic.start);
            break;
        }
        default:
            break;
    }
}


using OSSIATypeMap =
mpl::map<
mpl::pair<bool, OSSIA::Bool>,
mpl::pair<int, OSSIA::Int>,
mpl::pair<float, OSSIA::Float>,
mpl::pair<char, OSSIA::Char>,
mpl::pair<std::string, OSSIA::String>
>;
/*
using OSSIATypeEnumToTypeMap =
mpl::map<
mpl::pair<mpl::int_<(int)OSSIA::Value::Type::BOOL>, bool>,
mpl::pair<mpl::int_<(int)OSSIA::Value::Type::INT>, int>,
mpl::pair<mpl::int_<(int)OSSIA::Value::Type::FLOAT>, float>,
mpl::pair<mpl::int_<(int)OSSIA::Value::Type::CHAR>, char>,
mpl::pair<mpl::int_<(int)OSSIA::Value::Type::STRING>, std::string>
>;
*/

template<typename T>
OSSIA::Value* createOSSIAValue(const T& val)
{
    return new typename mpl::at<OSSIATypeMap, T>::type(val);
}


OSSIA::Value* toValue(
        const QVariant& val)
{
    switch(QMetaType::Type(val.type()))
    {
        case QVariant::Type::Invalid:
            return new OSSIA::Impulse;
        case QMetaType::Type::Bool:
            return createOSSIAValue(val.value<bool>());
        case QMetaType::Type::Int:
            return createOSSIAValue(val.value<int>());
        case QMetaType::Type::Float:
            return createOSSIAValue(val.value<float>());
        case QMetaType::Type::Char:
            return createOSSIAValue(val.value<char>());
        case QMetaType::Type::QString:
            return createOSSIAValue(val.value<QString>().toStdString());
            //TODO tuple & generic
            /*
        case OSSIA::Value::Type::TUPLE:
        {
            QVariantList tuple = data.value<QVariantList>();
            const auto& vec = dynamic_cast<OSSIA::Tuple&>(val).value;
            Q_ASSERT(tuple.size() == (int)vec.size());
            for(int i = 0; i < (int)vec.size(); i++)
            {
                updateOSSIAValue(tuple[i], *vec[i]);
            }

            break;
        }
        case OSSIA::Value::Type::GENERIC:
        {
            const auto& array = data.value<QByteArray>();
            auto& generic = dynamic_cast<OSSIA::Generic&>(val);

            generic.size = array.size();

            delete generic.start;
            generic.start = new char[generic.size];

            boost::range::copy(array, generic.start);
            break;
        }
        */
        default:
            break;
    }

    Q_ASSERT(false);
    return nullptr;
}

}
}
