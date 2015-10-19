// LateNoteFile.cpp

#include "LateNoteFile.h"

#include <QDebug>
#include "Assert.h"
#include "UUID.h"

static QString basicFilename(QString uuid) {
  return QString("%1") . arg(uuid);
}

LateNoteFile *createLateNoteFile(QDir const &dir, QObject *parent) {
  QString uuid = UUID::create(32);
  QString fn0 = basicFilename(uuid);
  QString pfn = dir.absoluteFilePath(fn0 + ".json");
  LateNoteFile *f = LateNoteFile::create(pfn, parent);
  if (!f)
    return 0;
  f->data()->setUuid(uuid);
  return f;
}

LateNoteFile *loadLateNoteFile(QDir const &dir, QString uuid, QObject *parent) {
  QString fn0 = basicFilename(uuid);
  QString pfn = dir.absoluteFilePath(fn0 + ".json");
  LateNoteFile *f = LateNoteFile::load(pfn, parent);
  if (!f)
    return 0;

  return f;
}

bool deleteLateNoteFile(QDir dir, QString uuid) {
  QString fn0 = basicFilename(uuid);
  QString jsonfn = fn0 + ".json";
  dir.remove(jsonfn + "~");
  bool ok = dir.rename(jsonfn, jsonfn + "~");
  return ok;
}

