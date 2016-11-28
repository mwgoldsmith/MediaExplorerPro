#ifndef EDITORWORKFLOW_H
#define EDITORWORKFLOW_H

#include <QObject>

class EditorWorkflow : public QObject {
  Q_OBJECT

public:
  explicit EditorWorkflow(QObject *parent = 0);

signals:

public slots:
  void initialize();

};

#endif // EDITORWORKFLOW_H
