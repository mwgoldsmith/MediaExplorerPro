#ifndef STATUSCONSOLEDOCK_H
#define STATUSCONSOLEDOCK_H

#include <QWidget>
#include <QMutex>

namespace Ui {
class StatusConsoleDock;
}

class QPlainTextEdit;

class StatusConsoleDock : public QWidget {
  Q_OBJECT

public:
  explicit StatusConsoleDock(QWidget *parent = 0);
  ~StatusConsoleDock();

public slots:
  void append(const QString& msg);
  void reset();

private:
  Ui::StatusConsoleDock* m_ui;
  QPlainTextEdit*        m_statusConsoleTextEdit;
  QMutex                 m_mutex;
};

#endif // STATUSCONSOLEDOCK_H
