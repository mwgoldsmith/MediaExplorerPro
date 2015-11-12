#include "xml_serializer.h"
#include "tinyxml2.h"
#include <iostream>


//template<typename U>
//struct element_name {
//  constexpr element_name(const U el_type, const char *el_name)
//    : type{ el_type }, name{ el_name } {
//  }
//  const U     type;
//  const char *name;
//};
/*
template<typename U>
U get_element_type(const std::string &name) {
  static struct element_name {
    constexpr element_name(const U el_type, const char *el_name)
      : type{ el_type }, name{ el_name } {
    }
    const U type;
    const char *name;
  } g_elements[]{
    { project_element::choose, "Choose" },
    { project_element::import, "Import" },
    { project_element::import_group, "ImportGroup" },
    { project_element::item, "Item" },
    { project_element::item_definition_group, "ItemDefinitionGroup" },
    { project_element::item_group, "ItemGroup" },
    { project_element::item_metadata, "ItemMetadata" },
    { project_element::on_error, "OnError" },
    { project_element::otherwise, "Otherwise" },
    { project_element::output, "Output" },
    { project_element::parameter, "Parameter" },
    { project_element::parameter_group, "ParameterGroup" },
    { project_element::project, "Project" },
    { project_element::project_extensions, "ProjectExtensions" },
    { project_element::property_group, "PropertyGroup" },
    { project_element::target, "Target" },
    { project_element::task, "Task" },
    { project_element::task_body, "TaskBody" },
    { project_element::using_task, "UsingTask" },
    { project_element::when, "When" }
  };
  constexpr const int g_element_count = (sizeof(g_elements) / sizeof(*g_elements));

  for (int i = 0; i < g_element_count; i++) {
    if (name.compare(g_elements[i].name) == 0)
      return g_elements[i].type;
  };
    
  return typename U::none;
}

*/