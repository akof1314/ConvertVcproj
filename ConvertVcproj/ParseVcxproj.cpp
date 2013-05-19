#include "StdAfx.h"
#include "ParseVcxproj.h"
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
					string strName = strCondition.substr(strPre.length());
					strName.erase(strName.length() - 1);
					stConfiguration.strName = strName;
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
					xml_node<> *nodeWholeProgramOptimization = nodePropertyGroup->first_node("WholeProgramOptimization");
					if (nodeWholeProgramOptimization)
					{
						string strWholeProgramOptimizationNum = "0";
						string strWholeProgramOptimization = nodeWholeProgramOptimization->value();
						if (strWholeProgramOptimization == "true")
						{
							strWholeProgramOptimizationNum = "1";
						}											
						stConfiguration.strWholeProgramOptimization = strWholeProgramOptimizationNum;
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
						pStConfiguration->strOutputDirectory = MacroConvert(nodeOutDir->value());
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
						pStConfiguration->strIntermediateDirectory = MacroConvert(nodeIntDir->value());
					}
				}
				nodeIntDir = nodeIntDir->next_sibling("IntDir");
			}
			xml_node<> *nodeLinkIncremental = nodePropertyGroup->first_node("LinkIncremental");
			while (nodeLinkIncremental)
			{
				xml_attribute<> *attrCondition = nodeLinkIncremental->first_attribute("Condition");
				if (attrCondition)
				{
					STConfiguration *pStConfiguration = GetSTConfigurationByCondition(attrCondition->value());
					if (pStConfiguration)
					{
						string strLinkIncrementalNum = "1";
						string strLinkIncremental = nodeLinkIncremental->value();
						if (strLinkIncremental == "true")
						{
							strLinkIncrementalNum = "2";
						}
						pStConfiguration->stVCLinkerTool.strLinkIncremental = strLinkIncrementalNum;
					}
				}
				nodeLinkIncremental = nodeLinkIncremental->next_sibling("LinkIncremental");
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
				xml_node<> *nodePreBuildEvent = nodeItemDefinitionGroup->first_node("PreBuildEvent");
				if (nodePreBuildEvent)
				{
					STVCPreBuildEventTool *pStVCPreBuildEventTool = &pStConfiguration->stVCPreBuildEventTool;
					xml_node<> *nodeCommand = nodePreBuildEvent->first_node("Command");
					if (nodeCommand)
					{
						pStVCPreBuildEventTool->strCommandLine = nodeCommand->value();
					}
					xml_node<> *nodeMessage = nodePreBuildEvent->first_node("Message");
					if (nodeMessage)
					{
						pStVCPreBuildEventTool->strDescription = nodeMessage->value();
					}
				}
				xml_node<> *nodeMidl = nodeItemDefinitionGroup->first_node("Midl");
				if (nodeMidl)
				{
					STVCMIDLTool *pStVCMIDLTool = &pStConfiguration->stVCMIDLTool;
					xml_node<> *nodePreprocessorDefinitions = nodeMidl->first_node("PreprocessorDefinitions");
					if (nodePreprocessorDefinitions)
					{
						pStVCMIDLTool->strPreprocessorDefinitions = MacroExclude(nodePreprocessorDefinitions->value());
					}
					xml_node<> *nodeMkTypLibCompatible = nodeMidl->first_node("MkTypLibCompatible");
					if (nodeMkTypLibCompatible)
					{
						pStVCMIDLTool->strMkTypLibCompatible = nodeMkTypLibCompatible->value();
					}
					xml_node<> *nodeTargetEnvironment = nodeMidl->first_node("TargetEnvironment");
					if (nodeTargetEnvironment)
					{
						string strTargetEnvironmentNum = "0";
						string strTargetEnvironment = nodeTargetEnvironment->value();
						if (strTargetEnvironment == "Win32")
						{
							strTargetEnvironmentNum = "1";
						}
						pStVCMIDLTool->strTargetEnvironment = strTargetEnvironmentNum;
					}
					xml_node<> *nodeGenerateStublessProxies = nodeMidl->first_node("GenerateStublessProxies");
					if (nodeGenerateStublessProxies)
					{
						pStVCMIDLTool->strGenerateStublessProxies = nodeGenerateStublessProxies->value();
					}
					xml_node<> *nodeTypeLibraryName = nodeMidl->first_node("TypeLibraryName");
					if (nodeTypeLibraryName)
					{
						pStVCMIDLTool->strTypeLibraryName = nodeTypeLibraryName->value();
					}
					xml_node<> *nodeHeaderFileName = nodeMidl->first_node("HeaderFileName");
					if (nodeHeaderFileName)
					{
						pStVCMIDLTool->strHeaderFileName = nodeHeaderFileName->value();
					}
					xml_node<> *nodeDllDataFileName = nodeMidl->first_node("DllDataFileName");
					if (nodeDllDataFileName)
					{
						pStVCMIDLTool->strDLLDataFileName = nodeDllDataFileName->value();
					}
					xml_node<> *nodeInterfaceIdentifierFileName = nodeMidl->first_node("InterfaceIdentifierFileName");
					if (nodeInterfaceIdentifierFileName)
					{
						pStVCMIDLTool->strInterfaceIdentifierFileName = nodeInterfaceIdentifierFileName->value();
					}
					xml_node<> *nodeProxyFileName = nodeMidl->first_node("ProxyFileName");
					if (nodeProxyFileName)
					{
						pStVCMIDLTool->strProxyFileName = nodeProxyFileName->value();
					}
				}
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
						else if (strOptimization == "MaxSpeed")
						{
							strOptimizationNum = "2";
						}
						pStVCCLCompilerTool->strOptimization = strOptimizationNum;
					}
					xml_node<> *nodeIntrinsicFunctions = nodeClCompile->first_node("IntrinsicFunctions");
					if (nodeIntrinsicFunctions)
					{
						pStVCCLCompilerTool->strEnableIntrinsicFunctions = nodeIntrinsicFunctions->value();
					}
					xml_node<> *nodeAdditionalIncludeDirectories = nodeClCompile->first_node("AdditionalIncludeDirectories");
					if (nodeAdditionalIncludeDirectories)
					{
						pStVCCLCompilerTool->strAdditionalIncludeDirectories = MacroExclude(nodeAdditionalIncludeDirectories->value());
					}
					xml_node<> *nodePreprocessorDefinitions = nodeClCompile->first_node("PreprocessorDefinitions");
					if (nodePreprocessorDefinitions)
					{
						pStVCCLCompilerTool->strPreprocessorDefinitions = MacroExclude(nodePreprocessorDefinitions->value());
					}
					xml_node<> *nodeMinimalRebuild = nodeClCompile->first_node("MinimalRebuild");
					if (nodeMinimalRebuild)
					{
						pStVCCLCompilerTool->strMinimalRebuild = nodeMinimalRebuild->value();
					}
					xml_node<> *nodeExceptionHandling = nodeClCompile->first_node("ExceptionHandling");
					if (nodeExceptionHandling)
					{
						pStVCCLCompilerTool->strExceptionHandling = nodeExceptionHandling->value();
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
						string strRuntimeLibraryNum;
						string strRuntimeLibrary = nodeRuntimeLibrary->value();
						if (strRuntimeLibrary == "MultiThreadedDebugDLL")
						{
							strRuntimeLibraryNum = "3";
						}
						else if (strRuntimeLibrary == "MultiThreadedDLL")
						{
							strRuntimeLibraryNum = "2";
						}
						else if (strRuntimeLibrary == "SingleThreaded")
						{
							strRuntimeLibraryNum = "4";
						}
						else if (strRuntimeLibrary == "SingleThreadedDebug")
						{
							strRuntimeLibraryNum = "5";
						}
						else if (strRuntimeLibrary == "MultiThreadedDebug")
						{
							strRuntimeLibraryNum = "1";
						}
						else if (strRuntimeLibrary == "MultiThreaded")
						{
							strRuntimeLibraryNum = "0";
						}
						pStVCCLCompilerTool->strRuntimeLibrary = strRuntimeLibraryNum;
					}
					xml_node<> *nodeFunctionLevelLinking = nodeClCompile->first_node("FunctionLevelLinking");
					if (nodeFunctionLevelLinking)
					{
						pStVCCLCompilerTool->strEnableFunctionLevelLinking = nodeFunctionLevelLinking->value();
					}
					xml_node<> *nodeTreatWChar_tAsBuiltInType = nodeClCompile->first_node("TreatWChar_tAsBuiltInType");
					if (nodeTreatWChar_tAsBuiltInType)
					{
						pStVCCLCompilerTool->strTreatWChar_tAsBuiltInType = nodeTreatWChar_tAsBuiltInType->value();
					}
					xml_node<> *nodePrecompiledHeader = nodeClCompile->first_node("PrecompiledHeader");
					if (nodePrecompiledHeader)
					{
						pStVCCLCompilerTool->strUsePrecompiledHeader = nodePrecompiledHeader->value();
					}
					xml_node<> *nodePrecompiledHeaderFile = nodeClCompile->first_node("PrecompiledHeaderFile");
					if (nodePrecompiledHeaderFile)
					{
						pStVCCLCompilerTool->strPrecompiledHeaderThrough = nodePrecompiledHeaderFile->value();
					}
					xml_node<> *nodePrecompiledHeaderOutputFile = nodeClCompile->first_node("PrecompiledHeaderOutputFile");
					if (nodePrecompiledHeaderOutputFile)
					{
						pStVCCLCompilerTool->strPrecompiledHeaderFile = nodePrecompiledHeaderOutputFile->value();
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
						else if (strWarningLevel == "Level4")
						{
							strWarningLevelNum = "4";
						}
						else if (strWarningLevel == "Level1")
						{
							strWarningLevelNum = "1";
						}
						else if (strWarningLevel == "Level2")
						{
							strWarningLevelNum = "2";
						}
						pStVCCLCompilerTool->strWarningLevel = strWarningLevelNum;
					}
					pStVCCLCompilerTool->strDetect64BitPortabilityProblems = "false";
					xml_node<> *nodeDebugInformationFormat = nodeClCompile->first_node("DebugInformationFormat");
					if (nodeDebugInformationFormat)
					{
						string strDebugInformationFormatNum = "0";
						string strDebugInformationFormat = nodeDebugInformationFormat->value();
						if (strDebugInformationFormat == "EditAndContinue")
						{
							strDebugInformationFormatNum = "4";
						}
						else if (strDebugInformationFormat == "ProgramDatabase")
						{
							strDebugInformationFormatNum = "3";
						}
						else if (strDebugInformationFormat == "LineInfoOnly")
						{
							strDebugInformationFormatNum = "2";
						}
						else if (strDebugInformationFormat == "OldStyleInfo")
						{
							strDebugInformationFormatNum = "1";
						}
						pStVCCLCompilerTool->strDebugInformationFormat = strDebugInformationFormatNum;
					}
					xml_node<> *nodeCompileAs = nodeClCompile->first_node("CompileAs");
					if (nodeCompileAs)
					{
						string strCompileAsNum = "0";
						string strCompileAs = nodeCompileAs->value();
						if (strCompileAs == "CompileAsC")
						{
							strCompileAsNum = "1";
						}
						else if (strCompileAs == "CompileAsCpp")
						{
							strCompileAsNum = "2";
						}
						pStVCCLCompilerTool->strCompileAs = strCompileAsNum;
					}
					xml_node<> *nodeDisableSpecificWarnings = nodeClCompile->first_node("DisableSpecificWarnings");
					if (nodeDisableSpecificWarnings)
					{
						pStVCCLCompilerTool->strDisableSpecificWarnings = MacroExclude(nodeDisableSpecificWarnings->value());
					}
				}
				xml_node<> *nodeResourceCompile = nodeItemDefinitionGroup->first_node("ResourceCompile");
				if (nodeResourceCompile)
				{
					STVCResourceCompilerTool *pStVCResourceCompilerTool = &pStConfiguration->stVCResourceCompilerTool;
					xml_node<> *nodePreprocessorDefinitions = nodeResourceCompile->first_node("PreprocessorDefinitions");
					if (nodePreprocessorDefinitions)
					{
						pStVCResourceCompilerTool->strPreprocessorDefinitions = MacroExclude(nodePreprocessorDefinitions->value());
					}
					xml_node<> *nodeCulture = nodeResourceCompile->first_node("Culture");
					if (nodeCulture)
					{
						string strCulture = nodeCulture->value();
						if (!strCulture.empty())
						{
							long lCulture = strtol(strCulture.c_str(), NULL, 16);
							char temp[50];
							strCulture = _ltoa_s(lCulture, temp, 10);
						}
						pStVCResourceCompilerTool->strCulture = strCulture;
					}
					xml_node<> *nodeAdditionalIncludeDirectories = nodeResourceCompile->first_node("AdditionalIncludeDirectories");
					if (nodeAdditionalIncludeDirectories)
					{
						pStVCResourceCompilerTool->strAdditionalIncludeDirectories = MacroExclude(nodeAdditionalIncludeDirectories->value());
					}
				}
				xml_node<> *nodePreLinkEvent = nodeItemDefinitionGroup->first_node("PreLinkEvent");
				if (nodePreLinkEvent)
				{
					STVCPreLinkEventTool *pStVCPreLinkEventTool = &pStConfiguration->stVCPreLinkEventTool;
					xml_node<> *nodeCommand = nodePreLinkEvent->first_node("Command");
					if (nodeCommand)
					{
						pStVCPreLinkEventTool->strCommandLine = nodeCommand->value();
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
				xml_node<> *nodeLink = nodeItemDefinitionGroup->first_node("Link");
				if (nodeLink)
				{
					STVCLinkerTool *pStVCLinkerTool = &pStConfiguration->stVCLinkerTool;
					xml_node<> *nodeAdditionalDependencies = nodeLink->first_node("AdditionalDependencies");
					if (nodeAdditionalDependencies)
					{
						pStVCLinkerTool->strAdditionalDependencies = SemicolonToSpace(MacroExclude(nodeAdditionalDependencies->value()));
					}
					xml_node<> *nodeOutputFile = nodeLink->first_node("OutputFile");
					if (nodeOutputFile)
					{
						pStVCLinkerTool->strOutputFile = nodeOutputFile->value();
					}
					xml_node<> *nodeAdditionalLibraryDirectories = nodeLink->first_node("AdditionalLibraryDirectories");
					if (nodeAdditionalLibraryDirectories)
					{
						pStVCLinkerTool->strAdditionalLibraryDirectories = MacroExclude(nodeAdditionalLibraryDirectories->value());
					}
					xml_node<> *nodeIgnoreAllDefaultLibraries = nodeLink->first_node("IgnoreAllDefaultLibraries");
					if (nodeIgnoreAllDefaultLibraries)
					{
						pStVCLinkerTool->strIgnoreAllDefaultLibraries = nodeIgnoreAllDefaultLibraries->value();
					}
					xml_node<> *nodeIgnoreSpecificDefaultLibraries = nodeLink->first_node("IgnoreSpecificDefaultLibraries");
					if (nodeIgnoreSpecificDefaultLibraries)
					{
						pStVCLinkerTool->strIgnoreDefaultLibraryNames = MacroExclude(nodeIgnoreSpecificDefaultLibraries->value());
					}
					xml_node<> *nodeGenerateDebugInformation = nodeLink->first_node("GenerateDebugInformation");
					if (nodeGenerateDebugInformation)
					{
						pStVCLinkerTool->strGenerateDebugInformation = nodeGenerateDebugInformation->value();
					}
					xml_node<> *nodeSubSystem = nodeLink->first_node("SubSystem");
					if (nodeSubSystem)
					{
						string strSubSystemNum = "0";
						string strSubSystem = nodeSubSystem->value();
						if (strSubSystem == "Windows")
						{
							strSubSystemNum = "2";
						}
						else if (strSubSystem == "Console")
						{
							strSubSystemNum = "1";
						}
						pStVCLinkerTool->strSubSystem = strSubSystemNum;
					}
					xml_node<> *nodeImportLibrary = nodeLink->first_node("ImportLibrary");
					if (nodeImportLibrary)
					{
						pStVCLinkerTool->strImportLibrary = nodeImportLibrary->value();
					}
					xml_node<> *nodeOptimizeReferences = nodeLink->first_node("OptimizeReferences");
					if (nodeOptimizeReferences)
					{
						string strOptimizeReferencesNum = "0";
						string strOptimizeReferences = nodeOptimizeReferences->value();
						if (strOptimizeReferences == "false")
						{
							strOptimizeReferencesNum = "1";
						}
						else if (strOptimizeReferences == "true")
						{
							strOptimizeReferencesNum = "2";
						}
						pStVCLinkerTool->strOptimizeReferences = strOptimizeReferencesNum;
					}
					xml_node<> *nodeEnableCOMDATFolding = nodeLink->first_node("EnableCOMDATFolding");
					if (nodeEnableCOMDATFolding)
					{
						string strEnableCOMDATFoldingNum = "0";
						string strEnableCOMDATFolding = nodeEnableCOMDATFolding->value();
						if (strEnableCOMDATFolding == "false")
						{
							strEnableCOMDATFoldingNum = "1";
						}
						else if (strEnableCOMDATFolding == "true")
						{
							strEnableCOMDATFoldingNum = "2";
						}
						pStVCLinkerTool->strEnableCOMDATFolding = strEnableCOMDATFoldingNum;
					}
					xml_node<> *nodeTargetMachine = nodeLink->first_node("TargetMachine");
					if (nodeTargetMachine)
					{
						string strTargetMachineNum = "0";
						string strTargetMachine = nodeTargetMachine->value();
						if (strTargetMachine == "MachineX86")
						{
							strTargetMachineNum = "1";
						}
						pStVCLinkerTool->strTargetMachine = strTargetMachineNum;
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
					stFilter.nParent = -1;
					stFilter.strName = strInclude;
				}
				else
				{
					stFilter.nParent = GetSTFilterIndexByFullName(strInclude.substr(0, nPos));
					stFilter.strName = strInclude.substr(nPos + 1);
				}
				stFilter.strFullName = strInclude;
				xml_node<> *nodeUniqueIdentifier = nodeFilter->first_node("UniqueIdentifier");
				if (nodeUniqueIdentifier)
				{
					stFilter.strUniqueIdentifier = nodeUniqueIdentifier->value();
				}
				xml_node<> *nodeExtensions = nodeFilter->first_node("Extensions");
				if (nodeExtensions)
				{
					stFilter.strExtensions = nodeExtensions->value();
				}
				vecStFilter.push_back(stFilter);
			}
			nodeFilter = nodeFilter->next_sibling("Filter");
		}
		xml_node<> *nodeClCompile = nodeItemGroup->first_node("ClCompile");
		while (nodeClCompile)
		{
			xml_attribute<> *attrInclude = nodeClCompile->first_attribute("Include");
			if (attrInclude)
			{
				xml_node<> *nodeFilter = nodeClCompile->first_node("Filter");
				if (nodeFilter)
				{
					STFilter *pStFilter = GetSTFilterByFullName(nodeFilter->value());
					if (pStFilter)
					{
						pStFilter->vecFile.push_back(attrInclude->value());
					}
				}
				else
				{
					STFilter stFilter;
					stFilter.nParent = -1;
					stFilter.strName = "";
					stFilter.strFullName = "";
					stFilter.vecFile.push_back(attrInclude->value());
					vecStFilter.push_back(stFilter);
				}
			}
			nodeClCompile = nodeClCompile->next_sibling("ClCompile");
		}
		xml_node<> *nodeClInclude = nodeItemGroup->first_node("ClInclude");
		while (nodeClInclude)
		{
			xml_attribute<> *attrInclude = nodeClInclude->first_attribute("Include");
			if (attrInclude)
			{
				xml_node<> *nodeFilter = nodeClInclude->first_node("Filter");
				if (nodeFilter)
				{
					STFilter *pStFilter = GetSTFilterByFullName(nodeFilter->value());
					if (pStFilter)
					{
						pStFilter->vecFile.push_back(attrInclude->value());
					}
				}
				else
				{
					STFilter stFilter;
					stFilter.nParent = -1;
					stFilter.strName = "";
					stFilter.strFullName = "";
					stFilter.vecFile.push_back(attrInclude->value());
					vecStFilter.push_back(stFilter);
				}
			}
			nodeClInclude = nodeClInclude->next_sibling("ClInclude");
		}
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
	strName.erase(strName.length() - 1);
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

int CParseVcxproj::GetSTFilterIndexByFullName(string strFullName)
{
	for (string::size_type i = 0; i < vecStFilter.size(); ++i)
	{
		if (vecStFilter[i].strFullName == strFullName)
		{
			return i;
		}
	}
	return -1;
}

bool CParseVcxproj::Save(string &strOut)
{
	using namespace rapidxml;
	xml_document<> doc;
	xml_node<> *pi = doc.allocate_node(node_pi, "xml version='1.0' encoding='UTF-8'");
	doc.append_node(pi);
	xml_node<> *root = doc.allocate_node(node_element, "VisualStudioProject");
	root->append_attribute(doc.allocate_attribute("ProjectType", "Visual C++"));
	root->append_attribute(doc.allocate_attribute("Version", "9.00"));
	root->append_attribute(doc.allocate_attribute("Name", strProjectName.c_str()));
	root->append_attribute(doc.allocate_attribute("ProjectGUID", strProjectGuid.c_str()));
	root->append_attribute(doc.allocate_attribute("RootNamespace", strRootNamespace.c_str()));
	root->append_attribute(doc.allocate_attribute("Keyword", strKeyword.c_str()));
	root->append_attribute(doc.allocate_attribute("TargetFrameworkVersion", "0"));
	doc.append_node(root);

	xml_node<> *nodePlatforms = doc.allocate_node(node_element, "Platforms");
	xml_node<> *nodePlatform = doc.allocate_node(node_element, "Platform");
	nodePlatform->append_attribute(doc.allocate_attribute("Name", "Win32"));
	nodePlatforms->append_node(nodePlatform);
	root->append_node(nodePlatforms);

	xml_node<> *nodeToolFiles = doc.allocate_node(node_element, "ToolFiles");
	root->append_node(nodeToolFiles);

	xml_node<> *nodeConfigurations = doc.allocate_node(node_element, "Configurations");
	for (string::size_type i = 0; i < vecStConfiguration.size(); ++i)
	{
		xml_node<> *nodeConfiguration = doc.allocate_node(node_element, "Configuration");
		nodeConfiguration->append_attribute(doc.allocate_attribute("Name", vecStConfiguration[i].strName.c_str()));
		nodeConfiguration->append_attribute(doc.allocate_attribute("OutputDirectory", vecStConfiguration[i].strOutputDirectory.c_str()));
		nodeConfiguration->append_attribute(doc.allocate_attribute("IntermediateDirectory", vecStConfiguration[i].strIntermediateDirectory.c_str()));
		nodeConfiguration->append_attribute(doc.allocate_attribute("ConfigurationType", vecStConfiguration[i].strConfigurationType.c_str()));
		nodeConfiguration->append_attribute(doc.allocate_attribute("CharacterSet", vecStConfiguration[i].strCharacterSet.c_str()));
		nodeConfiguration->append_attribute(doc.allocate_attribute("WholeProgramOptimization", vecStConfiguration[i].strWholeProgramOptimization.c_str()));

		xml_node<> *nodeVCPreBuildEventTool = doc.allocate_node(node_element, "Tool");
		nodeVCPreBuildEventTool->append_attribute(doc.allocate_attribute("Name", "VCPreBuildEventTool"));
		nodeVCPreBuildEventTool->append_attribute(doc.allocate_attribute("Description", vecStConfiguration[i].stVCPreBuildEventTool.strDescription.c_str()));
		nodeVCPreBuildEventTool->append_attribute(doc.allocate_attribute("CommandLine", vecStConfiguration[i].stVCPreBuildEventTool.strCommandLine.c_str()));
		nodeConfiguration->append_node(nodeVCPreBuildEventTool);

		xml_node<> *nodeVCCustomBuildTool = doc.allocate_node(node_element, "Tool");
		nodeVCCustomBuildTool->append_attribute(doc.allocate_attribute("Name", "VCCustomBuildTool"));
		nodeConfiguration->append_node(nodeVCCustomBuildTool);

		xml_node<> *nodeVCXMLDataGeneratorTool = doc.allocate_node(node_element, "Tool");
		nodeVCXMLDataGeneratorTool->append_attribute(doc.allocate_attribute("Name", "VCXMLDataGeneratorTool"));
		nodeConfiguration->append_node(nodeVCXMLDataGeneratorTool);

		xml_node<> *nodeVCWebServiceProxyGeneratorTool = doc.allocate_node(node_element, "Tool");
		nodeVCWebServiceProxyGeneratorTool->append_attribute(doc.allocate_attribute("Name", "VCWebServiceProxyGeneratorTool"));
		nodeConfiguration->append_node(nodeVCWebServiceProxyGeneratorTool);

		xml_node<> *nodeVCMIDLTool = doc.allocate_node(node_element, "Tool");
		nodeVCMIDLTool->append_attribute(doc.allocate_attribute("Name", "VCMIDLTool"));
		nodeVCMIDLTool->append_attribute(doc.allocate_attribute("PreprocessorDefinitions", vecStConfiguration[i].stVCMIDLTool.strPreprocessorDefinitions.c_str()));
		nodeVCMIDLTool->append_attribute(doc.allocate_attribute("MkTypLibCompatible", vecStConfiguration[i].stVCMIDLTool.strMkTypLibCompatible.c_str()));
		nodeVCMIDLTool->append_attribute(doc.allocate_attribute("TargetEnvironment", vecStConfiguration[i].stVCMIDLTool.strTargetEnvironment.c_str()));
		nodeVCMIDLTool->append_attribute(doc.allocate_attribute("GenerateStublessProxies", vecStConfiguration[i].stVCMIDLTool.strGenerateStublessProxies.c_str()));
		nodeVCMIDLTool->append_attribute(doc.allocate_attribute("TypeLibraryName", vecStConfiguration[i].stVCMIDLTool.strTypeLibraryName.c_str()));
		nodeVCMIDLTool->append_attribute(doc.allocate_attribute("HeaderFileName", vecStConfiguration[i].stVCMIDLTool.strHeaderFileName.c_str()));
		nodeVCMIDLTool->append_attribute(doc.allocate_attribute("DLLDataFileName", vecStConfiguration[i].stVCMIDLTool.strDLLDataFileName.c_str()));
		nodeVCMIDLTool->append_attribute(doc.allocate_attribute("InterfaceIdentifierFileName", vecStConfiguration[i].stVCMIDLTool.strInterfaceIdentifierFileName.c_str()));
		nodeVCMIDLTool->append_attribute(doc.allocate_attribute("ProxyFileName", vecStConfiguration[i].stVCMIDLTool.strProxyFileName.c_str()));
		nodeConfiguration->append_node(nodeVCMIDLTool);

		xml_node<> *nodeVCCLCompilerTool = doc.allocate_node(node_element, "Tool");
		nodeVCCLCompilerTool->append_attribute(doc.allocate_attribute("Name", "VCCLCompilerTool"));
		nodeVCCLCompilerTool->append_attribute(doc.allocate_attribute("Optimization", vecStConfiguration[i].stVCCLCompilerTool.strOptimization.c_str()));
		nodeVCCLCompilerTool->append_attribute(doc.allocate_attribute("EnableIntrinsicFunctions", vecStConfiguration[i].stVCCLCompilerTool.strEnableIntrinsicFunctions.c_str()));
		nodeVCCLCompilerTool->append_attribute(doc.allocate_attribute("AdditionalIncludeDirectories", vecStConfiguration[i].stVCCLCompilerTool.strAdditionalIncludeDirectories.c_str()));
		nodeVCCLCompilerTool->append_attribute(doc.allocate_attribute("PreprocessorDefinitions", vecStConfiguration[i].stVCCLCompilerTool.strPreprocessorDefinitions.c_str()));
		nodeVCCLCompilerTool->append_attribute(doc.allocate_attribute("MinimalRebuild", vecStConfiguration[i].stVCCLCompilerTool.strMinimalRebuild.c_str()));
		nodeVCCLCompilerTool->append_attribute(doc.allocate_attribute("ExceptionHandling", vecStConfiguration[i].stVCCLCompilerTool.strExceptionHandling.c_str()));
		nodeVCCLCompilerTool->append_attribute(doc.allocate_attribute("BasicRuntimeChecks", vecStConfiguration[i].stVCCLCompilerTool.strBasicRuntimeChecks.c_str()));
		nodeVCCLCompilerTool->append_attribute(doc.allocate_attribute("RuntimeLibrary", vecStConfiguration[i].stVCCLCompilerTool.strRuntimeLibrary.c_str()));
		nodeVCCLCompilerTool->append_attribute(doc.allocate_attribute("EnableFunctionLevelLinking", vecStConfiguration[i].stVCCLCompilerTool.strEnableFunctionLevelLinking.c_str()));
		nodeVCCLCompilerTool->append_attribute(doc.allocate_attribute("TreatWChar_tAsBuiltInType", vecStConfiguration[i].stVCCLCompilerTool.strTreatWChar_tAsBuiltInType.c_str()));
		nodeVCCLCompilerTool->append_attribute(doc.allocate_attribute("UsePrecompiledHeader", vecStConfiguration[i].stVCCLCompilerTool.strUsePrecompiledHeader.c_str()));
		nodeVCCLCompilerTool->append_attribute(doc.allocate_attribute("PrecompiledHeaderThrough", vecStConfiguration[i].stVCCLCompilerTool.strPrecompiledHeaderThrough.c_str()));
		nodeVCCLCompilerTool->append_attribute(doc.allocate_attribute("PrecompiledHeaderFile", vecStConfiguration[i].stVCCLCompilerTool.strPrecompiledHeaderFile.c_str()));
		nodeVCCLCompilerTool->append_attribute(doc.allocate_attribute("WarningLevel", vecStConfiguration[i].stVCCLCompilerTool.strWarningLevel.c_str()));
		nodeVCCLCompilerTool->append_attribute(doc.allocate_attribute("Detect64BitPortabilityProblems", vecStConfiguration[i].stVCCLCompilerTool.strDetect64BitPortabilityProblems.c_str()));
		nodeVCCLCompilerTool->append_attribute(doc.allocate_attribute("DebugInformationFormat", vecStConfiguration[i].stVCCLCompilerTool.strDebugInformationFormat.c_str()));
		nodeVCCLCompilerTool->append_attribute(doc.allocate_attribute("CompileAs", vecStConfiguration[i].stVCCLCompilerTool.strCompileAs.c_str()));
		nodeVCCLCompilerTool->append_attribute(doc.allocate_attribute("DisableSpecificWarnings", vecStConfiguration[i].stVCCLCompilerTool.strDisableSpecificWarnings.c_str()));
		nodeConfiguration->append_node(nodeVCCLCompilerTool);

		xml_node<> *nodeVCManagedResourceCompilerTool = doc.allocate_node(node_element, "Tool");
		nodeVCManagedResourceCompilerTool->append_attribute(doc.allocate_attribute("Name", "VCManagedResourceCompilerTool"));
		nodeConfiguration->append_node(nodeVCManagedResourceCompilerTool);

		xml_node<> *nodeVCResourceCompilerTool = doc.allocate_node(node_element, "Tool");
		nodeVCResourceCompilerTool->append_attribute(doc.allocate_attribute("Name", "VCResourceCompilerTool"));
		nodeVCResourceCompilerTool->append_attribute(doc.allocate_attribute("PreprocessorDefinitions", vecStConfiguration[i].stVCResourceCompilerTool.strPreprocessorDefinitions.c_str()));
		nodeVCResourceCompilerTool->append_attribute(doc.allocate_attribute("Culture", vecStConfiguration[i].stVCResourceCompilerTool.strCulture.c_str()));
		nodeVCResourceCompilerTool->append_attribute(doc.allocate_attribute("AdditionalIncludeDirectories", vecStConfiguration[i].stVCResourceCompilerTool.strAdditionalIncludeDirectories.c_str()));
		nodeConfiguration->append_node(nodeVCResourceCompilerTool);

		xml_node<> *nodeVCPreLinkEventTool = doc.allocate_node(node_element, "Tool");
		nodeVCPreLinkEventTool->append_attribute(doc.allocate_attribute("Name", "VCPreLinkEventTool"));
		nodeVCPreLinkEventTool->append_attribute(doc.allocate_attribute("CommandLine", vecStConfiguration[i].stVCPreLinkEventTool.strCommandLine.c_str()));
		nodeConfiguration->append_node(nodeVCPreLinkEventTool);

		xml_node<> *nodeVCLinkerTool = doc.allocate_node(node_element, "Tool");
		nodeVCLinkerTool->append_attribute(doc.allocate_attribute("Name", "VCLinkerTool"));
		nodeVCLinkerTool->append_attribute(doc.allocate_attribute("AdditionalDependencies", vecStConfiguration[i].stVCLinkerTool.strAdditionalDependencies.c_str()));
		nodeVCLinkerTool->append_attribute(doc.allocate_attribute("OutputFile", vecStConfiguration[i].stVCLinkerTool.strOutputFile.c_str()));
		nodeVCLinkerTool->append_attribute(doc.allocate_attribute("LinkIncremental", vecStConfiguration[i].stVCLinkerTool.strLinkIncremental.c_str()));
		nodeVCLinkerTool->append_attribute(doc.allocate_attribute("AdditionalLibraryDirectories", vecStConfiguration[i].stVCLinkerTool.strAdditionalLibraryDirectories.c_str()));
		nodeVCLinkerTool->append_attribute(doc.allocate_attribute("IgnoreAllDefaultLibraries", vecStConfiguration[i].stVCLinkerTool.strIgnoreAllDefaultLibraries.c_str()));
		nodeVCLinkerTool->append_attribute(doc.allocate_attribute("IgnoreDefaultLibraryNames", vecStConfiguration[i].stVCLinkerTool.strIgnoreDefaultLibraryNames.c_str()));
		nodeVCLinkerTool->append_attribute(doc.allocate_attribute("GenerateDebugInformation", vecStConfiguration[i].stVCLinkerTool.strGenerateDebugInformation.c_str()));
		nodeVCLinkerTool->append_attribute(doc.allocate_attribute("SubSystem", vecStConfiguration[i].stVCLinkerTool.strSubSystem.c_str()));
		nodeVCLinkerTool->append_attribute(doc.allocate_attribute("ImportLibrary", vecStConfiguration[i].stVCLinkerTool.strImportLibrary.c_str()));
		nodeVCLinkerTool->append_attribute(doc.allocate_attribute("OptimizeReferences", vecStConfiguration[i].stVCLinkerTool.strOptimizeReferences.c_str()));
		nodeVCLinkerTool->append_attribute(doc.allocate_attribute("EnableCOMDATFolding", vecStConfiguration[i].stVCLinkerTool.strEnableCOMDATFolding.c_str()));
		nodeVCLinkerTool->append_attribute(doc.allocate_attribute("TargetMachine", vecStConfiguration[i].stVCLinkerTool.strTargetMachine.c_str()));
		nodeConfiguration->append_node(nodeVCLinkerTool);

		xml_node<> *nodeVCLibrarianTool = doc.allocate_node(node_element, "Tool");
		nodeVCLibrarianTool->append_attribute(doc.allocate_attribute("Name", "VCLibrarianTool"));
		if (!vecStConfiguration[i].stVCLibrarianTool.strOutputFile.empty())
		{
			nodeVCLibrarianTool->append_attribute(doc.allocate_attribute("OutputFile", vecStConfiguration[i].stVCLibrarianTool.strOutputFile.c_str()));
		}
		nodeConfiguration->append_node(nodeVCLibrarianTool);

		xml_node<> *nodeVCALinkTool = doc.allocate_node(node_element, "Tool");
		nodeVCALinkTool->append_attribute(doc.allocate_attribute("Name", "VCALinkTool"));
		nodeConfiguration->append_node(nodeVCALinkTool);

		xml_node<> *nodeVCManifestTool = doc.allocate_node(node_element, "Tool");
		nodeVCManifestTool->append_attribute(doc.allocate_attribute("Name", "VCManifestTool"));
		nodeConfiguration->append_node(nodeVCManifestTool);

		xml_node<> *nodeVCXDCMakeTool = doc.allocate_node(node_element, "Tool");
		nodeVCXDCMakeTool->append_attribute(doc.allocate_attribute("Name", "VCXDCMakeTool"));
		nodeConfiguration->append_node(nodeVCXDCMakeTool);

		xml_node<> *nodeVCBscMakeTool = doc.allocate_node(node_element, "Tool");
		nodeVCBscMakeTool->append_attribute(doc.allocate_attribute("Name", "VCBscMakeTool"));
		nodeConfiguration->append_node(nodeVCBscMakeTool);

		xml_node<> *nodeVCFxCopTool = doc.allocate_node(node_element, "Tool");
		nodeVCFxCopTool->append_attribute(doc.allocate_attribute("Name", "VCFxCopTool"));
		nodeConfiguration->append_node(nodeVCFxCopTool);

		xml_node<> *nodeVCAppVerifierTool = doc.allocate_node(node_element, "Tool");
		nodeVCAppVerifierTool->append_attribute(doc.allocate_attribute("Name", "VCAppVerifierTool"));
		nodeConfiguration->append_node(nodeVCAppVerifierTool);

		xml_node<> *nodeVCPostBuildEventTool = doc.allocate_node(node_element, "Tool");
		nodeVCPostBuildEventTool->append_attribute(doc.allocate_attribute("Name", "VCPostBuildEventTool"));
		nodeConfiguration->append_node(nodeVCPostBuildEventTool);

		nodeConfigurations->append_node(nodeConfiguration);
	}
	root->append_node(nodeConfigurations);

	xml_node<> *nodeReferences = doc.allocate_node(node_element, "References");
	root->append_node(nodeReferences);

	xml_node<> *nodeFiles = doc.allocate_node(node_element, "Files");
	for (string::size_type i = 0; i < vecStFilter.size(); ++i)
	{
		if (vecStFilter[i].nParent == -1)
		{
			if (vecStFilter[i].strName.empty())
			{
				if (vecStFilter[i].vecFile.size() > 0)
				{
					xml_node<> *nodeFile = doc.allocate_node(node_element, "File");
					nodeFile->append_attribute(doc.allocate_attribute("RelativePath", vecStFilter[i].vecFile[0].c_str()));
					nodeFiles->append_node(nodeFile);
				}				
			} 
			else
			{
				AddFilterNode(&doc, nodeFiles, &vecStFilter[i], i);
			}
		}
	}
	root->append_node(nodeFiles);

	xml_node<> *nodeGlobals = doc.allocate_node(node_element, "Globals");
	root->append_node(nodeGlobals);

	print(back_inserter(strOut), doc, 0);
	return true;
}

void CParseVcxproj::AddFilterNode(rapidxml::xml_document<> *pDoc, rapidxml::xml_node<> *pNode, STFilter *pStFilter, int nIndex)
{
	using namespace rapidxml;
	xml_node<> *nodeFilter = pDoc->allocate_node(node_element, "Filter");
	nodeFilter->append_attribute(pDoc->allocate_attribute("Name", pStFilter->strName.c_str()));
	nodeFilter->append_attribute(pDoc->allocate_attribute("Filter", pStFilter->strExtensions.c_str()));
	nodeFilter->append_attribute(pDoc->allocate_attribute("UniqueIdentifier", pStFilter->strUniqueIdentifier.c_str()));
	for (string::size_type i = 0; i < pStFilter->vecFile.size(); ++i)
	{
		xml_node<> *nodeFile = pDoc->allocate_node(node_element, "File");
		nodeFile->append_attribute(pDoc->allocate_attribute("RelativePath", pStFilter->vecFile[i].c_str()));
		nodeFilter->append_node(nodeFile);
	}
	for (string::size_type i = 0; i < vecStFilter.size(); ++i)
	{
		if (nIndex == vecStFilter[i].nParent)
		{
			AddFilterNode(pDoc, nodeFilter, &vecStFilter[i], i);
		}
	}
	pNode->append_node(nodeFilter);
}

std::string CParseVcxproj::MacroConvert(const string &strValue)
{
	string strResult = strValue;
	string strMacro = "$(Configuration)";
	string::size_type nPos = strValue.find(strMacro);
	if (nPos != string::npos)
	{
		strResult.replace(nPos, strMacro.length(), "$(ConfigurationName)");
	}
	return strResult;
}

std::string CParseVcxproj::MacroExclude(const string &strValue)
{
	static vector<string> sVecMacro;
	if (sVecMacro.size() == 0)
	{
		sVecMacro.push_back("%(AdditionalIncludeDirectories)");
		sVecMacro.push_back("%(PreprocessorDefinitions)");
		sVecMacro.push_back("%(DisableSpecificWarnings)");
		sVecMacro.push_back("%(AdditionalDependencies)");
		sVecMacro.push_back("%(AdditionalLibraryDirectories)");
		sVecMacro.push_back("%(IgnoreSpecificDefaultLibraries)");
	}
	string strResult = strValue;
	for (string::size_type i = 0; i < sVecMacro.size(); ++i)
	{
		string::size_type nPos = strResult.find(sVecMacro[i]);
		if (nPos != string::npos)
		{
			strResult.erase(nPos, sVecMacro[i].length());
		}
	}
	return strResult;
}

std::string CParseVcxproj::SemicolonToSpace(const string &strValue)
{
	string strResult;
	for (string::size_type i = 0; i < strValue.size(); ++i)
	{
		if (strValue[i] == ';')
		{
			strResult.append(1, ' ');
		}
		else
		{
			strResult.append(1, strValue[i]);
		}
	}
	return strResult;
}