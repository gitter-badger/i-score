#pragma once
#include <core/presenter/MenubarManager.hpp>

#include <set>
#include <core/document/Document.hpp>

#include <QNamedObject>

namespace iscore
{
	class SerializableCommand;
	class PluginControlInterface;
	class Model;
	class View;
	class PanelFactoryInterface;
	class PanelPresenterInterface;
	/**
	 * @brief The Presenter class
	 *
	 * Certainly needs refactoring.
	 * For now, manages menus and plug-in objects.
	 *
	 * It is also able to instantiate a Command from serialized Undo/Redo data.
	 * (this should go in the DocumentPresenter maybe ?)
	 */
	class Presenter : public QNamedObject
	{
			Q_OBJECT
		public:
			Presenter(iscore::Model* model, iscore::View* view, QObject* parent);

			View* view() { return m_view; }
			Document* document() { return m_document; }

			void setupCommand(PluginControlInterface*);
			void addPanel(PanelFactoryInterface*);
			void setDocumentPanel(DocumentDelegateFactoryInterface*);

		signals:
			/**
			 * @brief instantiatedCommand Is emitted when a command was requested using Presenter::instantiateUndoCommand
			 */
			void instantiatedCommand(iscore::SerializableCommand*);

		public slots:
			/**
			 * @brief newDocument Create a new document.
			 */
			void newDocument();

			/**
			 * @brief applyCommand
			 *
			 * Forwards a command to the undo/redo stack
			 */
			void applyCommand(iscore::SerializableCommand*);

			/**
			 * @brief instantiateUndoCommand Is used to generate a Command from its serialized data.
			 * @param parent_name The name of the object able to generate the command. Must be a CustomCommand.
			 * @param name The name of the command to generate.
			 * @param data The data of the command.
			 */
			void instantiateUndoCommand(QString parent_name,
										QString name,
										QByteArray data);

		private:
			void setupMenus();

			Model* m_model;
			View* m_view;
			MenubarManager m_menubar;
			Document* m_document{};

			std::vector<PluginControlInterface*> m_customCommands;
			std::set<PanelPresenterInterface*> m_panelsPresenters;
	};
}