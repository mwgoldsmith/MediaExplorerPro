#ifndef DOWNLOADSWORKFLOW_H
#define DOWNLOADSWORKFLOW_H

#include <QObject>

class DownloadsWorkflow : public QObject {
  Q_OBJECT

public:
  explicit DownloadsWorkflow(QObject *parent = 0);

signals:

public slots:
  void initialize();
};

#endif // DOWNLOADSWORKFLOW_H
