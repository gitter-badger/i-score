#pragma once
#include "Curve/Segment/CurveSegmentModel.hpp"

class PowerCurveSegmentModel : public CurveSegmentModel
{
    public:
        using CurveSegmentModel::CurveSegmentModel;

        template<typename Impl>
        PowerCurveSegmentModel(Deserializer<Impl>& vis, QObject* parent) :
            CurveSegmentModel {vis, parent}
        {
            vis.writeTo(*this);
        }

        CurveSegmentModel* clone(
                const id_type<CurveSegmentModel>& id,
                QObject* parent) const override;

        QString name() const override;
        void serialize(const VisitorVariant& vis) const override;
        void on_startChanged() override;
        void on_endChanged() override;

        void updateData(int numInterp) override;

        boost::optional<double> verticalParameter() override;
        void setVerticalParameter(double p) override;

        double gamma = 0.5;

};
