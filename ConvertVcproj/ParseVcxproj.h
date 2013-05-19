#pragma once
#include <string>
#include <vector>
#include "rapidxml/rapidxml.hpp"
using namespace std;

struct STVCPreBuildEventTool{
	string strDescription;
	string strCommandLine;
};
struct STVCMIDLTool{
	string strPreprocessorDefinitions;
	string strMkTypLibCompatible;
	string strTargetEnvironment;
	string strGenerateStublessProxies;
	string strTypeLibraryName;
	string strHeaderFileName;
	string strDLLDataFileName;
	string strInterfaceIdentifierFileName;
	string strProxyFileName;
};

struct STVCCLCompilerTool{
	string strOptimization;
	string strEnableIntrinsicFunctions;
	string strAdditionalIncludeDirectories;
	string strPreprocessorDefinitions;
	string strMinimalRebuild;
	string strExceptionHandling;
	string strBasicRuntimeChecks;
	string strRuntimeLibrary;
	string strEnableFunctionLevelLinking;
	string strTreatWChar_tAsBuiltInType;
	string strUsePrecompiledHeader;
	string strPrecompiledHeaderThrough;
	string strPrecompiledHeaderFile;
	string strWarningLevel;
	string strDetect64BitPortabilityProblems;
	string strDebugInformationFormat;
	string strCompileAs;
	string strDisableSpecificWarnings;
};

struct STVCResourceCompilerTool{
	string strPreprocessorDefinitions;
	string strCulture;
	string strAdditionalIncludeDirectories;
};
struct STVCPreLinkEventTool{
	string strCommandLine;
};

struct STVCLinkerTool{
	string strAdditionalDependencies;
	string strOutputFile;
	string strLinkIncremental;
	string strAdditionalLibraryDirectories;
	string strIgnoreAllDefaultLibraries;
	string strIgnoreDefaultLibraryNames;
	string strGenerateDebugInformation;
	string strSubSystem;
	string strImportLibrary;
	string strOptimizeReferences;
	string strEnableCOMDATFolding;
	string strTargetMachine;
};
struct STVCLibrarianTool{
	string strOutputFile;
};
struct STConfiguration{
	string strName;
	string strOutputDirectory;
	string strIntermediateDirectory;
	string strConfigurationType;
	string strCharacterSet;
	string strWholeProgramOptimization;
	STVCPreBuildEventTool stVCPreBuildEventTool;
	STVCMIDLTool stVCMIDLTool;
	STVCCLCompilerTool stVCCLCompilerTool;
	STVCResourceCompilerTool stVCResourceCompilerTool;
	STVCPreLinkEventTool stVCPreLinkEventTool;
	STVCLinkerTool stVCLinkerTool;
	STVCLibrarianTool stVCLibrarianTool;
};

struct STFilter{
	int nParent;
	string strName;
	string strFullName;
	string strUniqueIdentifier;
	string strExtensions;
	vector<string> vecFile;
};

class CParseVcxproj
{
public:
	CParseVcxproj(void);
	~CParseVcxproj(void);

	bool Parse(char *pText);
	bool ParseFilters(char *pText);
	bool Save(string &strOut);

protected:
	STConfiguration *GetSTConfigurationByCondition(string strCondition);
	STFilter *GetSTFilterByFullName(string strFullName);
	int GetSTFilterIndexByFullName(string strFullName);
	void AddFilterNode(rapidxml::xml_document<> *pDoc, rapidxml::xml_node<> *pNode, STFilter *pStFilter, int nIndex);
	string MacroConvert(const string &strValue);
	string MacroExclude(const string &strValue);
	string SemicolonToSpace(const string &strValue); 

private:
	string strProjectName;
	string strProjectGuid;
	string strRootNamespace;
	string strKeyword;
	vector<STConfiguration> vecStConfiguration;
	vector<STFilter> vecStFilter;
};
