// App/DeletedItem.cpp - This file is part of NotedELN

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

// DeletedItem.C

#include "DeletedItem.h"
#include "ElnAssert.h"
#include "BlockItem.h"

DeletedItem::DeletedItem(Item *item) {
  ASSERT(item);
  data = item->data();
  ASSERT(data);
  parentItem = item->parent();
  if (parentItem)
    connect(parentItem, &QObject::destroyed,
            this, &DeletedItem::parentDestroyed);
}

DeletedItem::~DeletedItem() {
}

DeletedItem *DeletedItem::takeFromParent(Item *item) {
  ASSERT(item);
  BlockItem *ancestor = item->ancestralBlock();
  DeletedItem *dd = new DeletedItem(item);
  ASSERT(item->data()->parent());
  item->data()->parent()->takeChild(item->data());
  item->deleteLater();
  if (ancestor)
    ancestor->sizeToFit();
  return dd;
}

bool DeletedItem::isRestored() const {
  if (isOrphaned())
    return false;
  return parentItem->data()->children<Data>().contains(data);
}

Item *DeletedItem::restoreToParent() {
  ASSERT(!isOrphaned());
  ASSERT(!isRestored());
  parentItem->data()->addChild(data);
  Item *item = Item::create(data, parentItem);
  item->makeWritable();
  BlockItem *ancestor = item->ancestralBlock();
  if (ancestor)
    ancestor->sizeToFit();
  return item;
}

void DeletedItem::parentDestroyed() {
  parentItem = 0;
}

bool DeletedItem::isOrphaned() const {
  return parentItem == 0;
}

