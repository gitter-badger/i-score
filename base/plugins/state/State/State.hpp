#pragma once
#include <QVariant>
#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/serialization/JSONVisitor.hpp>

#include <State/StateInterface.hpp>

class State
{       friend
        void Visitor<Reader<DataStream>>::readFrom<State>(const State& mess);
        friend
        void Visitor<Reader<JSON>>::readFrom<State>(const State& mess);
        friend
        void Visitor<Writer<DataStream>>::writeTo<State>(State& mess);
        friend
        void Visitor<Writer<JSON>>::writeTo<State>(State& mess);

    public:
        State() = default;
        ~State() = default;
        State(const State&) = default;
        State(State&&) = default;
        State& operator=(const State&) = default;
        State& operator=(State&&) = default;

        template<typename T>
        State(T&& t):
            m_data{QVariant::fromValue(t)}
        {

        }

        State(QVariant variant):
            m_data{variant}
        {
        }

        const QVariant& data() const
        {
            return m_data;
        }

        bool operator==(const State& s) const
        {
            return m_data == s.m_data;
        }

    private:
        QVariant m_data;
};

using StateList = QList<State>;
Q_DECLARE_METATYPE(State)
Q_DECLARE_METATYPE(StateList)
