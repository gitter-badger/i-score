#include "Interval/IntervalContent/Storey/StoreyModel.hpp"

class IntervalContentModel;
class PositionedStoreyModel : public StoreyModel
{
		Q_OBJECT
		Q_PROPERTY(int position
				   READ position
				   WRITE setPosition
				   NOTIFY positionChanged)

	public:
		friend QDataStream& operator << (QDataStream& s, const PositionedStoreyModel& c);
		PositionedStoreyModel(QDataStream& s, IntervalContentModel* parent);
		PositionedStoreyModel(int position, int id, IntervalContentModel* parent);

		int position() const
		{
			return m_position;
		}

	signals:
		void positionChanged(int arg);

	public slots:
		void setPosition(int arg)
		{
			if (m_position == arg)
				return;

			m_position = arg;
			emit positionChanged(arg);
		}

	private:
		int m_position{};
};