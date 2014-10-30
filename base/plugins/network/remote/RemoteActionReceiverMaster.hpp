#pragma once
#include "RemoteActionReceiver.hpp"

#include <API/Headers/Repartition/session/MasterSession.h>

class MasterSession;
class RemoteActionReceiverMaster : public RemoteActionReceiver
{
		Q_OBJECT
	public:
		RemoteActionReceiverMaster(QObject* parent, MasterSession*);

	protected:
		virtual void handle__edit_command(osc::ReceivedMessageArgumentStream args) override;
		virtual void handle__edit_undo(osc::ReceivedMessageArgumentStream) override;
		virtual void handle__edit_redo(osc::ReceivedMessageArgumentStream) override;
		virtual Session* session() override
		{ return m_session; }

	private:
		MasterSession* m_session;
};