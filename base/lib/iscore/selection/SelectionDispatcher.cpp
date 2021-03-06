#include "SelectionDispatcher.hpp"
#include "SelectionStack.hpp"

using namespace iscore;

void SelectionDispatcher::setAndCommit(const Selection& s)
{
    m_stack.pushNewSelection(s);
}

iscore::SelectionStack &SelectionDispatcher::stack() const
{
    return m_stack;
}
