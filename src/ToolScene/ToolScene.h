// App/ToolScene.h - This file is part of NotedELN

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

// ToolScene.H

#ifndef TOOLSCENE_H

#define TOOLSCENE_H

#include <QGraphicsScene>

class ToolScene: public QGraphicsScene {
  Q_OBJECT;
public:
  ToolScene(class Mode *mode, QObject *parent=0);
  virtual ~ToolScene();
public:
  class Toolbars *toolbars() { return bars; }
  void showClock(bool);
  void moveClock(QRectF viewRect);
private:
  class Toolbars *bars;
  class ClockFace *clock;
  QRectF toolbarrect;
  QRectF adjustedrect;
};

#endif
