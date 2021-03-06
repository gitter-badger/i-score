#pragma once
#include <QVariant>
#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/serialization/JSONVisitor.hpp>

namespace iscore {
class State
{       friend
        void Visitor<Reader<DataStream>>::readFrom<iscore::State>(const iscore::State& mess);
        friend
        void Visitor<Reader<JSONObject>>::readFrom<iscore::State>(const iscore::State& mess);
        friend
        void Visitor<Writer<DataStream>>::writeTo<iscore::State>(iscore::State& mess);
        friend
        void Visitor<Writer<JSONObject>>::writeTo<iscore::State>(iscore::State& mess);

    public:
        State() = default;
        ~State() = default;
        State(const State&) = default;
        State(State&&) = default;
        State& operator=(const State&) = default;
        State& operator=(State&&) = default;

        template<typename T>
        explicit State(const T& t):
            m_data{QVariant::fromValue(t)}
        {

        }

        State(const QVariant& variant):
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

        bool operator!=(const State& s) const
        {
            return m_data != s.m_data;
        }

    private:
        QVariant m_data;
};

using StateList = QList<State>;
}
Q_DECLARE_METATYPE(iscore::State)
Q_DECLARE_METATYPE(iscore::StateList)


std::size_t hash_value(const iscore::State& state) noexcept;

struct state_hash
{
    std::size_t operator()(const iscore::State& state) const noexcept
    {
        return hash_value(state);
    }
};
