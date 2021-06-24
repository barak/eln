// Items/DragLine.H - This file is part of NotedELN

/* NotedELN is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   NotedELN is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with NotedELN.  If not, see <http://www.gnu.org/licenses/>.
*/

// DragLine.H

#ifndef DRAGLINE_H

#define DRAGLINE_H

#include <QGraphicsLineItem>

class DragLine: public QObject, public QGraphicsLineItem {
  Q_OBJECT;
public:
  DragLine(QPointF p0, QGraphicsItem *parent);
  virtual ~DragLine();
  QPointF startPoint() const;
  QPointF endPoint() const;
  void setEndPoint(QPointF p1);
  bool isShort() const; // true if trivially short
signals:
  void release();
public:
  static QPointF drag(QGraphicsScene *scene, QPointF p0, class Style const &s);
  static QPointF drag(QGraphicsScene *scene, QPointF p0, double pw, QColor c);
  /* drag() returns the end point of the dragged line, except if the line
     is short (according to isShort), in which case the start point is
     returned.
  */
  static bool isShort(QPointF p0, QPointF p1);
  static double shortThreshold();
protected:
  void mouseMoveEvent(QGraphicsSceneMouseEvent *);
  void mouseReleaseEvent(QGraphicsSceneMouseEvent *);
};

#endif
