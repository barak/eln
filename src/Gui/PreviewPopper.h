// App/PreviewPopper.H - This file is part of NotedELN

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

// PreviewPopper.H

#ifndef PREVIEWPOPPER_H

#define PREVIEWPOPPER_H

#include <QWidget>

class PreviewPopper: public QObject {
  Q_OBJECT;
public:
  PreviewPopper(class Resource *res, QRect over, QObject *parent);
  virtual ~PreviewPopper();
  QWidget *popup();
  /* POPUP - Open popup immediately and return widget pointer
     Returns zero if resource not available.
  */
  void closeSoon();
  /* CLOSESOON - Close popup soon, unless mouse enters it. */
signals:
  void clicked(Qt::KeyboardModifiers);
  /* CLICKED - Emitted when the popup is clicked */
private slots:
  void closeAndDie();
  void timeout();
private:
  void smartPosition();
private:
  Resource *res;
  QRect over;
  class PopLabel *widget;
  class QTimer *timer;
};

#endif
