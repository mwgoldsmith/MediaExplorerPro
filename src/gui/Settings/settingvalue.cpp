#include "SettingValue.h"

SettingValue::SettingValue(const QString& key, SettingValue::Type type, const QVariant& defaultValue,
  const char* name, const char* desc, SettingValue::Flags flags) :
  m_val(defaultValue),
  m_defaultVal(defaultValue),
  m_key(key),
  m_name(name),
  m_desc(desc),
  m_type(type),
  m_flags(flags),
  m_initLoad(true) {
}

void SettingValue::set(const QVariant& _val) {
  auto val = _val;
  if (val != m_val) {
    if ((m_flags & Clamped) != 0) {
      if (m_min.isValid() && val.toDouble() < m_min.toDouble()) {
        val = m_min;
      }
      if (m_max.isValid() && val.toDouble() > m_max.toDouble()) {
        val = m_max;
      }
    }
    if ((m_flags & EightMultiple) != 0) {
      val = (val.toInt() + 7) & ~7;
    }
    m_val = val;
    emit changed(m_val);
  } else if (m_initLoad) {
    emit changed(m_val);
  }
  m_initLoad = false;
}

const QVariant& SettingValue::get() {
  return m_val;
}

const char* SettingValue::description() const {
  return m_desc;
}

void SettingValue::restoreDefault() {
  set(m_defaultVal);
}

const QString& SettingValue::key() const {
  return m_key;
}

const char* SettingValue::name() const {
  return m_name;
}

SettingValue::Type SettingValue::type() const {
  return m_type;
}

SettingValue::Flags SettingValue::flags() const {
  return m_flags;
}

void SettingValue::setLimits(const QVariant& min, const QVariant& max) {
  Q_ASSERT_X(((m_flags & Clamped) != 0), "SettingValue", "Setting limits to a non-clamped value");
  if (min.isValid() == true)
    m_min = min;
  if (max.isValid() == true)
    m_max = max;
}

const QVariant& SettingValue::minValue() const {
  return m_min;
}

const QVariant& SettingValue::maxValue() const {
  return m_max;
}
