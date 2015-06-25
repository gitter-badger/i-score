#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/serialization/JSONVisitor.hpp>
#include "State.hpp"
#include "Message.hpp"

using namespace iscore;
template<>
void Visitor<Reader<DataStream>>::readFrom(const State& state)
{
    m_stream << state.m_data;
    insertDelimiter();
}

template<>
void Visitor<Reader<JSONObject>>::readFrom(const State& state)
{
    m_obj["Type"] = int(state.data().userType());
    if(state.data().canConvert<State>())
    {
        m_obj["Data"] = toJsonObject(state.data().value<State>());
    }
    else if(state.data().canConvert<StateList>())
    {
        m_obj["Data"] = toJsonArray(state.data().value<StateList>());
    }
    else if(state.data().canConvert<Message>())
    {
        m_obj["Data"] = toJsonObject(state.data().value<Message>());
    }
    else if(state.data().canConvert<MessageList>())
    {
        m_obj["Data"] = toJsonArray(state.data().value<MessageList>());
    }
    else
    {
        qDebug() << "TODO";
    }
}

template<>
void Visitor<Writer<DataStream>>::writeTo(State& state)
{
    m_stream >> state.m_data;

    checkDelimiter();
}

template<>
void Visitor<Writer<JSONObject>>::writeTo(State& state)
{
    QString t = QVariant::typeToName(m_obj["Type"].toInt());
    if("State" == t)
    {
        state = fromJsonObject<State>(m_obj["Data"].toObject());
    }
    else if("StateList" == t)
    {
        StateList t;
        fromJsonArray(m_obj["Data"].toArray(), t);
        state = t;
    }
    else if("Message" == t)
    {
        state = fromJsonObject<Message>(m_obj["Data"].toObject());
    }
    else if("MessageList" == t)
    {
        MessageList t;
        fromJsonArray(m_obj["Data"].toArray(), t);
        state = t;
    }
    else
    {
        qDebug() << Q_FUNC_INFO << "TODO";
    }
}

