#include "project.h"
#include "tinyxml2.h"
#include <iostream>
#include <memory>


//
//static msbuild::project_element get_element_type(const std::string &name) {
//  using namespace msbuild;
//  static struct element_name_pair_t {
//    constexpr element_name_pair_t(const project_element el_type, const char *el_name)
//      : type{ el_type }, name{ el_name } {
//    }
//    const project_element type;
//    const char *name;
//  } g_elements[] {
//    { project_element::choose, "Choose" },
//    { project_element::import, "Import" },
//    { project_element::import_group, "ImportGroup" },
//    { project_element::item, "Item" },
//    { project_element::item_definition_group, "ItemDefinitionGroup" },
//    { project_element::item_group, "ItemGroup" },
//    { project_element::item_metadata, "ItemMetadata" },
//    { project_element::on_error, "OnError" },
//    { project_element::otherwise, "Otherwise" },
//    { project_element::output, "Output" },
//    { project_element::parameter, "Parameter" },
//    { project_element::parameter_group, "ParameterGroup" },
//    { project_element::project, "Project" },
//    { project_element::project_extensions, "ProjectExtensions" },
//    { project_element::property_group, "PropertyGroup" },
//    { project_element::target, "Target" },
//    { project_element::task, "Task" },
//    { project_element::task_body, "TaskBody" },
//    { project_element::using_task, "UsingTask" },
//    { project_element::when, "When" }
//  };
//  constexpr const int g_element_count = (sizeof(g_elements) / sizeof(*g_elements));
//
//  for (int i = 0; i < g_element_count; i++) {
//    if (name.compare(g_elements[i].name) == 0)
//      return g_elements[i].type;
//  };
//
//  return msbuild::project_element::none;
//}
//
//class item_definition_group {
//  static constexpr const char *element_name = "ItemDefinitionGroup";
//};
//
//static const char *getXmlErrorString(tinyxml2::XMLError code) {
//  return code == tinyxml2::XML_ERROR_FILE_COULD_NOT_BE_OPENED
//    ? "File could not be opened"
//    : code == tinyxml2::XML_ERROR_FILE_NOT_FOUND
//    ? "File not found"
//    : code == tinyxml2::XML_ERROR_FILE_READ_ERROR
//    ? "File read error"
//    : "Unknown error";
//}
//
//bool msbuild::project::load(const std::string &filename) {
//  m_filename = filename;
//
//  tinyxml2::XMLDocument doc;
//  auto ret = doc.LoadFile(m_filename.c_str());
//  if (ret != tinyxml2::XML_NO_ERROR) {
//    std::cerr << "Failed to load project '" << filename << "': " << getXmlErrorString(ret) << std::endl;
//    return false;
//  }
//
//  auto xmlProject = doc.FirstChildElement("Project");
//  if (xmlProject == nullptr) {
//    std::cerr << "Unexpected document format in project '" << filename << "': " << "element 'Project' not found" << std::endl;
//    return false;
//  }
//  
//  for (auto child = xmlProject->FirstChildElement(); child != nullptr; child = child->NextSiblingElement()) {
//    auto name = child->Name();
//    auto type = get_element_type(name);
//
//    if (type == project_element::none) {
//      std::cerr << "Unexpected document format in project '" << filename << "': " << "element '" << name << "' not expected" << std::endl;
//      return false;
//    }
//    
//    return true;
//  }

void msbuild::project::set_property(const std::string &name, const std::string &value) {
  std::cout << ".." << name << " = " << value << std::endl;

}

void msbuild::project::add_item(const std::string &name, const tinyxml2::XMLElement* node, uint32_t sequence) {
  if (name == item_definition_group::get_name()) {
    auto importGroup = create<item_definition_group>(node, sequence);
  } else if (name == item_group::get_name()) {
    auto importGroup = create<item_group>(node, sequence); 
  } else if (name == property_group::get_name()) {
    auto importGroup = create<property_group>(node, sequence);
  } else if (name == import_group::get_name()) {
    auto importGroup = create<import_group>(node, sequence);
  }
}

void msbuild::property::set_property(const std::string& name, const std::string &value) {}

void msbuild::property::add_item(const std::string& name, const tinyxml2::XMLElement* node, uint32_t sequence) {}

void msbuild::property_group::set_property(const std::string& name, const std::string &value) {}

void msbuild::property_group::add_item(const std::string& name, const tinyxml2::XMLElement* node, uint32_t sequence) {}

void msbuild::item_group::set_property(const std::string& name, const std::string &value) {}

void msbuild::item_group::add_item(const std::string& name, const tinyxml2::XMLElement* node, uint32_t sequence) {}

void msbuild::item_definition_group::set_property(const std::string& name, const std::string &value) {}

void msbuild::item_definition_group::add_item(const std::string& name, const tinyxml2::XMLElement* node, uint32_t sequence) {}

void msbuild::import_group::set_property(const std::string& name, const std::string &value) {}

void msbuild::import_group::add_item(const std::string& name, const tinyxml2::XMLElement* node, uint32_t sequence) {}