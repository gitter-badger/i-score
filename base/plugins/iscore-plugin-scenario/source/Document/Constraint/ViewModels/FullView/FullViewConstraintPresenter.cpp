#include "FullViewConstraintPresenter.hpp"

#include "Document/Event/EventModel.hpp"
#include "Document/Constraint/ConstraintModel.hpp"
#include "Document/Constraint/ViewModels/FullView/FullViewConstraintViewModel.hpp"
#include "Document/Constraint/ViewModels/FullView/FullViewConstraintView.hpp"
#include "Commands/Constraint/AddProcessToConstraint.hpp"

#include <iscore/document/DocumentInterface.hpp>
#include <core/document/Document.hpp>

#include <QGraphicsScene>

FullViewConstraintPresenter::FullViewConstraintPresenter(
        const FullViewConstraintViewModel& cstr_model,
        QGraphicsItem*parentobject,
        QObject* parent) :
    AbstractConstraintPresenter {"FullViewConstraintPresenter",
                                 cstr_model,
                                 new FullViewConstraintView{*this, parentobject},
                                 parent},
    m_selectionDispatcher{iscore::IDocument::documentFromObject(cstr_model.model())->selectionStack()}
{
    if(viewModel(this)->isRackShown())
    {
        on_rackShown(viewModel(this)->shownRack());
    }

    updateHeight();

    connect(this, &AbstractConstraintPresenter::pressed,
            this, &FullViewConstraintPresenter::on_pressed);
}

FullViewConstraintPresenter::~FullViewConstraintPresenter()
{
    if(::view(this))
    {
        auto sc = ::view(this)->scene();

        if(sc && sc->items().contains(::view(this)))
        {
            sc->removeItem(::view(this));
        }

        ::view(this)->deleteLater();
    }
}

void FullViewConstraintPresenter::on_pressed(const QPointF&)
{
    m_selectionDispatcher.setAndCommit({&this->model()});
}
