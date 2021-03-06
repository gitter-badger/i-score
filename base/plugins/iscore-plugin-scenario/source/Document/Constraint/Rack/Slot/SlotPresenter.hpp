#pragma once
#include <iscore/tools/NamedObject.hpp>
#include <iscore/tools/SettableIdentifier.hpp>
#include <ProcessInterface/ZoomHelper.hpp>

class SlotModel;
class SlotView;
namespace iscore
{
    class SerializableCommand;
}
class ProcessPresenter;
class ProcessModel;
class LayerModel;
class Layer;
class RackView;
class SlotPresenter : public NamedObject
{
        Q_OBJECT

    public:
        SlotPresenter(const SlotModel& model,
                      RackView* view,
                      QObject* parent);
        virtual ~SlotPresenter();

        const id_type<SlotModel>& id() const;
        const SlotModel& model() const;
        int height() const; // Return the height of the view

        void setWidth(double w);
        void setVerticalPosition(double h);

        // Sets the enabled - disabled graphism for
        // the slot move tool
        void enable();
        void disable();

        using ProcessPair = QPair<ProcessPresenter*, Layer*>;
        const QVector<ProcessPair>& processes() const { return m_processes; }

    signals:
        void askUpdate();

        void pressed(const QPointF&) const;
        void moved(const QPointF&) const;
        void released(const QPointF&) const;

    public slots:
        // From Model
        void on_layerModelCreated(const id_type<LayerModel>& processId);
        void on_layerModelDeleted(const id_type<LayerModel>& processId);
        void on_layerModelPutToFront(const id_type<LayerModel>& processId);
        void on_heightChanged(double height);
        void on_parentGeometryChanged();

        void on_zoomRatioChanged(ZoomRatio);

    private:
        void on_layerModelCreated_impl(const LayerModel&);

        void updateProcessesShape();

        const SlotModel& m_model;
        SlotView* m_view{};
        QVector<ProcessPair> m_processes;

        // Maybe move this out of the state of the presenter ?
        int m_currentResizingValue {}; // Used when the m_slotView is being resized.

        ZoomRatio m_zoomRatio {};

        bool m_enabled{true};
};

