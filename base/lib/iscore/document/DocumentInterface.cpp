#include "DocumentInterface.hpp"
#include <core/document/Document.hpp>
#include <core/document/DocumentModel.hpp>
#include <core/document/DocumentPresenter.hpp>
#include <iscore/plugins/documentdelegate/DocumentDelegateModelInterface.hpp>
#include <iscore/plugins/panel/PanelModelInterface.hpp>

iscore::Document* iscore::IDocument::documentFromObject(const QObject* obj)
{
    QString objName {obj ? obj->objectName() : "INVALID"};

    while(obj && obj->objectName() != "Document")
    {
        obj = obj->parent();
    }

    if(!obj)
        throw std::runtime_error(
            QString("Object (name: %1) is not part of a Document!")
            .arg(objName)
            .toStdString());

    return static_cast<Document*>(const_cast<QObject*>(obj));
}


iscore::Document* iscore::IDocument::documentFromObject(const QObject& obj)
{
    return documentFromObject(&obj);
}

ObjectPath iscore::IDocument::path(const QObject* obj)
{
    auto doc = documentFromObject(obj);
    qDebug() << doc->objectName() << (void*) doc;
    qDebug() << "path" << (void*) doc->model();
    qDebug() << doc->model()->objectName();;

    return ObjectPath::pathBetweenObjects(documentFromObject(obj)->model(), obj);
}


iscore::DocumentDelegatePresenterInterface& iscore::IDocument::presenterDelegate_generic(const iscore::Document& d)
{
    return *d.presenter()->presenterDelegate();
}


iscore::DocumentDelegateModelInterface& iscore::IDocument::modelDelegate_generic(const Document& d)
{
    return *d.model()->modelDelegate();
}


const QList<iscore::PanelModelInterface*>& iscore::IDocument::panels(const iscore::Document* d)
{
    return d->model()->panels();
}


iscore::PanelModelInterface* iscore::IDocument::panel(const QString& name, const iscore::Document* d)
{
    using namespace std;
    auto panels = d->model()->panels();

    auto it = find_if(begin(panels), end(panels),
                      [name](PanelModelInterface* panel)
    {
              return panel->objectName() == name;
    });

    return (it != end(panels) ? *it : nullptr);
}


ObjectPath iscore::IDocument::path(const QObject &obj)
{
    return path(&obj);
}
