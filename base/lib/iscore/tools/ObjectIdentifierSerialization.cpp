#include "iscore/serialization/DataStreamVisitor.hpp"
#include "iscore/serialization/JSONVisitor.hpp"
#include "ObjectIdentifier.hpp"

template<>
void Visitor<Reader<DataStream>>::readFrom(const ObjectIdentifier& obj)
{
    m_stream << obj.m_objectType << obj.m_id;
}

template<>
void Visitor<Writer<DataStream>>::writeTo(ObjectIdentifier& obj)
{
    m_stream >> obj.m_objectType >> obj.m_id;
}

template<>
void Visitor<Reader<JSON>>::readFrom(const ObjectIdentifier& obj)
{
    m_obj["ObjectName"] = obj.m_objectType;
    m_obj["ObjectId"] = obj.m_id;
}

template<>
void Visitor<Writer<JSON>>::writeTo(ObjectIdentifier& obj)
{
    obj.m_objectType = m_obj["ObjectName"].toInt();
    obj.m_id = m_obj["ObjectId"].toInt();
}
