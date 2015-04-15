#include "BaseElementModel.hpp"

#include "source/Document/Constraint/ConstraintModel.hpp"
#include "source/Document/Constraint/ViewModels/FullView/FullViewConstraintViewModel.hpp"
#include <iscore/serialization/JSONVisitor.hpp>

#include <iostream>
#include <iscore/document/DocumentInterface.hpp>

#include "Commands/Constraint/AddProcessToConstraint.hpp"
#include "Commands/Constraint/AddBoxToConstraint.hpp"
#include "Commands/Constraint/Box/AddDeckToBox.hpp"
#include "Commands/Constraint/Box/Deck/ResizeDeckVertically.hpp"
#include "Commands/Constraint/Box/Deck/AddProcessViewModelToDeck.hpp"
#include "Commands/Scenario/ShowBoxInViewModel.hpp"

#include "Document/Constraint/ConstraintModel.hpp"
#include "Document/Constraint/Box/BoxModel.hpp"
#include "Document/Constraint/Box/Deck/DeckModel.hpp"
#include "ProcessInterface/ProcessSharedModelInterface.hpp"
#include "ProcessInterface/ProcessViewModelInterface.hpp"

using namespace Scenario;

BaseElementModel::BaseElementModel(QVariant data,
                                   QObject* parent) :
    iscore::DocumentDelegateModelInterface {id_type<iscore::DocumentDelegateModelInterface>(0),
                                            "BaseElementModel",
                                            parent}
{
    if(data.canConvert(QMetaType::QByteArray))
    {
        id_type<iscore::DocumentDelegateModelInterface> id;
        auto arr = data.toByteArray();
        Deserializer<DataStream> deser{&arr};
        deser.writeTo(id);
        this->setId(std::move(id));
        m_baseConstraint = new ConstraintModel{deser, this};
    }
    else if(data.canConvert(QMetaType::QJsonObject))
    {
        m_baseConstraint = new ConstraintModel{Deserializer<JSON>{data.toJsonObject()}, this};
    }
    else
    {
        qFatal("Could not load BaseElementModel");
        return;
    }

    m_baseConstraint->setObjectName("BaseConstraintModel");
}
#include "Document/Constraint/Box/BoxModel.hpp"
#include "Document/Constraint/Box/Deck/DeckModel.hpp"
BaseElementModel::BaseElementModel(QObject* parent) :
    iscore::DocumentDelegateModelInterface {id_type<iscore::DocumentDelegateModelInterface>(getNextId()), "BaseElementModel", parent},
    m_baseConstraint {new ConstraintModel{
                            id_type<ConstraintModel>{0},
                            id_type<AbstractConstraintViewModel>{0},
                            0,
                            this}}
{
    ConstraintModel::Algorithms::changeAllDurations(*m_baseConstraint, std::chrono::minutes{3});
    m_baseConstraint->setObjectName("BaseConstraintModel");

    initializeNewDocument(m_baseConstraint->fullView());
    setFocusedViewModel(m_baseConstraint->boxes().front()->decks().front()->processViewModels().front());
}

void BaseElementModel::initializeNewDocument(const FullViewConstraintViewModel *viewmodel)
{
    using namespace Scenario::Command;
    auto constraint_model = viewmodel->model();
    auto base_constraint_path = iscore::IDocument::path(m_baseConstraint);

    AddProcessToConstraint cmd1{ObjectPath{base_constraint_path}, "Scenario" };
    cmd1.redo();

    AddBoxToConstraint cmd2{ObjectPath{base_constraint_path}};
    cmd2.redo();
    auto box = constraint_model->boxes().front();

    ShowBoxInViewModel cmd3{
        iscore::IDocument::path(m_baseConstraint->fullView()),
        box->id() };
    cmd3.redo();

    AddDeckToBox cmd4{iscore::IDocument::path(m_baseConstraint->boxes().front())};
    cmd4.redo();

    auto deck_path = iscore::IDocument::path(m_baseConstraint->boxes().front()->decks().front());
    ResizeDeckVertically cmd5{ObjectPath{deck_path}, 1500};
    cmd5.redo();

    AddProcessViewModelToDeck cmd6{ObjectPath{deck_path},
                                  iscore::IDocument::path(constraint_model->processes().front())};
    cmd6.redo();
}

void BaseElementModel::setFocusedViewModel(ProcessViewModelInterface* proc)
{
    if(proc != m_focusedViewModel)
    {
        m_focusedViewModel = proc;
        emit focusedViewModelChanged();
    }
}

QByteArray BaseElementModel::toByteArray()
{
    QByteArray arr;
    Serializer<DataStream> s {&arr};
    s.readFrom(this->id());
    s.readFrom(*constraintModel());

    return arr;
}

QJsonObject BaseElementModel::toJson()
{
    Serializer<JSON> s;
    s.readFrom(*constraintModel());

    return s.m_obj;
}

void BaseElementModel::setNewSelection(const Selection& s)
{
    if(s.empty())
    {
        if(m_focusedProcess)
        {
            m_focusedProcess->setSelection({});
            m_displayedConstraint->selection.set(false);
            m_focusedProcess = nullptr;
        }
    }
    else if(s.first() == m_displayedConstraint)
    {
        m_displayedConstraint->selection.set(true);
    }
    else
    {
        // We know by the presenter that all objects
        // in a given selection are in the same Process.
        m_displayedConstraint->selection.set(false);
        auto newProc = parentProcess(s.first());
        if(m_focusedProcess && newProc != m_focusedProcess)
        {
            m_focusedProcess->setSelection({});
        }

        m_focusedProcess = newProc;
        m_focusedProcess->setSelection(s);
    }

    emit focusMe();
}

void BaseElementModel::setDisplayedConstraint(ConstraintModel *constraint)
{
    m_displayedConstraint = constraint;
}
