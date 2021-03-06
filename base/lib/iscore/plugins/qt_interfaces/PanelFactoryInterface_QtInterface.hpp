#pragma once
#include <QObject>
#include <QStringList>

namespace iscore
{
    class PanelFactory;
    class PanelFactory_QtInterface
    {
        public:
            virtual ~PanelFactory_QtInterface() = default;

            virtual QList<PanelFactory*> panels() = 0;
    };
}

#define PanelFactory_QtInterface_iid "org.ossia.i-score.plugins.PanelFactory_QtInterface"

Q_DECLARE_INTERFACE(iscore::PanelFactory_QtInterface, PanelFactory_QtInterface_iid)
