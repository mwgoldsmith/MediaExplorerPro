#ifndef CLIPPROPERTIESDOCK_H
#define CLIPPROPERTIESDOCK_H

#include <QWidget>

namespace Ui {
class ClipPropertiesDock;
}

class ClipPropertiesDock : public QWidget {
  Q_OBJECT

public:
  explicit ClipPropertiesDock(QWidget *parent = 0);
  ~ClipPropertiesDock();

private:
  Ui::ClipPropertiesDock* m_ui;
};

#endif // CLIPPROPERTIESDOCK_H
