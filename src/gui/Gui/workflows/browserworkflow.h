#ifndef BROWSERWORKFLOW_H
#define BROWSERWORKFLOW_H

#include <QObject>

class BrowserWorkflow : public QObject {
  Q_OBJECT

public:
  explicit BrowserWorkflow(QObject *parent = 0);

signals:

public slots:
  void initialize();
};

#endif // BROWSERWORKFLOW_H
