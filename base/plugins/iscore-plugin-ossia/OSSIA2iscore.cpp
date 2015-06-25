#include "OSSIA2iscore.hpp"

namespace OSSIA
{
namespace convert
{
IOType ToIOType(OSSIA::Address::AccessMode t)
{
    switch(t)
    {
        case OSSIA::Address::AccessMode::GET:
            return IOType::In;
        case OSSIA::Address::AccessMode::SET:
            return IOType::Out;
        case OSSIA::Address::AccessMode::BI:
            return IOType::InOut;
        default:
            Q_ASSERT(false);
            return IOType::Invalid;
    }
}


ClipMode ToClipMode(OSSIA::Address::BoundingMode b)
{
    switch(b)
    {
        case OSSIA::Address::BoundingMode::CLIP:
            return ClipMode::Clip;
            break;
        case OSSIA::Address::BoundingMode::FOLD:
            return ClipMode::Fold;
            break;
        case OSSIA::Address::BoundingMode::FREE:
            return ClipMode::Free;
            break;
        case OSSIA::Address::BoundingMode::WRAP:
            return ClipMode::Wrap;
            break;
        default:
            Q_ASSERT(false);
            return static_cast<ClipMode>(-1);
    }
}


QVariant ToVariant(const OSSIA::Value *val)
{
    QVariant v;
    switch(val->getType())
    {
        case OSSIA::Value::Type::IMPULSE:
            break;
        case OSSIA::Value::Type::BOOL:
            v = dynamic_cast<const OSSIA::Bool*>(val)->value;
            break;
        case OSSIA::Value::Type::INT:
            v = dynamic_cast<const OSSIA::Int*>(val)->value;
            break;
        case OSSIA::Value::Type::FLOAT:
            v= dynamic_cast<const OSSIA::Float*>(val)->value;
            break;
        case OSSIA::Value::Type::CHAR:
            v = dynamic_cast<const OSSIA::Char*>(val)->value;
            break;
        case OSSIA::Value::Type::STRING:
            v = QString::fromStdString(dynamic_cast<const OSSIA::String*>(val)->value);
            break;
        case OSSIA::Value::Type::TUPLE:
        {
            QVariantList tuple;
            for (const auto & e : dynamic_cast<const OSSIA::Tuple*>(val)->value)
            {
                tuple.append(ToVariant(e));
            }

            v = tuple;
            break;
        }
        case OSSIA::Value::Type::GENERIC:
        {
            auto generic = dynamic_cast<const OSSIA::Generic*>(val);
            v = QByteArray{generic->start, generic->size};
            break;
        }
        default:
            break;
    }

    return v;
}


AddressSettings ToAddressSettings(const OSSIA::Node &node)
{
    AddressSettings s;
    const auto& addr = node.getAddress();
    s.name = QString::fromStdString(node.getName());

    if(addr)
    {
        s.value = ToVariant(addr->getValue());
        s.ioType = ToIOType(addr->getAccessMode());
        s.clipMode = ToClipMode(addr->getBoundingMode());
        s.repetitionFilter = addr->getRepetitionFilter();

        if(auto& domain = addr->getDomain())
            s.domain = ToDomain(*domain);
    }
    return s;
}


iscore::Node* ToDeviceExplorer(const OSSIA::Node &node)
{
    iscore::Node* n = new iscore::Node{ToAddressSettings(node)};

    // 2. Recurse on the children
    for(const auto& ossia_child : node.children())
    {
        n->addChild(ToDeviceExplorer(*ossia_child.get()));
    }

    return n;
}


iscore::Domain ToDomain(OSSIA::Domain &domain)
{
    iscore::Domain d;
    d.min = ToVariant(domain.getMin());
    d.max = ToVariant(domain.getMax());

    for(const auto& val : domain.getValues())
    {
        d.values.append(ToVariant(val));
    }

    return d;
}
}
}
