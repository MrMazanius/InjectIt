#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <string>
#include <Psapi.h>

/*

Most functions copy pasted from https://github.com/adamhlt/DLL-Injector/
Thank you to @adamhlt for the functions.

*/

enum EInjectStatus : int
{
	STATUS_UNKNOWN,
	STATUS_SUCCESS,
	STATUS_ERROR
};

class InjectIt
{
private:
	bool m_bInit = false;
public:
	InjectIt()
	{
		m_bInit = true;
	}

	void ClearConsole();
	DWORD GetProcess(std::string strProcessName);
	EInjectStatus Inject(std::string strProcessName, std::string strDllPath);
	bool IsProcessRunning(const std::string& processName);
};

