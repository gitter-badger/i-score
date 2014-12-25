#pragma once
#include <core/presenter/command/SerializableCommand.hpp>
#include <tools/ObjectPath.hpp>

#include <tests/helpers/ForwardDeclaration.hpp>
namespace Scenario
{
	namespace Command
	{
		/**
		 * @brief The AddProcessViewToDeck class
		 *
		 * Adds a process view to a deck.
		 */
		class AddProcessViewModelToDeck : public iscore::SerializableCommand
		{
#include <tests/helpers/FriendDeclaration.hpp>
			public:
				AddProcessViewModelToDeck(ObjectPath&& deckPath, int sharedModelId);

				virtual void undo() override;
				virtual void redo() override;
				virtual int id() const override;
				virtual bool mergeWith(const QUndoCommand* other) override;

			protected:
				virtual void serializeImpl(QDataStream&) override;
				virtual void deserializeImpl(QDataStream&) override;

			private:
				ObjectPath m_path;

				int m_sharedModelId{};
				int m_createdProcessViewId{};
		};
	}
}