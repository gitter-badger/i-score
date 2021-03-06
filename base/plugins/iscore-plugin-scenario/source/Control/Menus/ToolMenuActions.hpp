#pragma once
#include "Control/Menus/AbstractMenuActions.hpp"

class QToolBar;
class ToolMenuActions : public AbstractMenuActions
{
    public:
        ToolMenuActions(iscore::ToplevelMenuElement menuElt, ScenarioControl* parent);
        void fillMenuBar(iscore::MenubarManager *menu) override;
        void fillContextMenu(QMenu* menu, const Selection&sel) override;
        void makeToolBar(QToolBar* bar);
        void setEnabled(bool);

        QList<QAction*> modeActions();
        QList<QAction*> toolActions();
        QAction* shiftAction();

    public slots:
        void keyPressed(int key);
        void keyReleased(int key);

    private:
        QActionGroup* m_scenarioScaleModeActionGroup{};
        QActionGroup* m_scenarioToolActionGroup{};
        QAction* m_shiftAction{};

        QAction* m_selecttool{};
        QAction* m_createtool{};
};
