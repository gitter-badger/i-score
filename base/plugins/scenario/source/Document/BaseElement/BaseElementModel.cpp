#include "BaseElementModel.hpp"

#include "source/Document/Constraint/ConstraintModel.hpp"
#include "source/Document/Constraint/ViewModels/FullView/FullViewConstraintViewModel.hpp"
#include <QJsonDocument>
#include <interface/serialization/JSONVisitor.hpp>

#include <iostream>
#include <core/interface/document/DocumentInterface.hpp>

#include "Commands/Constraint/AddProcessToConstraint.hpp"
#include "Commands/Constraint/AddBoxToConstraint.hpp"
#include "Commands/Constraint/Box/AddDeckToBox.hpp"
#include "Commands/Constraint/Box/Deck/ResizeDeckVertically.hpp"
#include "Commands/Constraint/Box/Deck/AddProcessViewModelToDeck.hpp"
#include "Commands/Scenario/ShowBoxInViewModel.hpp"
#include "Commands/Scenario/CreateEvent.hpp"

#include "Document/Constraint/ConstraintModel.hpp"
#include "Document/Constraint/Box/BoxModel.hpp"
#include "Document/Constraint/Box/Deck/DeckModel.hpp"
#include "ProcessInterface/ProcessSharedModelInterface.hpp"
#include "ProcessInterface/ProcessViewModelInterface.hpp"

using namespace Scenario;

BaseElementModel::BaseElementModel(QByteArray data, QObject* parent) :
    iscore::DocumentDelegateModelInterface {"BaseElementModel", parent},
    m_baseConstraint {new ConstraintModel{Deserializer<DataStream>{&data}, this}}
{
    m_baseConstraint->setObjectName("BaseConstraintModel");
}

BaseElementModel::BaseElementModel(QObject* parent) :
    iscore::DocumentDelegateModelInterface {"BaseElementModel", parent},
    m_baseConstraint {new ConstraintModel{
                            id_type<ConstraintModel>{0},
                            id_type<AbstractConstraintViewModel>{0},
                            0,
                            this}}
{
    m_baseConstraint->setDefaultDuration(std::chrono::seconds{1});
    m_baseConstraint->setObjectName("BaseConstraintModel");
    initializeNewDocument(m_baseConstraint->fullView());
    on_processSelected(findChild<ProcessSharedModelInterface*>("ScenarioProcessSharedModel"));

    using namespace iscore::IDocument;
    auto inspector = panel("InspectorWidgetModel", documentFromObject(this));
    if(inspector)
    {
        qDebug() << "yay";
    }

    // TODO : this document -> current inspector model
    //auto inspector = qApp->findChild<Inspecto
    //connect(&m_selectionStack, &SelectionStack::currentSelectionChanged,
    //        this, )
}

void BaseElementModel::initializeNewDocument(const FullViewConstraintViewModel *viewmodel)
{
    using namespace Scenario::Command;
    auto constraint_model = viewmodel->model();

    AddProcessToConstraint cmd1
    {
        {
            {"BaseConstraintModel", {}}
        },
        "Scenario"
    };
    cmd1.redo();
    auto scenarioId = constraint_model->processes().front()->id();

    AddBoxToConstraint cmd2
    {
        ObjectPath{
            {"BaseConstraintModel", {}}
        }
    };
    cmd2.redo();
    auto box = constraint_model->boxes().front();

    ShowBoxInViewModel cmd3 {viewmodel, box->id() };
    cmd3.redo();

    AddDeckToBox cmd4
    {
        ObjectPath{
            {"BaseConstraintModel", {}},
            {"BoxModel", box->id() }
        }
    };
    cmd4.redo();
    auto deckId = box->decks().front()->id();

    ResizeDeckVertically cmd5
    {
        ObjectPath{
            {"BaseConstraintModel", {}},
            {"BoxModel", box->id() },
            {"DeckModel", deckId}
        },
        1500
    };
    cmd5.redo();

    AddProcessViewModelToDeck cmd6
    {
        {
            {"BaseConstraintModel", {}},
            {"BoxModel", box->id() },
            {"DeckModel", deckId}
        },
        {
            {"BaseConstraintModel", {}},
            {"ScenarioModel", scenarioId}
        }
    };
    cmd6.redo();
}

QByteArray BaseElementModel::save()
{
    QByteArray arr;
    Serializer<DataStream> s {&arr};
    s.readFrom(*constraintModel());

    return arr;
}

#include <QApplication>
#include "base/plugins/device_explorer/DeviceInterface/DeviceExplorerInterface.hpp"
QJsonObject BaseElementModel::toJson()
{
    QJsonObject complete;
    // TODO : save all panels from the iscore_lib::Document
    // Device explorer
    auto deviceExplorerModel = DeviceExplorer::getModel(this);

    if(deviceExplorerModel)
    {
        complete["DeviceExplorer"] = DeviceExplorer::toJson(deviceExplorerModel);
    }

    // Document
    Serializer<JSON> s;
    s.readFrom(*constraintModel());

    complete["Scenario"] = s.m_obj;
    return complete;
}

void BaseElementModel::on_processSelected(ProcessSharedModelInterface *proc)
{
    if(m_selectedProcess)
        disconnect(m_selectedProcess, &ProcessSharedModelInterface::selectedChildrenChanged,
                   this,			  &BaseElementModel::on_selectedChildrenChanged);

    m_selectedProcess = proc;

    if(m_selectedProcess)
        connect(m_selectedProcess, &ProcessSharedModelInterface::selectedChildrenChanged,
                this,			   &BaseElementModel::on_selectedChildrenChanged);
}

void BaseElementModel::on_selectedChildrenChanged()
{
    m_selectionStack.push(m_selectedProcess->selectedChildren());
}


