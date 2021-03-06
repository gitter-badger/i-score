#include "InspectorPanel.hpp"
#include <Inspector/InspectorSectionWidget.hpp>
#include <Inspector/InspectorWidgetList.hpp>

#include <QTabWidget>

InspectorPanel::InspectorPanel(iscore::SelectionStack& s, QWidget* parent) :
    QWidget {parent},
    m_layout{new QVBoxLayout{this}},
    m_tabWidget{new QTabWidget{this}},
    m_selectionDispatcher{s}
{
    m_tabWidget->setTabsClosable(true);
    m_layout->setMargin(0);
    setMinimumWidth(300);
    setMaximumWidth(300);
    m_layout->addWidget(m_tabWidget);


    connect(m_tabWidget,    &QTabWidget::tabCloseRequested,
            this, &InspectorPanel::on_tabClose);
}

void InspectorPanel::newItemsInspected(const Selection& objects)
{
    // Ignore items in both
    // Create items in objects and not in current
    // Delete items in current and not in objects
    Selection toCreate, toDelete;
    std::set_difference(objects.begin(), objects.end(),
                        m_currentSel.begin(), m_currentSel.end(),
                        std::inserter(toCreate, toCreate.begin()));

    std::set_difference(m_currentSel.begin(), m_currentSel.end(),
                        objects.begin(), objects.end(),
                        std::inserter(toDelete, toDelete.begin()));
    for(const auto& object : toDelete)
    {
        auto widget_it = m_map.get<0>().find(object);
        if(widget_it != m_map.get<0>().end())
        {
            (*widget_it)->deleteLater();
            *m_map.get<0>().erase(widget_it);
        }
    }

    for(const auto& object : toCreate)
    {
        auto widget = InspectorWidgetList::makeInspectorWidget(object->objectName(),
                                                               object,
                                                               m_tabWidget);
        m_tabWidget->addTab(widget, widget->tabName());
        m_map.insert(widget);
    }

    m_currentSel = objects;
}

void InspectorPanel::on_tabClose(int index)
{
    auto inspector_widget = static_cast<InspectorWidgetBase*>(m_tabWidget->widget(index));
    // TODO need m_tabWidget.movable() = false !

    Selection sel = m_currentSel;
    sel.erase(inspector_widget->inspectedObject());

    m_selectionDispatcher.setAndCommit(sel);
}
