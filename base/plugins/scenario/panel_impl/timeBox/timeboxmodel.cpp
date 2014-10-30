/*
Copyright: LaBRI / SCRIME

This software is governed by the CeCILL license under French law and
abiding by the rules of distribution of free software.  You can  use,
modify and/ or redistribute the software under the terms of the CeCILL
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info".

As a counterpart to the access to the source code and  rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty  and the software's author,  the holder of the
economic rights,  and the successive licensors  have only  limited
liability.

In this respect, the user's attention is drawn to the risks associated
with loading,  using,  modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean  that it is complicated to manipulate,  and  that  also
therefore means  that it is reserved for developers  and  experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or
data to be ensured and,  more generally, to use and operate it in the
same conditions as regards security.

The fact that you are presently reading this means that you have had
knowledge of the CeCILL license and that you accept its terms.
*/

#include "timeboxmodel.hpp"
#include "timebox.hpp"
#include "../timeEvent/timeevent.hpp"
#include "../timeEvent/timeeventmodel.hpp"

#include <QFinalState>
#include <QGraphicsScene>

TimeboxModel::TimeboxModel(qreal t, qreal y, qreal w, qreal h, QString name, Timebox *pParent, TimeEvent *pTimeEventStart, TimeEvent *pTimeEventEnd)
  : QObject(pParent), _time(t), _yPosition(y), _width(w), _height(h), _name(name), _pStartTimeEvent(pTimeEventStart), _pEndTimeEvent(pTimeEventEnd)
{

  TimeEventModel *timeEventModel = _pStartTimeEvent->model();
  timeEventModel->addTimebox(pParent);
  connect(timeEventModel, SIGNAL(timeChanged(qreal)), this, SLOT(settime(qreal)));
  connect(this, SIGNAL(timeChanged(qreal)), timeEventModel, SLOT(settime(qreal)));
  connect(timeEventModel, SIGNAL(yPositionChanged(qreal)), this, SLOT(setYPosition(qreal)));
  connect(this, SIGNAL(yPositionChanged(qreal)), timeEventModel, SLOT(setYPosition(qreal)));

  timeEventModel = _pEndTimeEvent->model();
  timeEventModel->addTimebox(pParent);
  connect(timeEventModel, SIGNAL(timeChanged(qreal)), this, SLOT(settimeEnd(qreal)));
  connect(this, SIGNAL(timeEndChanged(qreal)), timeEventModel, SLOT(settime(qreal)));
  connect(timeEventModel, SIGNAL(yPositionChanged(qreal)), this, SLOT(setYPosition(qreal)));
  connect(this, SIGNAL(yPositionChanged(qreal)), timeEventModel, SLOT(setYPosition(qreal)));
}

void TimeboxModel::addPluginSmall()
{
  _pluginsSmallView.emplace_back();
}

void TimeboxModel::removePluginSmall()
{
  _pluginsSmallView.pop_back();
}

void TimeboxModel::addPluginFull()
{
  _pluginsFullView.emplace_back();
}

void TimeboxModel::removePluginFull()
{
  _pluginsFullView.pop_back();
}

void TimeboxModel::setname(QString arg)
{
  if (_name != arg) {
      _name = arg;
      emit nameChanged(arg);
    }
}

void TimeboxModel::settime(qreal arg)
{
  if (_time != arg) {
      _time = arg;
      emit timeChanged(arg);
      emit timeEndChanged(arg+width());
    }
}

void TimeboxModel::settimeEnd(qreal arg)
{
  arg -= width();
  settime(arg);
}

void TimeboxModel::setYPosition(qreal arg)
{
  if (_yPosition != arg) {
      _yPosition = arg;
      emit yPositionChanged(arg);
    }
}