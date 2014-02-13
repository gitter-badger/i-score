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

#ifndef TIMEBOX_HPP
#define TIMEBOX_HPP

class TimeboxModel;
class TimeboxPresenter;
class TimeboxFullView;
class TimeboxSmallView;
class QGraphicsView;
class QGraphicsScene;
class TimeEvent;

#include <QObject>
#include "utils.hpp"

class Timebox : public QObject
{
  Q_OBJECT

public:
  TimeboxPresenter *_pPresenter; /// @todo rendu public pour slot goSmallView()
  TimeboxSmallView *_pSmallView;

  QGraphicsScene *_pScene; /// @todo Sert actuellement à héberger le scénario principal (qgraphicsscene) qui n'est pas encore un plugin Scenario (par jC)

private:
  TimeboxModel *_pModel;
  TimeboxFullView *_pFullView;
  QGraphicsView *_pView;

public:
  Timebox(Timebox *pParent = 0);
  explicit Timebox(Timebox *pParent, QGraphicsView *pView, const QPointF &pos = QPointF(10,10), float width = 300, float height = 200, ViewMode mode = SMALL);
  ~Timebox();

signals:
  void timeboxBecameFull();

public:
  //ViewMode mode() const {return _pPresenter->_mode;}
  QGraphicsView* view() const {return _pView;}
  bool isEqual(const Timebox &other);
  void addChild (Timebox *other);
  void addChild (TimeEvent *timeEvent);
};

#endif // TIMEBOX_HPP