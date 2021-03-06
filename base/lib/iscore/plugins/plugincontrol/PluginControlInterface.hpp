#pragma once
#include <iscore/tools/NamedObject.hpp>
#include <core/presenter/Presenter.hpp>
#include <iscore/widgets/OrderedToolbar.hpp>
class QToolBar;

namespace iscore
{
    class DocumentDelegatePluginModel;
    class SerializableCommand;
    class Presenter;
    class MenubarManager;

    /**
     * @brief The PluginControlInterface class
     *
     * This class's goal is to :
     * * Instantiate some elements that are deeply intertwined with Qt : menus, toolbars
     * * Manage the Commands of the plug-in : it has to be able to instantiate any meaningful
     *   Command, if received by the network.
     *
     * It is instatiated exactly once by the Presenter class in i-score.
     */
    class PluginControlInterface : public NamedObject
    {
            // Menus : trouver un moyen pour créer automatiquement si n'existe pas ?
            // Dire chemin : File/Export/SomeCustomExport ?
            // Pb. : traduction ? (ex. : soft traduit & plug pas traduit ?)
            // Fournir menus de base : Fichier Edition Affichage Objet Arrangement Devices Fenêtre Paramètres Aide
        Q_OBJECT
        public:
            PluginControlInterface(iscore::Presenter* presenter,
                                   const QString& name,
                                   QObject* parent);

            virtual void populateMenus(iscore::MenubarManager*);
            virtual QList<OrderedToolbar> makeToolbars();

            virtual DocumentDelegatePluginModel* loadDocumentPlugin(
                    const QString& name,
                    const VisitorVariant& var,
                    iscore::DocumentModel *parent);

            Presenter* presenter() const;

            // A generic method to deserialize commands. Examples in the plug-ins.
            template<typename CommandFactory>
            SerializableCommand* instantiateUndoCommand(
                    const QString& name,
                    const QByteArray& data)
            {
                auto it = CommandFactory::map.find(name);
                if(it != CommandFactory::map.end())
                {
                    iscore::SerializableCommand* cmd = (*(*it).second)();
                    cmd->deserialize(data);
                    return cmd;
                }
                else
                {
                    qDebug() << Q_FUNC_INFO << "Warning : command" << name << "received, but it could not be read. Aborting.";
                    Q_ASSERT(false);

                    return nullptr;
                }
            }

            virtual SerializableCommand* instantiateUndoCommand(
                    const QString& /*name*/,
                    const QByteArray& /*data*/);

            Document* currentDocument() const;

            virtual void on_newDocument(iscore::Document* doc);
            virtual void on_loadedDocument(iscore::Document* doc);

        signals:
            void documentChanged();

        protected:
            virtual void on_documentChanged();

        private:
            Presenter* m_presenter{};
    };

}
