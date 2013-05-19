#pragma once
#include <string>
#include <vector>
using namespace std;

class CParseSln
{
public:
	CParseSln(void);
	~CParseSln(void);

	bool Parse(const char *pText);
	bool Save(string &strOut);
	int GetVcxprojCount();
	const char* GetVcxprojByIndex(int nIndex);

private:
	vector<string> vecVcxproj;
	string strSln;
};
