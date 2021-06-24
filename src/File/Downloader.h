// File/Downloader.h - This file is part of NotedELN

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

// Downloader.h

#ifndef DOWNLOADER_H

#define DOWNLOADER_H

#include <QUrl>
#include <QObject>

class Downloader: public QObject {
  Q_OBJECT;
public:
  Downloader(QUrl url, QObject *parent=0);
  virtual ~Downloader();
  void start();
  void start(QString dest);
  /* START - Starts the loading process
     Upon completion, FINISHED() is emitted, whether or not successful.
  */
  bool isComplete() const;
  /* ISCOMPLETE - Returns true if finished without error */
  bool isFailed() const;
  /* ISFAILED - Returns true if finished with error */
  QString mimeType() const;
  /* MIMETYPE - Mime type of downloaded resource
     May return "" if not (yet) known. */
  static QString mime2ext(QString);
  /* MIME2EXT - Convert a mime type to a file extension.
     Currently only knows about "pdf" and "html". Returns "" if unknown.
   */
  QByteArray data() const; // only available if started w/o filename
  QUrl source() const;
  static int maxDownloadLength();
  QString error() const;
signals:
  void finished();
  /* FINISHED - Emitted when process finishes.
     Emitted whether or not the download was successful.
  */
private:
  void startDownload();
private slots:
  void qnrFinished();
  void qnrDataAv();
private:
  class QFile *dst;
  QByteArray dat;
  class QNetworkReply *qnr;
  QUrl src;
  bool started;
  bool ok;
  bool err;
  int N;
  int redirectCount;
  QString errs;
  QString mimetype;
private:
  static class QNetworkAccessManager &networkAccessManager();
};

#endif
