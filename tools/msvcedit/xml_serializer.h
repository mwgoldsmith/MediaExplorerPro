#ifndef XML_SERIALIZER_H
#define XML_SERIALIZER_H

#include <assert.h>
#include <string>
#include <iostream>
#include <memory>
#include "tinyxml2.h"
#include "serializable.h"

namespace msbuild {

template<typename T>
class xml_serializer {
public:
  /****
  * Constructors / destructors
  */
  explicit xml_serializer(const char *filename) : xml_serializer(std::string{ filename }) {
   // static_assert(is_base_of<serializable, T>::value, "type T must be derived from serializable");
  };

  explicit xml_serializer(const std::string &filename) : m_filename{ filename } {
   // static_assert(is_base_of<serializable, T>::value, "type T must be derived from serializable");
  };

  ~xml_serializer() = default;


public:
  /****
  * 
  */
  void serialize(std::unique_ptr<T> obj) {
    
  }

  std::unique_ptr<T> deserialize() {
//    tinyxml2::XMLDocument doc;
//    auto ret = doc.LoadFile(m_filename.c_str());
//    if (ret != tinyxml2::XML_NO_ERROR) {
//      std::cerr << "Failed to load project '" << m_filename << "': " << get_xml_error_string(ret) << std::endl;
//      return nullptr;
//    }
//
//    auto root = doc.FirstChild();// .FirstChildElement(typename T::get_name().c_str());
//    if (root == nullptr) {
//      std::cerr << "Unexpected document format in project '" << m_filename << "': " << "element 'Project' not found" << std::endl;
//      return nullptr;
//    }
//
//
    auto obj = std::make_unique<T>();
//    deserialize(obj, root, 0);
    return obj;
  }
//
//  template<typename U>
//  uint32_t deserialize(const std::unique_ptr<U> &parent, const tinyxml2::XMLElement* node, uint32_t start) {
//    for (auto child = node->FirstAttribute(); child != nullptr; child = child->Next()) {
//      std::string name = child->Name();
//      std::string value = child->Value();
//      std::cout << ".." << name << std::endl;
//
//      parent->set_property(name, value);
//    }
//
//    auto counter = 0;
//    for (auto child = node->FirstChildElement(); child != nullptr; child = child->NextSiblingElement()) {
//      auto name = child->Name();
//      ++counter;
//      std::cout << counter << " " << name << std::endl;
//
//      parent->add_item(name, child, start + counter);
//    }
//
//    return counter;
//  }
//
  template<typename U>
  std::unique_ptr<U> create(const tinyxml2::XMLElement* node, uint32_t sequence) {
    auto ret = std::make_unique<U>(sequence, m_depth + 1);
//
//    deserialize(ret, node);
//
    return ret;
  }
private:
  static const char *get_xml_error_string(tinyxml2::XMLError code) {
    return code == tinyxml2::XML_ERROR_FILE_COULD_NOT_BE_OPENED
      ? "File could not be opened"
      : code == tinyxml2::XML_ERROR_FILE_NOT_FOUND
      ? "File not found"
      : code == tinyxml2::XML_ERROR_FILE_READ_ERROR
      ? "File read error"
      : "Unknown error";
  }

private:
  static uint32_t m_sequence;
  std::string m_filename;
};


}
//
//namespace msbuild {
//
//template <typename B, typename D>
//struct is_base_of {
//  typedef char yes[1];
//  typedef char no[2];
//
//  // ReSharper disable CppFunctionIsNotImplemented
//  static yes& test(B*);
//  static no& test(...);
//  static D* get(void);
//
//  static const bool value = sizeof(test(get()) == sizeof(yes));
//  // ReSharper restore CppFunctionIsNotImplemented
//};
//
//

//template<typename T>
//uint32_t xml_serializer<T>::m_sequence{ 0 };
//
//
//
//
//}

#endif