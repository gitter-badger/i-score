#pragma once
#include <tools/IdentifiedObject.hpp>
#include <tools/SettableIdentifier.hpp>
#include <interface/serialization/DataStreamVisitor.hpp>
#include <interface/serialization/JSONVisitor.hpp>
#include "Document/ModelMetadata.hpp"
#include <ProcessInterface/TimeValue.hpp>

class EventModel;
class TimeNodeModel : public IdentifiedObject<TimeNodeModel>
{
		Q_OBJECT

		friend void Visitor<Reader<DataStream>>::readFrom<TimeNodeModel>(const TimeNodeModel& ev);
		friend void Visitor<Reader<JSON>>::readFrom<TimeNodeModel>(const TimeNodeModel& ev);
		friend void Visitor<Writer<DataStream>>::writeTo<TimeNodeModel>(TimeNodeModel& ev);
		friend void Visitor<Writer<JSON>>::writeTo<TimeNodeModel>(TimeNodeModel& ev);

	public:
		ModelMetadata metadata;

		TimeNodeModel(id_type<TimeNodeModel> id, QObject* parent);
		TimeNodeModel(id_type<TimeNodeModel> id, TimeValue date, QObject *parent);
		~TimeNodeModel();

		template<typename DeserializerVisitor>
		TimeNodeModel(DeserializerVisitor&& vis, QObject* parent):
			IdentifiedObject<TimeNodeModel>{vis, parent}
		{
			vis.writeTo(*this);
		}

		void addEvent(id_type<EventModel>);
		bool removeEvent(id_type<EventModel>);

		double top() const;
		double bottom() const;
		TimeValue date() const;

		void setDate(TimeValue);

		bool isEmpty();

		double y() const;
		void setY(double y);

		QVector<id_type<EventModel> > events() const;
		void setEvents(const QVector<id_type<EventModel> > &events);

	signals:
		void dateChanged();

	public slots:

	private:
		TimeValue m_date{0s};
		double m_y{0.0};

		QVector<id_type<EventModel>> m_events;

		// @todo : maybe not useful ...
		double m_topY{0.0};
		double m_bottomY{0.0};
};