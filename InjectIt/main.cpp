#include "InjectIt/InjectIt.h"
#include "SimpleIni/SimpleIni.h"

#include <string>
#include <thread>
#include <chrono>
#include <psapi.h>

static InjectIt Inject;
static CSimpleIniA Ini;

int main()
{
	Ini.SetUnicode();

	if (Ini.LoadFile("config.ini") < 0) 
	{
		std::cout << "Failed to load config.ini" << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(10));
		return 1;
	}

	bool bFailed = false;
	bool bRunning = true;
	while (bRunning)
	{
		bFailed = false;

		bool bFoundProcess = false;

		std::string strDllPath = Ini.GetValue("Settings", "DllPath", "default");
		std::string strProcess = Ini.GetValue("Settings", "Process", "default");
		int iWaitTime = Ini.GetLongValue("Settings", "WaitTimeFromProcessFoundToInject_Seconds", 5);

		std::cout << "-----CONFIG-----" << std::endl;
		std::cout << "Dll Path.: " << strDllPath << std::endl;
		std::cout << "Process..: " << strProcess << std::endl;
		std::cout << "Wait Time: " << iWaitTime << std::endl;
		std::cout << "----------------" << std::endl;

		std::cout << "Waiting for process.";
		while (!bFoundProcess)
		{
			if (Inject.IsProcessRunning(strProcess))
			{
				bFoundProcess = true;  
			}
			else
			{
				std::cout << ".";
				std::this_thread::sleep_for(std::chrono::seconds(1)); 
			}
		}

		bFoundProcess = false;

		std::cout << std::endl;

		std::cout << "Waiting for " << iWaitTime << " seconds.." << std::endl;

		bool bFoundProcessWhileWaiting = true;
		for (int i = 0; i < iWaitTime; i++)
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));

			if (!Inject.IsProcessRunning(strProcess))
			{
				bFoundProcessWhileWaiting = false;
				break;
			}
		}

		if (!bFoundProcessWhileWaiting)
		{
			Inject.ClearConsole();
			continue;
		}
			

		EInjectStatus nStatus = Inject.Inject(strProcess, strDllPath);
		if (nStatus != STATUS_SUCCESS)
		{
			std::cout << "Injection failed.." << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(2));
			//bRunning = false;
			bFailed = true;
		}

		std::cout << "Waiting for process to close..." << std::endl;
		while (Inject.IsProcessRunning(strProcess))
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}

		Inject.ClearConsole();
	}

	if (bFailed)
	{
		std::cout << "Injection failed. Stopped loop. Please re-open the program to continue injecting." << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(10));
		return 1;
	}
	
	return 0;
}