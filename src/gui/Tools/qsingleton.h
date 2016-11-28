#ifndef QSINGLETON_H
#define QSINGLETON_H

#include <stdlib.h>
#include <QWidget>

template <typename T>
class QSingleton {
public:
  static T* getInstance(QWidget* parent = NULL) {
    if (m_instance == NULL)
      m_instance = new T(parent);
    return m_instance;
  }

  static void destroyInstance() {
    if (m_instance != NULL) {
      delete m_instance;
      m_instance = NULL;
    }
  }

protected:
  QSingleton(){}
  virtual ~QSingleton(){}

  //Not implemented since these methods should *NEVER* been called. If they do, it probably won't compile :)
  QSingleton(const QSingleton<T>&);
  QSingleton<T>& operator=(const QSingleton<T>&);

private:
  static T* m_instance;
};

template <typename T>
T* QSingleton<T>::m_instance = NULL;

#endif // QSINGLETON_H
