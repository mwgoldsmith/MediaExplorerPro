
#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#define MXP_CREATE_PREFERENCE( type, key, defaultValue, name, desc, flags )  \
        Core::getInstance()->getSettings()->createVar( type, key, defaultValue, name, desc, flags );

/// Vlmc preferences macros
#define MXP_CREATE_PREFERENCE_INT( key, defaultValue, name, desc )  \
        MXP_CREATE_PREFERENCE( SettingValue::Int, key, defaultValue, name, desc, SettingValue::Nothing )
#define MXP_CREATE_PREFERENCE_STRING( key, defaultValue, name, desc )  \
        MXP_CREATE_PREFERENCE( SettingValue::String, key, defaultValue, name, desc, SettingValue::Nothing )
#define MXP_CREATE_PREFERENCE_DOUBLE( key, defaultValue, name, desc )  \
        MXP_CREATE_PREFERENCE( SettingValue::Double, key, defaultValue, name, desc, SettingValue::Nothing )
#define MXP_CREATE_PREFERENCE_LANGUAGE( key, defaultValue, name, desc )  \
        MXP_CREATE_PREFERENCE( SettingValue::Language, key, defaultValue, name, desc, SettingValue::Nothing )
#define MXP_CREATE_PREFERENCE_KEYBOARD( key, defaultValue, name, desc )  \
        MXP_CREATE_PREFERENCE( SettingValue::KeyboardShortcut, key, defaultValue, name, desc, SettingValue::Nothing )
#define MXP_CREATE_PREFERENCE_BOOL( key, defaultValue, name, desc )  \
        MXP_CREATE_PREFERENCE( SettingValue::Bool, key, defaultValue, name, desc, SettingValue::Nothing )
#define MXP_CREATE_PREFERENCE_PATH( key, defaultValue, name, desc )  \
        MXP_CREATE_PREFERENCE( SettingValue::Path, key, defaultValue, name, desc, SettingValue::Nothing )

#include "SettingValue.h"

#include <QString>
#include <QMap>
#include <QPair>
#include <QObject>
#include <QReadWriteLock>
#include <QVariant>
#include <QFile>
#include <QJsonDocument>

class Settings : public QObject {
  Q_OBJECT

public:
  typedef QList<SettingValue*>          SettingList;
  typedef QMap<QString, SettingValue*>  SettingMap;

  Settings();
  explicit Settings(const QString& settingsFile);
  ~Settings();

  bool setValue(const QString &key, const QVariant &value);
  SettingValue* value(const QString &key);
  SettingValue* createVar(SettingValue::Type type, const QString &key, const QVariant &defaultValue, const char *name, const char *desc, SettingValue::Flags flags);
  SettingList group(const QString &groupName) const;
  bool load();
  bool save();
  void addSettings(const QString& name, Settings& settings);
  void restoreDefaultValues();
  void setSettingsFile(const QString& settingsFile);

private:
  SettingMap                       m_settings;
  mutable QReadWriteLock           m_rwLock;
  QFile*                           m_settingsFile;
  QList<QPair<QString, Settings*>> m_settingsChildren;

  QJsonDocument readSettingsFromFile();
  void loadJsonFrom(const QJsonObject& object);
  void saveJsonTo(QJsonObject& object);

signals:
  void postLoad();
  void preSave();
};

#endif
