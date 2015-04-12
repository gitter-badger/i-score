#include <QApplication>
#include <iscore/tools/IdentifiedObject.hpp>
#include <iscore/tools/ObjectPath.hpp>
#include <iscore/presenter/PresenterInterface.hpp>
#include <core/document/Document.hpp>
#include <core/document/DocumentModel.hpp>

ObjectPath ObjectPath::pathBetweenObjects(const QObject* const parent_obj, const QObject* target_object)
{
    QVector<ObjectIdentifier> v;

    auto current_obj = target_object;
    auto add_parent_to_vector = [&v](const QObject * ptr)
    {
        if(auto id_obj = dynamic_cast<const IdentifiedObjectAbstract*>(ptr))
            v.push_back({id_obj->class_id(), id_obj->id_val() });
        else
            v.push_back({id_obj->class_id(), {}});
    };

    QString debug_objectnames;
    // Recursively go through the object and all the parents
    while(current_obj != parent_obj)
    {
        debug_objectnames += current_obj->objectName() + "->";

        if(current_obj->objectName().isEmpty())
        {
            qDebug() << "Names: " << debug_objectnames;
            throw std::runtime_error("ObjectPath::pathFromObject : an object in the hierarchy does not have a name.");
        }

        add_parent_to_vector(current_obj);

        current_obj = current_obj->parent();

        if(!current_obj)
        {
            throw std::runtime_error("ObjectPath::pathFromObject : Could not find path to parent object");
        }
    }

    // Add the last parent (the one specified with parent_name)
    add_parent_to_vector(current_obj);

    // Search goes from top to bottom (of the parent hierarchy) instead
    std::reverse(std::begin(v), std::end(v));
    return std::move(v);
}

QString ObjectPath::toString() const
{
    QString s;

    for(auto& obj : m_objectIdentifiers)
    {
        s += obj.objectType();

        if(obj.id())
        {
            s += ".";
            s += QString::number(obj.id());
        }

        s += "/";
    }

    return s;
}

ObjectPath ObjectPath::pathFromObject(QString parent_name, QObject* target_object)
{
    QObject* parent_obj = qApp->findChild<QObject*> (parent_name);
    return ObjectPath::pathBetweenObjects(parent_obj, target_object);
}

ObjectPath ObjectPath::pathFromObject(QObject* origin_object)
{
    auto path = ObjectPath::pathBetweenObjects(qApp, origin_object);
    path.m_objectIdentifiers.removeFirst();
    return path;
}

QObject* ObjectPath::find_impl() const
{
    using namespace std;
    auto docs = iscore::IPresenter::documents();
    // 1. Find the correct DocumentModel.
    auto doc_it = std::find_if(begin(docs),
                               end(docs),
                               [theId = m_objectIdentifiers.at(0).id()] (iscore::Document* doc)
                { return doc->model()->id() == id_type<iscore::DocumentModel>(theId); });

    Q_ASSERT(doc_it != end(docs));

    IdentifiedObjectAbstract* obj = (*doc_it)->model();
    for(int i = 1; i < m_objectIdentifiers.size(); i++)
    {
        obj = obj->findIdentifiedChild(m_objectIdentifiers.at(i));
    }

    return obj;
}
