#include "Settings.h"
#include "SettingValue.h"

#include <QJsonObject>
#include <QDir>
#include "core.h"

Settings::Settings()
  : m_settingsFile(nullptr) {
}

Settings::Settings(const QString &settingsFile)
  : m_settingsFile(nullptr) {
  setSettingsFile(settingsFile);
}

Settings::~Settings() {
  qDeleteAll(m_settings);
  delete m_settingsFile;
}

void Settings::setSettingsFile(const QString &settingsFile) {
  delete m_settingsFile;

  if (settingsFile.isEmpty() == false) {
    QFileInfo fInfo(settingsFile);
    if (fInfo.exists() == false) {
      auto dir = fInfo.dir();
      if (dir.exists() == false) {
        dir.mkpath(fInfo.absolutePath());
      }
    }
    m_settingsFile = new QFile(settingsFile);
  } else {
    m_settingsFile = nullptr;
  }
}

QJsonDocument Settings::readSettingsFromFile() {
  if (m_settingsFile->open(QFile::ReadOnly) == false) {
    auto msg = QString("Failed to open settings file %1").arg(m_settingsFile->fileName()); 
    //Core::getInstance()->statusMessage(msg);
    return QJsonDocument(QJsonObject());
  }
  QJsonParseError error;
  auto doc = QJsonDocument::fromJson(m_settingsFile->readAll(), &error);
  if (error.error != QJsonParseError::NoError) {
    auto msg = QString("Failed to open settings file %1: %2").arg(m_settingsFile->fileName()).arg(error.errorString());
    //Core::getInstance()->statusMessage(msg);
    return QJsonDocument(QJsonObject());
  }

  m_settingsFile->close();
  return doc;
}

bool Settings::load() {
  if (m_settingsFile == nullptr)
    return false;

  auto top = readSettingsFromFile().object();
  loadJsonFrom(top);

  for (const auto& child : m_settingsChildren) {
    child.second->loadJsonFrom(top[child.first].toObject());
  }

  return true;
}

bool Settings::save() {
  if (m_settingsFile == nullptr)
    return false;

  QReadLocker lock(&m_rwLock);

  auto doc = readSettingsFromFile();
  auto top = doc.object();
  saveJsonTo(top);

  for (const auto& child : m_settingsChildren) {
    QJsonObject object;
    child.second->saveJsonTo(object);
    top.insert(child.first, QJsonValue(object));
  }

  doc.setObject(top);

  m_settingsFile->open(QFile::WriteOnly);
#ifdef NDEBUG
  m_settingsFile->write(doc.toJson(QJsonDocument::Compact));
#else
  m_settingsFile->write(doc.toJson(QJsonDocument::Indented));
#endif
  m_settingsFile->close();

  return true;
}

void Settings::loadJsonFrom(const QJsonObject &object) {
  for (auto it = object.constBegin(); it != object.constEnd(); ++it) {
    // Check if the key is a child settings'
    auto isChildSettings = false;
    if ((*it).type() == QJsonValue::Object) {
      for (const auto& pair : m_settingsChildren) {
        if (pair.first == it.key()) {
          isChildSettings = true;
          break;
        }
      }
    }

    if (isChildSettings == true)
      continue;

    auto val = value(it.key());
    if (val == nullptr) {
      auto msg = QString("Loaded invalid project setting: %1").arg(it.key());
      Core::getInstance()->statusMessage(msg);
    }

    if (val->type() == SettingValue::ByteArray) {
      val->set(QByteArray::fromBase64((*it).toVariant().toByteArray()));
    } else {
      val->set((*it).toVariant());
    }
  }

  emit postLoad();
}

void Settings::saveJsonTo(QJsonObject &object) {
  emit preSave();
  for (const auto& val : m_settings) {
    if ((val->flags() & SettingValue::Runtime) != 0) {
      continue;
    }

    if (val->type() == SettingValue::ByteArray) {
      object.insert(val->key(), QJsonValue(QString(val->get().toByteArray().toBase64())));
    } else {
      object.insert(val->key(), QJsonValue::fromVariant(val->get()));
    }
  }
}

void Settings::addSettings(const QString &name, Settings &settings) {
  m_settingsChildren << qMakePair(name, &settings);
}

bool Settings::setValue(const QString &key, const QVariant &value) {
  auto it = m_settings.find(key);
  if (it != m_settings.end()) {
    (*it)->set(value);
    return true;
  }
  Q_ASSERT_X(false, __FILE__, "setting value without a created variable");
  return false;
}

void Settings::restoreDefaultValues() {
  QReadLocker lock(&m_rwLock);
  for (auto s : m_settings) {
    s->restoreDefault();
  }
  for (auto &pair : m_settingsChildren) {
    pair.second->restoreDefaultValues();
  }
}

SettingValue* Settings::value(const QString &key) {
  QReadLocker lock(&m_rwLock);

  auto it = m_settings.find(key);
  if (it != m_settings.end())
    return *it;

  Q_ASSERT_X(false, __FILE__, "fetching value without a created variable");
  return nullptr;
}

SettingValue* Settings::createVar(SettingValue::Type type, const QString &key, const QVariant &defaultValue, const char *name, const char *desc, SettingValue::Flags flags) {
  QWriteLocker lock(&m_rwLock);
  if (m_settings.contains(key)) {
    return nullptr;
  }

  auto val = new SettingValue(key, type, defaultValue, name, desc, flags);
  m_settings.insert(key, val);
  return val;
}

Settings::SettingList Settings::group(const QString &groupName) const {
  QReadLocker lock(&m_rwLock);
  SettingList ret;

  auto grp = groupName + '/';
  for (auto s : m_settings) {
    if (s->key().startsWith(grp))
      ret.push_back(s);
  }

  return ret;
}
