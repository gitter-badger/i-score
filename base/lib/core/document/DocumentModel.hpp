#pragma once
#include <iscore/tools/IdentifiedObject.hpp>
#include <iscore/selection/Selection.hpp>
#include <set>

namespace iscore
{
    class DocumentDelegateFactoryInterface;
    class DocumentDelegateModelInterface;
    class DocumentDelegatePluginModel;
    class PanelModelInterface;
    /**
     * @brief The DocumentDelegateModelInterface class
     *
     * Drawbridge between the application and a model given by a plugin.
     */
    class DocumentModel : public IdentifiedObject<DocumentModel>
    {
            Q_OBJECT
        public:
            IdentifiedObjectAbstract* findIdentifiedChild(const ObjectIdentifier& idtf) const override
            {
                //TODO
                if(idtf.objectType() == 0 )
                {

                }
                return nullptr;

            }

            DocumentModel(DocumentDelegateFactoryInterface* fact,
                          QObject* parent);
            DocumentModel(QVariant data,
                          DocumentDelegateFactoryInterface* fact,
                          QObject* parent);

            DocumentDelegateModelInterface* modelDelegate() const
            {
                return m_model;
            }

            void addPanel(PanelModelInterface* m)
            {
                m_panelModels.append(m);
            }

            void addPluginModel(DocumentDelegatePluginModel* m)
            {
                m_pluginModels.append(m);
            }

            const QList<PanelModelInterface*>& panels() const
            {
                return m_panelModels;
            }

            // Returns a Panel by name.
            PanelModelInterface* panel(QString name) const;

            DocumentDelegatePluginModel* pluginModel(QString name) const;

        public slots:
            void setNewSelection(const Selection&);

        private:
            DocumentDelegateModelInterface* m_model{};
            QList<PanelModelInterface*> m_panelModels;
            QList<DocumentDelegatePluginModel*> m_pluginModels;
    };
}
