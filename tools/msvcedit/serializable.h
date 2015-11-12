#ifndef SERIALIAZABLE_H
#define SERIALIAZABLE_H

#include "tinyxml2.h"
#include <memory>
#include <unordered_map>
#include <iostream>

namespace msbuild {

class serializable {
  typedef tinyxml2::XMLDocument xml_document;
  typedef tinyxml2::XMLElement  xml_element;

protected:
  serializable(uint32_t sequence, uint32_t depth)
    : m_attributes{ std::make_unique<std::unordered_map<std::string, std::string>>() }
    , m_children{ std::make_unique<std::vector<serializable>>() }
    , m_sequence{ sequence }
    , m_depth{ depth } {
  }

  explicit serializable(const xml_element* node) : serializable{ 0, 0 } {
    deserialize(node, m_sequence, m_depth);
  }

  virtual ~serializable() = default;

  virtual void set_property(const std::string& name, const std::string& value) = 0;
  virtual void add_item(const std::string& name, const xml_element* node, uint32_t sequence) = 0;

private:
  uint32_t deserialize(const xml_element* node, uint32_t sequenc, uint32_t depth) {
    for (auto child = node->FirstAttribute(); child != nullptr; child = child->Next()) {
      std::string name = child->Name();
      std::string value = child->Value();
      m_attributes->emplace(name, value);

      //set_property(name, value);
    }

    uint32_t counter = 0;
    for (auto child = node->FirstChildElement(); child != nullptr; child = child->NextSiblingElement()) {
      auto name = child->Name();

      auto kids = deserialize(child, m_sequence + counter + 1, m_depth + 1);
      counter += kids;

      std::cout << m_sequence << " " << name << std::endl;

    //  add_item(name, child, m_sequence);
    }

    return counter;
  }
private:
  const std::unique_ptr<std::unordered_map<std::string, std::string>> m_attributes;
  const std::unique_ptr<std::vector<serializable>> m_children;
  const uint32_t m_sequence;
  const uint32_t m_depth;
};

//class serializable {

//  //template<typename T>
//  //std::unique_ptr<T> create(const tinyxml2::XMLElement* node, uint32_t sequence) {
//  //  auto ret = std::make_unique<T>(sequence, m_depth + 1);
//  //
//  //  ret->deserialize(node);
//  //
//  //  return ret;
//  //}

//
//};

}
 
#endif