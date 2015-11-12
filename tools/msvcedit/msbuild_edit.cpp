// msbuild.cpp : Defines the entry point for the console application.
//

#include "msbuild_edit.h"
#include "xml_serializer.h"
#include "project.h"

int main()
{
 // msbuild::project project;

  msbuild::xml_serializer<msbuild::project> xml("C:\\projects\\MediaExplorerPro\\tools\\msbuild\\msbuild.vcxproj");

  auto project =  xml.deserialize();

    return 0;
}

