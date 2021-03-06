#pragma once
#include <iscore/plugins/plugincontrol/PluginControlInterface.hpp>

class DeviceExplorerControl : public iscore::PluginControlInterface
{
    public:
        DeviceExplorerControl(iscore::Presenter*);

        virtual iscore::DocumentDelegatePluginModel* loadDocumentPlugin(
                const QString& name,
                const VisitorVariant& var,
                iscore::DocumentModel *parent);

        iscore::SerializableCommand*
        instantiateUndoCommand(const QString & name,
                               const QByteArray & arr) override;

    protected:
        void on_newDocument(iscore::Document* doc) override;
        void on_documentChanged() override;

    private:
        void setupCommands();
};
