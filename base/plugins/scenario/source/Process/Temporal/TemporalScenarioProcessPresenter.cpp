#include "TemporalScenarioProcessPresenter.hpp"

#include "Process/ScenarioProcessSharedModel.hpp"
#include "Process/Temporal/TemporalScenarioProcessViewModel.hpp"
#include "Process/Temporal/TemporalScenarioProcessView.hpp"
#include "Document/Constraint/Temporal/TemporalConstraintView.hpp"
#include "Document/Constraint/Temporal/TemporalConstraintPresenter.hpp"
#include "Document/Constraint/Temporal/TemporalConstraintViewModel.hpp"
#include "Document/Constraint/ConstraintModel.hpp"
#include "Document/Constraint/ConstraintData.hpp"
#include "Document/Constraint/ConstraintModel.hpp"
#include "Document/Constraint/Box/BoxView.hpp"
#include "Document/Constraint/Box/BoxPresenter.hpp"
#include "Document/Event/EventModel.hpp"
#include "Document/Event/EventPresenter.hpp"
#include "Document/Event/EventView.hpp"
#include "Document/Event/EventData.hpp"
#include "Commands/Scenario/CreateEventCommand.hpp"
#include "Commands/Scenario/CreateEventAfterEventCommand.hpp"
#include "Commands/Scenario/MoveEventCommand.hpp"
#include "Commands/Scenario/MoveConstraintCommand.hpp"

#include <tools/utilsCPP11.hpp>

#include <QDebug>
#include <QRectF>
#include <QGraphicsItem>
#include <QGraphicsScene>

TemporalScenarioProcessPresenter::TemporalScenarioProcessPresenter(ProcessViewModelInterface* process_view_model,
												   ProcessViewInterface* view,
												   QObject* parent):
	ProcessPresenterInterface{"TemporalScenarioProcessPresenter", parent},
	m_viewModel{static_cast<TemporalScenarioProcessViewModel*>(process_view_model)},
	m_view{static_cast<TemporalScenarioProcessView*>(view)}
{
	/////// Setup of existing data
	// For each constraint & event, display' em
	for(auto constraint_view_model : constraintsViewModels(m_viewModel))
	{
		on_constraintCreated_impl(constraint_view_model);
	}

	for(auto event_model : model(m_viewModel)->events())
	{
		on_eventCreated_impl(event_model);
	}

	/////// Connections
	connect(this,	SIGNAL(elementSelected(QObject*)),
			parent, SIGNAL(elementSelected(QObject*)));

	connect(m_view, &TemporalScenarioProcessView::deletePressed,
			this,	&TemporalScenarioProcessPresenter::on_deletePressed);
	connect(m_view, &TemporalScenarioProcessView::scenarioPressed,
			this,	&TemporalScenarioProcessPresenter::on_scenarioPressed);
	connect(m_view, &TemporalScenarioProcessView::scenarioPressedWithControl,
			this,	&TemporalScenarioProcessPresenter::on_scenarioPressedWithControl);
	connect(m_view, &TemporalScenarioProcessView::scenarioReleased,
			this,	&TemporalScenarioProcessPresenter::on_scenarioReleased);

	connect(m_viewModel, &TemporalScenarioProcessViewModel::eventCreated,
			this,		 &TemporalScenarioProcessPresenter::on_eventCreated);
	connect(m_viewModel, &TemporalScenarioProcessViewModel::eventDeleted,
			this,		 &TemporalScenarioProcessPresenter::on_eventDeleted);
	connect(m_viewModel, &TemporalScenarioProcessViewModel::constraintViewModelCreated,
			this,		 &TemporalScenarioProcessPresenter::on_constraintCreated);
	connect(m_viewModel, &TemporalScenarioProcessViewModel::constraintViewModelRemoved,
			this,		 &TemporalScenarioProcessPresenter::on_constraintViewModelRemoved);
	connect(m_viewModel, &TemporalScenarioProcessViewModel::eventMoved,
			this,		 &TemporalScenarioProcessPresenter::on_eventMoved);
	connect(m_viewModel, &TemporalScenarioProcessViewModel::constraintMoved,
			this,		 &TemporalScenarioProcessPresenter::on_constraintMoved);
}

TemporalScenarioProcessPresenter::~TemporalScenarioProcessPresenter()
{
	auto sc = m_view->scene();
	if(sc) sc->removeItem(m_view);
	m_view->deleteLater();
}

int TemporalScenarioProcessPresenter::viewModelId() const
{
	return m_viewModel->id();
}

int TemporalScenarioProcessPresenter::modelId() const
{
	return m_viewModel->sharedProcessModel()->id();
}

int TemporalScenarioProcessPresenter::currentlySelectedEvent() const
{
	return m_currentlySelectedEvent;
}

void TemporalScenarioProcessPresenter::on_eventCreated(int eventId)
{
	on_eventCreated_impl(model(m_viewModel)->event(eventId));
}

void TemporalScenarioProcessPresenter::on_constraintCreated(int constraintViewModelId)
{
	on_constraintCreated_impl(constraintViewModel(m_viewModel, constraintViewModelId));
}

void TemporalScenarioProcessPresenter::on_eventDeleted(int eventId)
{
	removeFromVectorWithId(m_events, eventId);
	m_view->update();
}

void TemporalScenarioProcessPresenter::on_constraintViewModelRemoved(int constraintViewModelId)
{
	vec_erase_remove_if(m_constraints,
						[&constraintViewModelId] (TemporalConstraintPresenter* pres)
						{
							bool to_delete = pres->viewModel()->id() == constraintViewModelId;
							if(to_delete) delete pres;
							return to_delete;
						} );

	m_view->update();
}

void TemporalScenarioProcessPresenter::on_eventMoved(int eventId)
{
	auto rect = m_view->boundingRect();
	auto ev = findById(m_events, eventId);

	ev->view()->setPos({qreal(ev->model()->date()),
						rect.height() * ev->model()->heightPercentage()});
	ev->model()->updateVerticalLink();

	m_view->update();
}

void TemporalScenarioProcessPresenter::on_constraintMoved(int constraintId)
{
	auto rect = m_view->boundingRect();

	for(TemporalConstraintPresenter* cstr_pres : m_constraints)
	{
		ConstraintModel* cstr_model{cstr_pres->viewModel()->model()};
		if(cstr_model->id() == constraintId )
		{
			cstr_pres->view()->setPos({qreal(cstr_model->startDate()),
									   rect.height() * cstr_model->heightPercentage()});

			cstr_pres->view()->setWidth(cstr_model->width());
		}
	}
	m_view->update();
}

void TemporalScenarioProcessPresenter::on_deletePressed()
{
	deleteSelection();
}

void TemporalScenarioProcessPresenter::on_scenarioPressed()
{
	for(auto& event : m_events)
	{
		event->deselect();
	}
	for(auto& constraint : m_constraints)
	{
		constraint->deselect();
	}
}


void TemporalScenarioProcessPresenter::on_scenarioPressedWithControl(QPointF point)
{
	// @todo maybe better to create event on mouserelease ? And only show a "fake" event + interval on mousepress.
	auto cmd = new CreateEventCommand(ObjectPath::pathFromObject("BaseConstraintModel",
																 m_viewModel->sharedProcessModel()),
									 point.x(),
									 (point - m_view->boundingRect().topLeft() ).y() / m_view->boundingRect().height() );
	this->submitCommand(cmd);
}

void TemporalScenarioProcessPresenter::on_scenarioReleased(QPointF point)
{
	EventData data{};
	data.eventClickedId = m_events.back()->id();
	data.x = point.x() - m_events.back()->model()->date();
	data.y = point.y();
	if (point.x() - m_events.back()->model()->date() > 20 ) // @todo use a const to do that !
		createConstraintAndEventFromEvent(data);
}

void TemporalScenarioProcessPresenter::on_askUpdate()
{
	m_view->update();
}

template<typename InputVector, typename OutputVector>
void copyIfSelected(const InputVector& in, OutputVector& out)
{
	std::copy_if(begin(in),
				 end(in),
				 back_inserter(out),
				 [] (typename InputVector::value_type c) { return c->isSelected(); });
}

#include "Commands/Scenario/DeleteConstraintCommand.hpp"
#include "Commands/Scenario/DeleteEventCommand.hpp"
#include "Commands/DeleteMultipleElements.hpp"
void TemporalScenarioProcessPresenter::deleteSelection()
{
	using namespace Scenario::Command;
	// 1. Select items
	std::vector<TemporalConstraintPresenter*> constraintsToRemove;
	std::vector<EventPresenter*> eventsToRemove;

	copyIfSelected(m_constraints, constraintsToRemove);
	copyIfSelected(m_events, eventsToRemove);

	QVector<iscore::SerializableCommand*> commands;

	// 3. Create a Delete command for each. For now : only emptying.
	for(auto& constraint : m_constraints)
	{
		commands.push_back(
					new ClearConstraint(
						ObjectPath::pathFromObject("BaseConstraintModel",
												   constraint->viewModel()->model())));
	}

	for(auto& event : m_events)
	{
		commands.push_back(
					new EmptyEventCommand(
						ObjectPath::pathFromObject("BaseConstraintModel",
												   event->model())));
	}

	// 4. Make a meta-command that binds them all and calls undo & redo on the queue.
	auto cmd = new DeleteMultipleElementsCommand(std::move(commands));
	emit submitCommand(cmd);
}

void TemporalScenarioProcessPresenter::setCurrentlySelectedEvent(int arg)
{
	if (m_currentlySelectedEvent != arg)
	{
		m_currentlySelectedEvent = arg;
		emit currentlySelectedEventChanged(arg);
	}
}

void TemporalScenarioProcessPresenter::createConstraintAndEventFromEvent(EventData data)
{
	data.x = data.x - model(m_viewModel)->event(data.eventClickedId)->date();
	data.relativeY = data.y / m_view->boundingRect().height();

	auto cmd = new CreateEventAfterEventCommand(ObjectPath::pathFromObject("BaseConstraintModel",
																		   m_viewModel->sharedProcessModel()),
												data);

	submitCommand(cmd);
}

void TemporalScenarioProcessPresenter::moveEventAndConstraint(EventData data)
{
	data.relativeY = data.y / m_view->boundingRect().height();

	auto cmd = new MoveEventCommand(ObjectPath::pathFromObject("BaseConstraintModel",
															   m_viewModel->sharedProcessModel()),
									data);
	submitCommand(cmd);
}

void TemporalScenarioProcessPresenter::moveConstraint(ConstraintData data)
{
	data.relativeY = data.y / m_view->boundingRect().height();

	auto cmd = new MoveConstraintCommand(ObjectPath::pathFromObject("BaseConstraintModel",
																	m_viewModel->sharedProcessModel()),
									data);

	submitCommand(cmd);
}




void TemporalScenarioProcessPresenter::on_eventCreated_impl(EventModel* event_model)
{
	auto rect = m_view->boundingRect();

	auto event_view = new EventView{m_view};
	auto event_presenter = new EventPresenter{event_model,
											  event_view,
											  this};
	event_view->setPos({rect.x() + event_model->date(),
						rect.y() + rect.height() * event_model->heightPercentage()});

	m_events.push_back(event_presenter);

	connect(event_presenter, &EventPresenter::eventSelected,
			this,			 &TemporalScenarioProcessPresenter::setCurrentlySelectedEvent);
	connect(event_presenter, &EventPresenter::eventReleasedWithControl,
			this,			 &TemporalScenarioProcessPresenter::createConstraintAndEventFromEvent);
	connect(event_presenter, &EventPresenter::eventReleased,
			this,			 &TemporalScenarioProcessPresenter::moveEventAndConstraint);
	connect(event_presenter, &EventPresenter::elementSelected,
			this,			 &TemporalScenarioProcessPresenter::elementSelected);

	connect(event_presenter, &EventPresenter::linesExtremityChange,
			[event_view, this] (double top, double bottom)
			{
				event_view->setLinesExtremity(top * m_view->boundingRect().height(),
											  bottom * m_view->boundingRect().height());
			});
}

void TemporalScenarioProcessPresenter::on_constraintCreated_impl(TemporalConstraintViewModel* constraint_view_model)
{
	auto rect = m_view->boundingRect();

	auto constraint_view = new TemporalConstraintView{m_view};
	auto constraint_presenter = new TemporalConstraintPresenter{
													constraint_view_model,
													constraint_view,
													this};

	constraint_view->setPos({rect.x() + constraint_view_model->model()->startDate(),
							 rect.y() + rect.height() * constraint_view_model->model()->heightPercentage()});

	m_constraints.push_back(constraint_presenter);

	connect(constraint_presenter,	&TemporalConstraintPresenter::constraintReleased,
			this,					&TemporalScenarioProcessPresenter::moveConstraint);
	connect(constraint_presenter,	&TemporalConstraintPresenter::submitCommand,
			this,					&TemporalScenarioProcessPresenter::submitCommand);
	connect(constraint_presenter,	&TemporalConstraintPresenter::elementSelected,
			this,					&TemporalScenarioProcessPresenter::elementSelected);


	connect(constraint_presenter,	&TemporalConstraintPresenter::askUpdate,
			this,					&TemporalScenarioProcessPresenter::on_askUpdate);
}