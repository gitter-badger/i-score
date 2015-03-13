#pragma once
#include <iscore/tools/IdentifiedObject.hpp>
#include <QJsonObject>
#include <iscore/selection/SelectionStack.hpp>
namespace iscore
{
    class DocumentDelegatePresenterInterface;
    class DocumentDelegateModelInterface : public IdentifiedObject<DocumentDelegateModelInterface>
    {
            Q_OBJECT
        public:
            using IdentifiedObject::IdentifiedObject;
            virtual ~DocumentDelegateModelInterface() = default;

            virtual QByteArray toByteArray() = 0;
            virtual QJsonObject toJson() = 0;

        public slots:
            virtual void setNewSelection(const Selection& s) = 0;
    };
}