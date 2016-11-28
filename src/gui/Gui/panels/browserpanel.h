#ifndef BROWSERPANEL_H
#define BROWSERPANEL_H

#include <QWidget>

namespace Ui {
class BrowserPanel;
}

class BrowserPanel : public QWidget {
  Q_OBJECT

public:
  explicit BrowserPanel(QWidget *parent = 0);
  ~BrowserPanel();

private:
  Ui::BrowserPanel* m_ui;
};

#endif // BROWSERPANEL_H
