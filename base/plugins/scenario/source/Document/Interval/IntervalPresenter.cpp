#include "IntervalPresenter.hpp"

#include "Document/Interval/IntervalModel.hpp"
#include "Document/Interval/IntervalView.hpp"
#include "Document/Interval/IntervalContent/IntervalContentPresenter.hpp"
#include "Document/Interval/IntervalContent/IntervalContentView.hpp"
#include "Commands/Interval/Process/AddProcessToIntervalCommand.hpp"

#include <core/presenter/command/SerializableCommand.hpp>

#include <QDebug>
#include <QGraphicsScene>

IntervalPresenter::IntervalPresenter(IntervalModel* model,
									 IntervalView* view,
									 QObject* parent):
	NamedObject{"IntervalPresenter", parent},
	m_model{model},
	m_view{view}
{
	view->m_rect.setWidth(model->width()/m_millisecPerPixel);
	view->m_rect.setHeight(model->height());

	// Le contentView est child de IntervalView (au sens Qt) mais est accessible via son présenteur.
	// Le présenteur parent va créer les vues correspondant aux présenteurs enfants
	// TODO mettre ça dans la doc des classes

	auto contentView = new IntervalContentView{view};

	// Cas par défaut
	auto interval_presenter =
			new IntervalContentPresenter{
							  model->contentModels().front(),
							  contentView,
							  this};

	connect(interval_presenter, &IntervalContentPresenter::submitCommand,
			this,				&IntervalPresenter::submitCommand);
	connect(interval_presenter, &IntervalContentPresenter::elementSelected,
			this,				&IntervalPresenter::elementSelected);

	m_contentPresenters.push_back(interval_presenter);

	connect(m_view, &IntervalView::intervalPressed,
			this,	&IntervalPresenter::on_intervalPressed);

	connect(m_view, &IntervalView::intervalReleased,
			[&] (QPointF p)
	{
		emit intervalReleased(id(), p.y());
	});

	connect(m_view, &IntervalView::addScenarioProcessClicked,
			[&] ()
	{
		auto path = ObjectPath::pathFromObject("BaseIntervalModel", m_model);
		auto cmd = new AddProcessToIntervalCommand(std::move(path), "Scenario");
		emit submitCommand(cmd);
	});
}

IntervalPresenter::~IntervalPresenter()
{
	auto sc = m_view->scene();
	if(sc) sc->removeItem(m_view);
	m_view->deleteLater();
}

int IntervalPresenter::id() const
{
	return m_model->id();
}

IntervalView *IntervalPresenter::view()
{
	return m_view;
}

IntervalModel *IntervalPresenter::model()
{
	return m_model;
}

void IntervalPresenter::on_intervalPressed()
{
	emit elementSelected(m_model);
}