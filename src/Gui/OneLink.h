// App/OneLink.h - This file is part of NotedELN

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

// OneLink.h

#ifndef ONELINK_H

#define ONELINK_H

#include <QObject>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include "PreviewPopper.h"
#include <QPointer>

class OneLink: public QObject {
  Q_OBJECT;
public:
  OneLink(class MarkupData *md, class TextItem *item, bool tryload);
  virtual ~OneLink();
  bool mousePress(QGraphicsSceneMouseEvent *);
  bool mouseDoubleClick(QGraphicsSceneMouseEvent *);
  void enter(QGraphicsSceneHoverEvent *);
  void leave();
  void update();
private slots:
  void downloadFinished();
  void activate(Qt::KeyboardModifiers);
public:
  QString refText() const;
  void openLink();
  void openArchive();
  class Resource *resource() const;
  void contextMenu(QGraphicsSceneMouseEvent *);
  void openPage(bool newView=false);
protected:
  void getArchiveAndPreview();
private:
  MarkupData *md; // we do not own
  TextItem *ti; // we do not own
  QPointer<PreviewPopper> popper; // we own
  QString lastRef;
  bool lastRefIsNew;
  bool busy;  
};

#endif
