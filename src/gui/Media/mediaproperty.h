#ifndef MEDIAPROPERTY_H
#define MEDIAPROPERTY_H

#include <memory>
#include <QObject>

class MediaProperty : public QObject {
  Q_OBJECT

  struct ObjectConcept {
    virtual ~ObjectConcept() {}
  };

  template<typename T>
  struct ObjectModel : ObjectConcept {
    ObjectModel(const T& t) : object(t) {}
    virtual ~ObjectModel() {}

  private:
    T object;
  };

public:
  template<typename T>
  MediaProperty(const QString& name, const T& val, QObject *parent = 0)
    : QObject(parent)
    , m_name(name)
    , m_object(new ObjectModel<T>(val)) {}

  QString name() const;

signals:
  void valueChanged();

public slots:

private:
  const QString                  m_name;
  std::shared_ptr<ObjectConcept> m_object;
};

#endif // MEDIAPROPERTY_H
