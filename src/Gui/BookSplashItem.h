// App/BookSplashItem.H - This file is part of NotedELN

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

// BookSplashItem.H

#ifndef BOOKSPLASHITEM_H

#define BOOKSPLASHITEM_H

#include "RecentBooks.h"
#include <QGraphicsObject>

class BookSplashItem: public QGraphicsObject {
  Q_OBJECT;
public:
  static constexpr int BOXWIDTH = 500;
  static constexpr int BOXHEIGHT = 63;
  static constexpr int SMALLBOXHEIGHT = 30;
  static constexpr double BOXRAD = 3.0;
  static constexpr double HOVERDX = 1.5;
  static constexpr double HOVERDX1 = 0.5;
  static constexpr double SHRINK = 1;
public:
  BookSplashItem(QString dirname, BookInfo const &info,
                 QGraphicsItem *parent=0);
  BookSplashItem(QString label,
                 QGraphicsItem *parent=0);
  void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
  QRectF boundingRect() const;
signals:
  void leftClick(QString); // dirname or label
  //  void rightClick(QString); // dirname or label
protected:
  void hoverEnterEvent(QGraphicsSceneHoverEvent *);
  void hoverLeaveEvent(QGraphicsSceneHoverEvent *);
  void mousePressEvent(QGraphicsSceneMouseEvent *);
private:
  void doMenu();
private:
  QString dirname;
  BookInfo info;
  bool hov;
};

#endif
