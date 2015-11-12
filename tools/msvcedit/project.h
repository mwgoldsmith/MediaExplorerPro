#ifndef PROJECT_H
#define PROJECT_H

#include "msbuild_edit.h"
#include "tinyxml2.h"
#include "serializable.h"
#include "serializable.h"

namespace msbuild {

class project : public serializable {
public:
  project() = default;
  project (uint32_t sequence, uint32_t depth) : serializable(sequence, depth) {}
  virtual ~project() = default;

  static std::string get_name() {
    static std::string name{m_name};
    return name;
  }

  virtual void set_property(const std::string& name, const std::string& value) override;

  virtual void add_item(const std::string& name, const tinyxml2::XMLElement* node, uint32_t sequence) override;

private:
  static constexpr const char* m_name = "Project";
};

}

//
//template<typename T>
//struct type2name {
//  explicit constexpr type2name(const char* n) : name{n} {}
//
//  typedef T type;
//  const char* name;
//};
//
////
////
////template<typename P>
////class element_factory {
////  template<class T, class U>
////  T* create(const U& arg, type2type<T>) {
////    return new T(arg);
////  }
////};
////
////enum project_elements {
////  none,
////  choose,
////  import,
////  import_group,
////  item,
////  item_definition_group,
////  item_group,
////  item_metadata,
////  on_error,
////  otherwise,
////  output,
////  parameter,
////  parameter_group,
////  project,
////  project_extensions,
////  property,
////  property_group,
////  target,
////  task,
////  task_body,
////  using_task,
////  when
////};
//
///**
//*
//*/
//class project : public serializable {
//public:
//  project() = default;
//  project (uint32_t sequence, uint32_t depth) : serializable(sequence, depth) {}
//  virtual ~project() = default;
//
//  static std::string get_name() {
//    static std::string name{m_name};
//    return name;
//  }
//
//  virtual void set_property(const std::string& name, const std::string& value) override;
//
//  virtual void add_item(const std::string& name, const tinyxml2::XMLElement* node, uint32_t sequence) override;
//
//private:
//  static constexpr const char* m_name = "Project";
//};
//
///**
//*
//*/
//class property : public serializable {
//public:
//  property() = default;
//  property(uint32_t sequence, uint32_t depth) : serializable(sequence, depth) {}
//  virtual ~property() = default;
//
//  std::string get_name() const {
//    return m_name;
//  }
//
//  virtual void set_property(const std::string& name, const std::string& value) override;
//
//  virtual void add_item(const std::string& name, const tinyxml2::XMLElement* node, uint32_t sequence) override;
//
//private:
//  std::string m_name;
//};
//
///**
//*
//*/
//class property_group : public serializable {
//public:
//  property_group() = default;
//  property_group(uint32_t sequence, uint32_t depth) : serializable(sequence, depth) {}
//  virtual ~property_group() = default;
//
//  static std::string get_name() {
//    static std::string name{m_name};
//    return name;
//  }
//
//  virtual void set_property(const std::string& name, const std::string& value) override;
//
//  virtual void add_item(const std::string& name, const tinyxml2::XMLElement* node, uint32_t sequence) override;
//
//private:
//  static constexpr const char* m_name = "PropertyGroup";
//};
//
///**
//*
//*/
//class item_group : public serializable {
//public:
//  item_group() = default;
//  item_group(uint32_t sequence, uint32_t depth) : serializable(sequence, depth) {}
//  virtual ~item_group() = default;
//
//  static std::string get_name() {
//    static std::string name{m_name};
//    return name;
//  }
//
//  virtual void set_property(const std::string& name, const std::string& value) override;
//
//  virtual void add_item(const std::string& name, const tinyxml2::XMLElement* node, uint32_t sequence) override;
//
//private:
//  static constexpr const char* m_name = "ItemGroup";
//};
//
///**
//*
//*/
//class item_definition_group : public serializable {
//public:
//  item_definition_group() = default;
//  item_definition_group(uint32_t sequence, uint32_t depth) : serializable(sequence, depth) {}
//  virtual ~item_definition_group() = default;
//
//  static std::string get_name() {
//    static std::string name{m_name};
//    return name;
//  }
//
//  virtual void set_property(const std::string& name, const std::string& value) override;
//
//  virtual void add_item(const std::string& name, const tinyxml2::XMLElement* node, uint32_t sequence) override;
//
//private:
//  static constexpr const char* m_name = "ItemDefinitionGroup";
//};
//
//class import_group : public serializable {
//public:
//  import_group() = default;
//  import_group(uint32_t sequence, uint32_t depth) : serializable(sequence, depth) {}
//  virtual ~import_group() = default;
//
//  static std::string get_name() {
//    static std::string name{m_name};
//    return name;
//  }
//
//  virtual void set_property(const std::string& name, const std::string& value) override;
//
//  virtual void add_item(const std::string& name, const tinyxml2::XMLElement* node, uint32_t sequence) override;
//
//
//private:
//  static constexpr const char* m_name = "ImportGroup";
//};
//
///**
//*
//*/
////template<>
////class element_factory<project> {
////  template <class U>
////  property_group* create(const U& arg, type2type<property_group>) {
////    return new property_group(arg);
////  }
////
////  template <class U>
////  item_definition_group* create(const U& arg, type2type<item_definition_group>) {
////    return new item_definition_group(arg);
////  }
////};
//


#endif
