// Book/Index.h - This file is part of NotedELN

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

// Index.H

#ifndef INDEX_H

#define INDEX_H

#include <QObject>
#include <QMap>
#include "Entry.h"

class Index: public QObject {
  Q_OBJECT;
public:
  Index(QString rootDir, class TOC *toc, QObject *parent);
  virtual ~Index();
  void watchEntry(Entry *);
  void unwatchEntry(Entry *);
  void deleteEntry(Entry *);
  class WordIndex *words() const;
public slots:
  void updateEntry(QObject *);
  void flush();
private:
  class WordIndex *widx;
  QMap<int, QSet<QString> > oldsets;
  QString rootdir;
  class QSignalMapper *mp;
  bool needToSave;
  class QTimer *saveTimer;
  QMap<QObject *, QMetaObject::Connection> cons;
};

#endif
