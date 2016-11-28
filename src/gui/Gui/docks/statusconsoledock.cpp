#include "statusconsoledock.h"
#include "ui_statusconsoledock.h"

#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QMutexLocker>
#include <QDateTime>

StatusConsoleDock::StatusConsoleDock(QWidget *parent)
  : QWidget(parent)
  , m_ui(new Ui::StatusConsoleDock) {
  m_ui->setupUi(this);

  m_statusConsoleTextEdit = new QPlainTextEdit(this);
  m_statusConsoleTextEdit->setObjectName(QStringLiteral("textEdit"));

  auto font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
  m_statusConsoleTextEdit->setFont(font);
  m_statusConsoleTextEdit->setReadOnly(true);
  QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
  sizePolicy.setHorizontalStretch(0);
  sizePolicy.setVerticalStretch(0);
  sizePolicy.setHeightForWidth(m_statusConsoleTextEdit->sizePolicy().hasHeightForWidth());

  m_statusConsoleTextEdit->setSizePolicy(sizePolicy);
  m_statusConsoleTextEdit->setFrameShape(QFrame::Box);
  m_statusConsoleTextEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  m_statusConsoleTextEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  m_statusConsoleTextEdit->setUndoRedoEnabled(false);
  m_statusConsoleTextEdit->setLineWrapMode(QPlainTextEdit::NoWrap);

  auto layout = new QVBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(m_statusConsoleTextEdit);
  setLayout(layout);
}

StatusConsoleDock::~StatusConsoleDock() {
  delete m_ui;
}

void StatusConsoleDock::append(const QString& msg) {
  QMutexLocker locker(&m_mutex);
  QDateTime dateTime = QDateTime::currentDateTime();
  QString output = QString("%1:\t%2").arg(dateTime.toString("yyyy-MM-dd hh:mm:ss")).arg(msg);

  m_statusConsoleTextEdit->appendPlainText(output);
}

void StatusConsoleDock::reset() {
  QMutexLocker locker(&m_mutex);
  m_statusConsoleTextEdit->clear();
}
