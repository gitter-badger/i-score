#include "ObjectMenuActions.hpp"

#include <core/document/DocumentModel.hpp>

#include "iscore/menu/MenuInterface.hpp"

#include "Process/ScenarioGlobalCommandManager.hpp"
#include "Process/ScenarioModel.hpp"
#include "Process/Temporal/TemporalScenarioPresenter.hpp"

#include "Document/BaseElement/BaseElementModel.hpp"
#include "Document/BaseElement/BaseElementPresenter.hpp"
#include "Document/Constraint/ConstraintModel.hpp"
#include "Document/Event/EventModel.hpp"
#include "Document/TimeNode/TimeNodeModel.hpp"

#include "Commands/Constraint/CopyConstraintContent.hpp"
#include "Commands/Constraint/AddProcessToConstraint.hpp"

#include "Control/ScenarioControl.hpp"

#include <QJsonDocument>
#include <QApplication>
#include <QClipboard>

#include <QTextEdit>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QTextBlock>
#include <QDialog>

// TODO make a class
class TextDialog : public QDialog
{
    public:
        TextDialog(const QString& s)
        {
            this->setLayout(new QGridLayout);
            auto textEdit = new QTextEdit;
            textEdit->setPlainText(s);
            layout()->addWidget(textEdit);
            auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
            layout()->addWidget(buttonBox);

            connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
        }
};

ObjectMenuActions::ObjectMenuActions(
        iscore::ToplevelMenuElement menuElt,
        ScenarioControl* parent) :
    AbstractMenuActions(menuElt, parent)
{
    // REMOVE
    m_removeElements = new QAction{tr("Remove selected elements"), this};
    m_removeElements->setShortcut(QKeySequence::Delete);
    connect(m_removeElements, &QAction::triggered,
            [this]()
    {
        if (auto sm = m_parent->focusedScenarioModel())
        {
            ScenarioGlobalCommandManager mgr{m_parent->currentDocument()->commandStack()};
            mgr.removeSelection(*sm);
        }
    });

    m_clearElements = new QAction{tr("Clear selected elements"), this};
    m_clearElements->setShortcut(Qt::Key_Backspace);
    connect(m_clearElements, &QAction::triggered,
            [this]()
    {
        if (auto sm = m_parent->focusedScenarioModel())
        {
            ScenarioGlobalCommandManager mgr{m_parent->currentDocument()->commandStack()};
            mgr.clearContentFromSelection(*sm);
        }
    });

    // COPY/CUT
    m_copyContent = new QAction{tr("Copy"), this};
    m_copyContent->setShortcut(QKeySequence::Copy);
    connect(m_copyContent, &QAction::triggered,
            [this]()
    {
        QJsonDocument doc{copySelectedElementsToJson()};
        auto clippy = QApplication::clipboard();
        clippy->setText(doc.toJson(QJsonDocument::Indented));
    });

    m_cutContent = new QAction{tr("Cut"), this};
    m_cutContent->setShortcut(QKeySequence::Cut);
    connect(m_cutContent, &QAction::triggered,
            [this]()
    {
        QJsonDocument doc{cutSelectedElementsToJson()};
        auto clippy = QApplication::clipboard();
        clippy->setText(doc.toJson(QJsonDocument::Indented));
    });

    m_pasteContent = new QAction{tr("Paste"), this};
    m_pasteContent->setShortcut(QKeySequence::Paste);
    connect(m_pasteContent, &QAction::triggered,
            [this]()
    {
        writeJsonToSelectedElements(
                    QJsonDocument::fromJson(
                        QApplication::clipboard()->text().toLatin1()).object());
    });

    // DISPLAY JSON
    m_elementsToJson = new QAction{tr("Convert selection to JSON"), this};
    connect(m_elementsToJson, &QAction::triggered,
            [this]()
    {
        QJsonDocument doc{copySelectedElementsToJson()};
        auto s = new TextDialog(doc.toJson(QJsonDocument::Indented));

        s->show();
    });

    // ADD PROCESS
    m_addProcessDialog.setParent(qApp->activeWindow());
    connect(&m_addProcessDialog, &AddProcessDialog::okPressed,
            this, &ObjectMenuActions::addProcessInConstraint);

    m_addProcess = new QAction{tr("Add Process in constraint"), this};
    connect(m_addProcess, &QAction::triggered,
            [this]()
    {
        m_addProcessDialog.launchWindow();
    });

}

void ObjectMenuActions::fillMenuBar(iscore::MenubarManager* menu)
{
    menu->insertActionIntoToplevelMenu(m_menuElt, m_addProcess);
    menu->insertActionIntoToplevelMenu(m_menuElt, m_elementsToJson);
    menu->insertActionIntoToplevelMenu(m_menuElt, m_removeElements);
    menu->insertActionIntoToplevelMenu(m_menuElt, m_clearElements);
    menu->addSeparatorIntoToplevelMenu(m_menuElt, iscore::EditMenuElement::Separator_Copy);
    menu->insertActionIntoToplevelMenu(m_menuElt, m_copyContent);
    menu->insertActionIntoToplevelMenu(m_menuElt, m_cutContent);
    menu->insertActionIntoToplevelMenu(m_menuElt, m_pasteContent);
}

void ObjectMenuActions::fillContextMenu(QMenu *menu, const Selection& sel)
{
    if(sel.empty())
        return;

    if(std::any_of(sel.cbegin(),
                   sel.cend(),
                   [] (const QObject* obj) { return dynamic_cast<const ConstraintModel*>(obj); }))
    {
        menu->addAction(m_addProcess);
        menu->addSeparator();
    }

    menu->addAction(m_elementsToJson);
    menu->addAction(m_removeElements);
    menu->addAction(m_clearElements);
    menu->addSeparator();

    menu->addAction(m_copyContent);
    menu->addAction(m_cutContent);
    menu->addAction(m_pasteContent);
}

template<typename Selected_T>
auto arrayToJson(Selected_T &&selected)
{
    QJsonArray array;
    if (!selected.empty())
    {
        for (const auto &element : selected)
        {
            Visitor<Reader<JSONObject>> jr;
            jr.readFrom(*element);
            array.push_back(jr.m_obj);
        }
    }

    return array;
}

QJsonObject ObjectMenuActions::copySelectedElementsToJson()
{
    QJsonObject base;

    if (auto sm = m_parent->focusedScenarioModel())
    {
        base["Constraints"] = arrayToJson(selectedElements(sm->constraints()));
        base["Events"] = arrayToJson(selectedElements(sm->events()));
        base["TimeNodes"] = arrayToJson(selectedElements(sm->timeNodes()));
        base["TimeNodes"] = arrayToJson(selectedElements(sm->states()));
    }
    else
    {
        // Full-view copy
        auto& bem = iscore::IDocument::modelDelegate<BaseElementModel>(*m_parent->currentDocument());
        if(bem.baseConstraint().selection.get())
        {
            QJsonArray arr;
            Visitor<Reader<JSONObject>> jr;
            jr.readFrom(bem.baseConstraint());
            arr.push_back(jr.m_obj);
            base["Constraints"] = arr;
        }
    }
    return base;
}

QJsonObject ObjectMenuActions::cutSelectedElementsToJson()
{
    auto obj = copySelectedElementsToJson();

    if (auto sm = m_parent->focusedScenarioModel())
    {
        ScenarioGlobalCommandManager mgr{m_parent->currentDocument()->commandStack()};
        mgr.clearContentFromSelection(*sm);
    }

    return obj;
}

void ObjectMenuActions::writeJsonToSelectedElements(const QJsonObject &obj)
{
    auto pres = m_parent->focusedPresenter();
    if(!pres)
        return;

    auto sm = m_parent->focusedScenarioModel();

    auto selectedConstraints = selectedElements(sm->constraints());
    for(const auto& json_vref : obj["Constraints"].toArray())
    {
        for(const auto& constraint : selectedConstraints)
        {
            auto cmd = new Scenario::Command::CopyConstraintContent{
                       json_vref.toObject(),
                       iscore::IDocument::path(constraint),
                       pres->stateMachine().expandMode()};

            CommandDispatcher<> dispatcher{m_parent->currentDocument()->commandStack()};
            dispatcher.submitCommand(cmd);
        }
    }
}

void ObjectMenuActions::addProcessInConstraint(QString processName)
{
    auto selectedConstraints = selectedElements(m_parent->focusedScenarioModel()->constraints());
    auto cmd = new Scenario::Command::AddProcessToConstraint
    {
        iscore::IDocument::path(*selectedConstraints.begin()),
        processName
    };
    CommandDispatcher<> dispatcher{m_parent->currentDocument()->commandStack()};
    emit dispatcher.submitCommand(cmd);
}

