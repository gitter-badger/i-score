#include "CurveTool.hpp"
#include "Curve/StateMachine/CurveStateMachine.hpp"


CurveTool::CurveTool(const CurveStateMachine& csm, QState* parent):
    ToolState{csm.scene(), parent},
    m_parentSM{csm}
{

}
