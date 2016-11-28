#ifndef EDITORPANEL_H
#define EDITORPANEL_H

#include <QWidget>

namespace Ui {
class EditorPanel;
}

class EditorPanel : public QWidget {
  Q_OBJECT

public:
  explicit EditorPanel(QWidget *parent = 0);
  ~EditorPanel();

private:
  Ui::EditorPanel* m_ui;
};

#endif // EDITORPANEL_H
