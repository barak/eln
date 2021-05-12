// Data/EntryData.H - This file is part of NotedELN

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

// EntryData.H

#ifndef ENTRYDATA_H

#define ENTRYDATA_H

#include "Data.h"
#include <QList>
#include <QPointer>

class EntryData: public Data {
  Q_OBJECT;
  Q_PROPERTY(int startPage READ startPage WRITE setStartPage)
  Q_PROPERTY(bool unlocked READ isUnlocked WRITE setUnlocked)
  Q_PROPERTY(int stampTime READ stampTime WRITE setStampTime)
  Q_PROPERTY(QString cui READ cui WRITE setCui)
public:
  // constructor and destructor  
  EntryData(Data *parent=0);
  virtual ~EntryData();
  // read properties
  int startPage() const;
  bool isUnlocked() const;
  int stampTime() const;
  QString cui() const;
  /* stampTime > 0 means: always stamp; < 0 means: never stamp; = 0 means
     stamp if auto-timestamp-min-dt is exceeded. */
  // write properties
  void setStartPage(int);
  void setUnlocked(bool);
  void setStampTime(int);
  void setCui(QString);
  // other
  virtual bool isWritable() const;
  void resetCreation(); // only works if isempty.
  virtual bool lateNotesAllowed() const;
  class TitleData *title();
  QString titleText();
  QList<class BlockData *> blocks() const;
  void addBlock(BlockData *); // we become owner
  void insertBlockBefore(BlockData *newBlock, Data *ref); // we become owner
  bool deleteBlock(BlockData *);
  int sheetCount() const;
  void setBook(class Notebook *nb);      // } This is for use by items that 
  Notebook *book() const;                // } wish to use resources. We don't
  ResManager *resManager() const;        // } use it ourselves.
  EntryData const *entry() const;
  EntryData *entry();
  bool isEmpty() const; // true iff no blocks and title is default
  virtual void markModified(ModType mt=UserVisibleMod);
signals:
  void titleMod();
  void sheetCountMod();
  void emptyStatusChanged(bool empty);
protected:
  virtual void loadMore(QVariantMap const &src);
private slots:
  void newSheet();
protected:
  int startPage_;
  bool unlocked_;
  int stampTime_;
  QString cui_;
  int maxSheet;
  Notebook *nb;
  bool wasEmpty;
};

#endif
