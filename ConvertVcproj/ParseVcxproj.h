#pragma once
#include <string>
#include <vector>
using namespace std;

struct STVCCLCompilerTool{
	string strOptimization;
	string strAdditionalIncludeDirectories;
	string strPreprocessorDefinitions;
	string strMinimalRebuild;
	string strBasicRuntimeChecks;
	string strRuntimeLibrary;
	string strUsePrecompiledHeader;
	string strWarningLevel;
	string strDetect64BitPortabilityProblems;
	string strDebugInformationFormat;
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
	STVCCLCompilerTool stVCCLCompilerTool;
	STVCLibrarianTool stVCLibrarianTool;
};

struct STFilter{
	STFilter *pParent;
	string strName;
	string strFullName;
	vector<string> vecFile;
};

class CParseVcxproj
{
public:
	CParseVcxproj(void);
	~CParseVcxproj(void);

	bool Parse(char *pText);
	bool ParseFilters(char *pText);

protected:
	STConfiguration *GetSTConfigurationByCondition(string strCondition);
	STFilter *GetSTFilterByFullName(string strFullName);

private:
	string strProjectName;
	string strProjectGuid;
	string strRootNamespace;
	string strKeyword;
	vector<STConfiguration> vecStConfiguration;
	vector<STFilter> vecStFilter;
};
