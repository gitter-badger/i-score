#pragma once
#include <boost/optional.hpp>
#include <QDebug>

template<typename tag, typename impl>
class id
{
    public:
        using value_type = impl;
        explicit id() = default;
        explicit id(value_type val) : m_id {val} { }

        friend bool operator== (const id& lhs, const id& rhs)
        {
            return lhs.m_id == rhs.m_id;
        }

        friend bool operator!= (const id& lhs, const id& rhs)
        {
            return lhs.m_id != rhs.m_id;
        }

        friend bool operator< (const id& lhs, const id& rhs)
        {
            return *lhs.val() < *rhs.val();
        }

        explicit operator bool() const
        {
            return bool(m_id);
        }

        explicit operator value_type() const
        {
            return m_id;
        }

        const value_type& val() const
        {
            return m_id;
        }

        void setVal(value_type&& val)
        {
            m_id = val;
        }

        void unset()
        {
            m_id = value_type();
        }

    private:
        value_type m_id {};
};

template<typename tag, typename impl>
using optional_tagged_id = id<tag, boost::optional<impl>>;

template<typename tag>
using optional_tagged_int32_id = optional_tagged_id<tag, int32_t>;

template<typename tag>
using id_type = optional_tagged_int32_id<tag>;

template<typename tag>
struct id_hash
{
    std::size_t operator()(const id_type<tag>& id) const
    {
        return std::hash<int32_t>()(*id.val());
    }
};

template<typename T>
uint qHash(const id_type<T>& id, uint seed)
{
    return qHash(id.val().get(), seed);
}

template<typename tag>
QDebug operator<< (QDebug dbg, const id_type<tag>& c)
{
    if(c.val())
    {
        dbg.nospace() << *c.val();
    }
    else
    {
        dbg.nospace() << "Not set";
    }

    return dbg.space();
}
