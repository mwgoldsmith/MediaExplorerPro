#ifndef CLIPVIEWERDOCK_H
#define CLIPVIEWERDOCK_H

#include <QWidget>

namespace Ui {
class ClipViewerDock;
}

class ClipViewerDock : public QWidget
{
  Q_OBJECT

public:
  explicit ClipViewerDock(QWidget *parent = 0);
  ~ClipViewerDock();

private:
  Ui::ClipViewerDock *ui;
};

#endif // CLIPVIEWERDOCK_H
