#include "StdAfx.h"
#include "ParseVcxproj.h"
#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_print.hpp"

CParseVcxproj::CParseVcxproj(void)
{
}

CParseVcxproj::~CParseVcxproj(void)
{
}

bool CParseVcxproj::Parse(char *pText)
{
	using namespace rapidxml;
	xml_document<> doc;
	doc.parse<0>(pText);
	xml_node<> *root = doc.first_node();
	if (!root)
	{
		return false;
	}

	xml_node<> *nodePropertyGroup = root->first_node("PropertyGroup");
	while (nodePropertyGroup)
	{
		xml_attribute<> *attrLabel = nodePropertyGroup->first_attribute("Label");
		if (attrLabel)
		{
			string strLabel = attrLabel->value();
			if (strLabel == "Globals")
			{
				xml_node<> *nodeProjectName = nodePropertyGroup->first_node("ProjectName");
				if (nodeProjectName)
				{
					strProjectName = nodeProjectName->value();
				}
				xml_node<> *nodeProjectGuid = nodePropertyGroup->first_node("ProjectGuid");
				if (nodeProjectGuid)
				{
					strProjectGuid = nodeProjectGuid->value();
				}
				xml_node<> *nodeRootNamespace = nodePropertyGroup->first_node("RootNamespace");
				if (nodeRootNamespace)
				{
					strRootNamespace = nodeRootNamespace->value();
				}
				xml_node<> *nodeKeyword = nodePropertyGroup->first_node("Keyword");
				if (nodeKeyword)
				{
					strKeyword = nodeKeyword->value();
				}
			}
			else if (strLabel == "Configuration")
			{
				xml_attribute<> *attrCondition = nodePropertyGroup->first_attribute("Condition");
				if (attrCondition)
				{
					string strPre = "'$(Configuration)|$(Platform)'=='";
					string strCondition = attrCondition->value();
					string::size_type nPos = strCondition.find(strPre);
					ASSERT(nPos != string::npos);

					STConfiguration stConfiguration;
					stConfiguration.strName = strCondition.substr(strPre.length());
					xml_node<> *nodeConfigurationType = nodePropertyGroup->first_node("ConfigurationType");
					if (nodeConfigurationType)
					{
						string strConfigurationTypeNum = "0";
						string strConfigurationType = nodeConfigurationType->value();
						if (strConfigurationType == "Application")
						{
							strConfigurationTypeNum = "1";
						}
						else if (strConfigurationType == "DynamicLibrary")
						{
							strConfigurationTypeNum = "2";
						}
						else if (strConfigurationType == "Generic")
						{
							strConfigurationTypeNum = "3";
						}
						else if (strConfigurationType == "StaticLibrary")
						{
							strConfigurationTypeNum = "4";
						}
						else if (strConfigurationType == "Unknown")
						{
							strConfigurationTypeNum = "5";
						}
						stConfiguration.strConfigurationType = strConfigurationTypeNum;
					}
					xml_node<> *nodeCharacterSet = nodePropertyGroup->first_node("CharacterSet");
					if (nodeCharacterSet)
					{
						string strCharacterSetNum = "0";
						string strCharacterSet = nodeCharacterSet->value();
						if (strCharacterSet == "Unicode")
						{
							strCharacterSetNum = "1";
						}
						else if (strCharacterSet == "MBCS")
						{
							strCharacterSetNum = "2";
						}
						else if (strCharacterSet == "NotSet")
						{
							strCharacterSetNum = "0";
						}						
						stConfiguration.strCharacterSet = strCharacterSetNum;
					}
					vecStConfiguration.push_back(stConfiguration);
				}
			}
		}
		else
		{
			xml_node<> *nodeOutDir = nodePropertyGroup->first_node("OutDir");
			while (nodeOutDir)
			{
				xml_attribute<> *attrCondition = nodeOutDir->first_attribute("Condition");
				if (attrCondition)
				{
					STConfiguration *pStConfiguration = GetSTConfigurationByCondition(attrCondition->value());
					if (pStConfiguration)
					{
						pStConfiguration->strOutputDirectory = nodeOutDir->value();
					}
				}
				nodeOutDir = nodeOutDir->next_sibling("OutDir");
			}
			xml_node<> *nodeIntDir = nodePropertyGroup->first_node("IntDir");
			while (nodeIntDir)
			{
				xml_attribute<> *attrCondition = nodeIntDir->first_attribute("Condition");
				if (attrCondition)
				{
					STConfiguration *pStConfiguration = GetSTConfigurationByCondition(attrCondition->value());
					if (pStConfiguration)
					{
						pStConfiguration->strIntermediateDirectory = nodeIntDir->value();
					}
				}
				nodeIntDir = nodeIntDir->next_sibling("IntDir");
			}
		}
		nodePropertyGroup = nodePropertyGroup->next_sibling("PropertyGroup");
	}
	xml_node<> *nodeItemDefinitionGroup = root->first_node("ItemDefinitionGroup");
	while (nodeItemDefinitionGroup)
	{
		xml_attribute<> *attrCondition = nodeItemDefinitionGroup->first_attribute("Condition");
		if (attrCondition)
		{
			STConfiguration *pStConfiguration = GetSTConfigurationByCondition(attrCondition->value());
			if (pStConfiguration)
			{
				xml_node<> *nodeClCompile = nodeItemDefinitionGroup->first_node("ClCompile");
				if (nodeClCompile)
				{
					STVCCLCompilerTool *pStVCCLCompilerTool = &pStConfiguration->stVCCLCompilerTool;
					xml_node<> *nodeOptimization = nodeClCompile->first_node("Optimization");
					if (nodeOptimization)
					{
						string strOptimizationNum = "0";
						string strOptimization = nodeOptimization->value();
						if (strOptimization == "Disabled")
						{
							strOptimizationNum = "0";
						}
						pStVCCLCompilerTool->strOptimization = strOptimizationNum;
					}
					xml_node<> *nodeAdditionalIncludeDirectories = nodeClCompile->first_node("AdditionalIncludeDirectories");
					if (nodeAdditionalIncludeDirectories)
					{
						pStVCCLCompilerTool->strAdditionalIncludeDirectories = nodeAdditionalIncludeDirectories->value();
					}
					xml_node<> *nodePreprocessorDefinitions = nodeClCompile->first_node("PreprocessorDefinitions");
					if (nodePreprocessorDefinitions)
					{
						pStVCCLCompilerTool->strPreprocessorDefinitions = nodePreprocessorDefinitions->value();
					}
					xml_node<> *nodeMinimalRebuild = nodeClCompile->first_node("MinimalRebuild");
					if (nodeMinimalRebuild)
					{
						pStVCCLCompilerTool->strMinimalRebuild = nodeMinimalRebuild->value();
					}
					xml_node<> *nodeBasicRuntimeChecks = nodeClCompile->first_node("BasicRuntimeChecks");
					if (nodeBasicRuntimeChecks)
					{
						string strBasicRuntimeChecksNum = "0";
						string strBasicRuntimeChecks = nodeBasicRuntimeChecks->value();
						if (strBasicRuntimeChecks == "EnableFastChecks")
						{
							strBasicRuntimeChecksNum = "3";
						}
						pStVCCLCompilerTool->strBasicRuntimeChecks = strBasicRuntimeChecksNum;
					}
					xml_node<> *nodeRuntimeLibrary = nodeClCompile->first_node("RuntimeLibrary");
					if (nodeRuntimeLibrary)
					{
						string strRuntimeLibraryNum = "0";
						string strRuntimeLibrary = nodeRuntimeLibrary->value();
						if (strRuntimeLibrary == "MultiThreadedDebugDLL")
						{
							strRuntimeLibraryNum = "3";
						}
						pStVCCLCompilerTool->strRuntimeLibrary = strRuntimeLibraryNum;
					}
					xml_node<> *nodeWarningLevel = nodeClCompile->first_node("WarningLevel");
					if (nodeWarningLevel)
					{
						string strWarningLevelNum = "0";
						string strWarningLevel = nodeWarningLevel->value();
						if (strWarningLevel == "Level3")
						{
							strWarningLevelNum = "3";
						}
						pStVCCLCompilerTool->strWarningLevel = strWarningLevelNum;
					}
					xml_node<> *nodeDebugInformationFormat = nodeClCompile->first_node("DebugInformationFormat");
					if (nodeDebugInformationFormat)
					{
						string strDebugInformationFormatNum = "0";
						string strDebugInformationFormat = nodeDebugInformationFormat->value();
						if (strDebugInformationFormat == "EditAndContinue")
						{
							strDebugInformationFormatNum = "4";
						}
						pStVCCLCompilerTool->strDebugInformationFormat = strDebugInformationFormatNum;
					}
				}
				xml_node<> *nodeLib = nodeItemDefinitionGroup->first_node("Lib");
				if (nodeLib)
				{
					STVCLibrarianTool *pStVCLibrarianTool = &pStConfiguration->stVCLibrarianTool;
					xml_node<> *nodeOutputFile = nodeLib->first_node("OutputFile");
					if (nodeOutputFile)
					{
						pStVCLibrarianTool->strOutputFile = nodeOutputFile->value();
					}
				}
			}
		}
		nodeItemDefinitionGroup = nodeItemDefinitionGroup->next_sibling("ItemDefinitionGroup");
	}
	return true;
}

bool CParseVcxproj::ParseFilters(char *pText)
{
	using namespace rapidxml;
	xml_document<> doc;
	doc.parse<0>(pText);
	xml_node<> *root = doc.first_node();
	if (!root)
	{
		return false;
	}

	xml_node<> *nodeItemGroup = root->first_node("ItemGroup");
	while (nodeItemGroup)
	{
		xml_node<> *nodeFilter = nodeItemGroup->first_node("Filter");
		while (nodeFilter)
		{
			xml_attribute<> *attrInclude = nodeFilter->first_attribute("Include");
			if (attrInclude)
			{
				STFilter stFilter;
				string strInclude = attrInclude->value();
				string::size_type nPos = strInclude.rfind('\\');
				if (nPos == string::npos)
				{
					stFilter.pParent = NULL;
					stFilter.strName = strInclude;
				}
				else
				{
					stFilter.pParent = GetSTFilterByFullName(strInclude.substr(0, nPos - 1));
					stFilter.strName = strInclude.substr(nPos + 1);
				}
				stFilter.strFullName = strInclude;
				vecStFilter.push_back(stFilter);
			}
			nodeFilter = nodeFilter->next_sibling("Filter");
		}
		xml_node<> *nodeClCompile = 
		nodeItemGroup = nodeItemGroup->next_sibling("ItemGroup");
	}
	return true;
}

STConfiguration * CParseVcxproj::GetSTConfigurationByCondition(string strCondition)
{
	string strPre = "'$(Configuration)|$(Platform)'=='";
	string::size_type nPos = strCondition.find(strPre);
	if (nPos == string::npos)
	{
		return NULL;
	}

	string strName = strCondition.substr(strPre.length());
	for (string::size_type i = 0; i < vecStConfiguration.size(); ++i)
	{
		if (vecStConfiguration[i].strName == strName)
		{
			return &vecStConfiguration[i];
		}
	}
	return NULL;
}

STFilter * CParseVcxproj::GetSTFilterByFullName(string strFullName)
{
	for (string::size_type i = 0; i < vecStFilter.size(); ++i)
	{
		if (vecStFilter[i].strFullName == strFullName)
		{
			return &vecStFilter[i];
		}
	}
	return NULL;
}
