#pragma once
#include <State/Address.hpp>
#include <QValidator>

class AddressValidator : public QValidator
{
    public:
        State validate(QString& s, int& pos) const
        {
            // TODO use the middle state for when the
            // device / address is valid but not in the tree.
            return iscore::Address::validateString(s)
                    ? State::Acceptable
                    : State::Invalid;
        }
};
