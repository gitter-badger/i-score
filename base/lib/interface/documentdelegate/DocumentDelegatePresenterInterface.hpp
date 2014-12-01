#pragma once
#include <QObject>
#include <core/document/DocumentPresenter.hpp>

namespace iscore
{
	class DocumentPresenter;
	class DocumentDelegateModelInterface;
	class DocumentDelegateViewInterface;
	class SerializableCommand;

	class DocumentDelegatePresenterInterface : public QNamedObject
	{
			Q_OBJECT
		public:
			DocumentDelegatePresenterInterface(DocumentPresenter* parent_presenter,
								   QString object_name,
								   DocumentDelegateModelInterface* model,
								   DocumentDelegateViewInterface* view):
				QNamedObject{parent_presenter, object_name},
				m_model{model},
				m_view{view},
				m_parentPresenter{parent_presenter}
			{

			}

			virtual ~DocumentDelegatePresenterInterface() = default;

			void setModel(DocumentDelegateModelInterface* m)
			{ m_model = m; }
			void setView(DocumentDelegateViewInterface* v)
			{ m_view = v; }

		signals:
			void submitCommand(iscore::SerializableCommand* cmd);

		protected:
			DocumentDelegateModelInterface* m_model;
			DocumentDelegateViewInterface* m_view;
			DocumentPresenter* m_parentPresenter;
	};
}