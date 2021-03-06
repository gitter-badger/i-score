#pragma once
#include <iscore/plugins/documentdelegate/plugin/DocumentDelegatePluginModel.hpp>


#include "DistributedScenario/Group.hpp"
#include <iscore/serialization/VisitorCommon.hpp>

class NetworkControl;
class ClientSession;
class MasterSession;
class Session;
class GroupManager;

namespace iscore
{
    class Document;
    class DocumentModel;
}

class NetworkPluginPolicy : public QObject
{
    public:
        using QObject::QObject;
        virtual Session* session() const = 0;
};

class NetworkDocumentPlugin : public iscore::DocumentDelegatePluginModel
{
        Q_OBJECT

        // TODO make an ISCORE_SERIALIZE_FRIENDS macro
        friend void Visitor<Reader<DataStream>>::readFrom<NetworkDocumentPlugin>(const NetworkDocumentPlugin& ev);
        friend void Visitor<Reader<JSONObject>>::readFrom<NetworkDocumentPlugin>(const NetworkDocumentPlugin& ev);
        friend void Visitor<Writer<DataStream>>::writeTo<NetworkDocumentPlugin>(NetworkDocumentPlugin& ev);
        friend void Visitor<Writer<JSONObject>>::writeTo<NetworkDocumentPlugin>(NetworkDocumentPlugin& ev);
    public:
        NetworkDocumentPlugin(NetworkPluginPolicy* policy, iscore::DocumentModel *doc);

        // Loading has to be in two steps since the plugin policy is different from the client
        // and server.
        NetworkDocumentPlugin(const VisitorVariant& loader, iscore::DocumentModel* doc);
        void setPolicy(NetworkPluginPolicy*);

        QList<iscore::ElementPluginModelType> elementPlugins() const override;

        iscore::ElementPluginModel* makeElementPlugin(
                const QObject* element,
                iscore::ElementPluginModelType,
                QObject* parent) override;

        iscore::ElementPluginModel* loadElementPlugin(
                const QObject* element,
                const VisitorVariant&,
                QObject* parent) override;

        iscore::ElementPluginModel* cloneElementPlugin(
                const QObject* element,
                iscore::ElementPluginModel*,
                QObject* parent) override;

        virtual QWidget *makeElementPluginWidget(
                const iscore::ElementPluginModel*, QWidget* widg) const override;

        void serialize(const VisitorVariant&) const override;


        GroupManager* groupManager() const
        { return m_groups; }

        NetworkPluginPolicy* policy() const
        { return m_policy; }

    signals:
        void sessionChanged();

    private:
        void setupGroupPlugin(GroupMetadata* grp);

        NetworkPluginPolicy* m_policy{};
        GroupManager* m_groups{};

};

