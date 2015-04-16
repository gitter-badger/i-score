#pragma once
#include <iscore/serialization/VisitorInterface.hpp>
#include <cstdint>
#include <utility>
#include <QVector>
#include <QMetaType>

class DataStream;
class JSON;
using class_id_type = int32_t;
/**
 * @brief The ObjectIdentifier class
 *
 * A mean to identify an object without a pointer. The id is useful
 * if the object is inside a collection.
 *
 * Example:
 * @code
 * TODO
 * @endcode
 */
class ObjectIdentifier
{
        friend Serializer<DataStream>;
        friend Serializer<JSON>;
        friend Deserializer<DataStream>;
        friend Deserializer<JSON>;

        friend bool operator== (const ObjectIdentifier& lhs, const ObjectIdentifier& rhs)
        {
            return (lhs.m_idInParent == rhs.m_idInParent) && (lhs.m_id == rhs.m_id);
        }
    public:
        ObjectIdentifier() = default;
        ObjectIdentifier(int32_t type) :
            m_idInParent {type}
        { }

        ObjectIdentifier(int32_t type, int32_t id) :
            m_idInParent{std::move(type)},
                     m_id{std::move(id)}
        { }

        template<typename T>
        ObjectIdentifier(int32_t type, const T& id) :
            m_idInParent {std::move(type) }
        {
            if(id.val())
            {
                m_id = *id.val().get();
            }
        }

        const int32_t& idInParent() const
        {
            return m_idInParent;
        }

        const int32_t& id() const
        {
            return m_id;
        }

    private:
        int32_t m_idInParent;
        int32_t m_id;
};

Q_DECLARE_METATYPE(ObjectIdentifier)

using ObjectIdentifierVector = QVector<ObjectIdentifier> ;

