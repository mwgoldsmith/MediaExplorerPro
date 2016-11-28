#ifndef DOWNLOADSPANEL_H
#define DOWNLOADSPANEL_H

#include <QWidget>

namespace Ui {
class DownloadsPanel;
}

class DownloadsPanel : public QWidget {
  Q_OBJECT

public:
  explicit DownloadsPanel(QWidget *parent = 0);
  ~DownloadsPanel();

private:
  Ui::DownloadsPanel* m_ui;
};

#endif // DOWNLOADSPANEL_H
