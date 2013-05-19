#include "StdAfx.h"
#include "ParseSln.h"

CParseSln::CParseSln(void)
{
}

CParseSln::~CParseSln(void)
{
}

bool CParseSln::Parse(const char *pText)
{
	strSln = pText;
	string strTemp = "Microsoft Visual Studio Solution File, Format Version ";
	string::size_type nTemp = strSln.find(strTemp);
	if (nTemp == string::npos)
	{
		return false;
	}
	string::size_type nLineEnd = strSln.find('\r', nTemp);
	if (nLineEnd == string::npos)
	{
		return false;
	}
	string::size_type nVersion = nTemp + strTemp.length();
	string strVersion = strSln.substr(nVersion, nLineEnd - nVersion);
	if (strVersion == "10.00")
	{
		return false;
	}
	strSln.replace(nVersion, 2, "10");

	strTemp = "# Visual Studio ";
	nTemp = strSln.find(strTemp, nLineEnd);
	strSln.replace(nTemp + strTemp.length(), 4, "2008");

	string strVcx = ".vcxproj";
	string strProjectBegin = "Project(\"{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}\") = \"";
	strTemp = "\", \"";
	string::size_type nProjectBegin = strSln.find(strProjectBegin);
	while (nProjectBegin != string::npos)
	{
		string::size_type nPathBegin = strSln.find(strTemp, nProjectBegin + strProjectBegin.length());
		nPathBegin += strTemp.length();
		string::size_type nPathEnd = strSln.find(strTemp, nPathBegin);
		string strPath = strSln.substr(nPathBegin, nPathEnd - nPathBegin);
		vecVcxproj.push_back(strPath);

		nTemp = strPath.rfind(strVcx);
		strSln.replace(nPathBegin + nTemp, strVcx.length(), ".vcproj");

		nProjectBegin = strSln.find(strProjectBegin, nProjectBegin + 1);
	}
	return true;
}

bool CParseSln::Save(string &strOut)
{
	strOut = strSln;
	return true;
}

int CParseSln::GetVcxprojCount()
{
	return vecVcxproj.size();
}

const char* CParseSln::GetVcxprojByIndex(int nIndex)
{
	if (nIndex >= 0 && nIndex < GetVcxprojCount())
	{
		return vecVcxproj[nIndex].c_str();
	}
	return NULL;
}