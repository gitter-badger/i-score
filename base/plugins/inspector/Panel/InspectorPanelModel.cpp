#include "InspectorPanelModel.hpp"

#include <core/model/Model.hpp>
#include <QMetaMethod>
#include <QDebug>

InspectorPanelModel::InspectorPanelModel (iscore::Model* parent) :
	iscore::PanelModelInterface {"InspectorPanelModel", nullptr}
  // NOTE : here we declare parent after because else for some weird reason,
  // "newItemInspected" is not found...
{
	this->setParent(parent);
}

void InspectorPanelModel::newItemInspected(QObject* item)
{
	//TODO save the path to the item.
	emit setNewItem(item);
}