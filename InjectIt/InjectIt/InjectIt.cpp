#include "InjectIt.h"

void InjectIt::ClearConsole()
{
    /*std::cout << "\x1B[2J\x1B[H";*/
    system("cls"); 
}

DWORD InjectIt::GetProcess(std::string strProcessName)
{
    char lpCurrentProcessName[255];

    PROCESSENTRY32 ProcList{};
    ProcList.dwSize = sizeof(ProcList);

    const HANDLE hProcList = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcList == INVALID_HANDLE_VALUE)
        return -1;

    if (!Process32First(hProcList, &ProcList))
        return -1;

    wcstombs_s(nullptr, lpCurrentProcessName, ProcList.szExeFile, 255);

    if (lstrcmpA(lpCurrentProcessName, strProcessName.c_str()) == 0)
        return ProcList.th32ProcessID;

    while (Process32Next(hProcList, &ProcList))
    {
        wcstombs_s(nullptr, lpCurrentProcessName, ProcList.szExeFile, 255);

        if (lstrcmpA(lpCurrentProcessName, strProcessName.c_str()) == 0)
            return ProcList.th32ProcessID;
    }

    return -1;
}

EInjectStatus InjectIt::Inject(std::string strProcessName, std::string strDllPath)
{
    std::cout << "-------------------------------------" << std::endl;
    std::cout << "[InjectIt] Starting injection" << std::endl;

    if (strDllPath.empty())
    {
        std::cout << "[InjectIt] Error: Empty dll path." << std::endl;
        std::cout << "-------------------------------------" << std::endl;
        return EInjectStatus::STATUS_ERROR;
    }
        
    const HANDLE hTargetProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetProcess(strProcessName));
    if (hTargetProcess == INVALID_HANDLE_VALUE)
    {
        std::cout << "[InjectIt] Error: Invalid handle value." << std::endl;
        std::cout << "-------------------------------------" << std::endl;
        return EInjectStatus::STATUS_ERROR;
    }

    std::cout << "[InjectIt] Successfully opened process" << std::endl;

    const LPVOID lpPathAddress = VirtualAllocEx(hTargetProcess, nullptr, lstrlenA(strDllPath.c_str()) + 1, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!lpPathAddress)
    {
        std::cout << "[InjectIt] Error: Path address was nullptr." << std::endl;
        std::cout << "-------------------------------------" << std::endl;
        return EInjectStatus::STATUS_ERROR;
    }

    std::cout << "[InjectIt] Memory allocate at 0x" << (UINT)(uintptr_t)lpPathAddress << std::endl;

    const DWORD dwWriteResult = WriteProcessMemory(hTargetProcess, lpPathAddress, strDllPath.c_str(), lstrlenA(strDllPath.c_str()) + 1, nullptr);
    if (dwWriteResult == 0)
    {
        std::cout << "[InjectIt] Error: Write result was 0." << std::endl;
        std::cout << "-------------------------------------" << std::endl;
        return EInjectStatus::STATUS_ERROR;
    }

    std::cout << "[InjectIt] Path written successfully" << std::endl;

    const HMODULE hModule = GetModuleHandleA("kernel32.dll");
    if (hModule == nullptr || hModule == INVALID_HANDLE_VALUE)
    {
        std::cout << "[InjectIt] Error: Module was invalid." << std::endl;
        std::cout << "-------------------------------------" << std::endl;
        return EInjectStatus::STATUS_ERROR;
    }

    const FARPROC lpFunctionAddress = GetProcAddress(hModule, "LoadLibraryA");
    if (!lpFunctionAddress)
    {
        std::cout << "[InjectIt] Error: Function address was nullptr." << std::endl;
        std::cout << "-------------------------------------" << std::endl;
        return EInjectStatus::STATUS_ERROR;
    }

    std::cout << "[InjectIt] LoadLibraryA address at 0x" << (UINT)(uintptr_t)lpFunctionAddress << std::endl;

    const HANDLE hThreadCreationResult = CreateRemoteThread(hTargetProcess, nullptr, 0, (LPTHREAD_START_ROUTINE)lpFunctionAddress, lpPathAddress, 0, nullptr);
    if (hThreadCreationResult == INVALID_HANDLE_VALUE)
    {
        std::cout << "[InjectIt] Error: An error is occured when trying to create the thread in the target process." << std::endl;
        std::cout << "-------------------------------------" << std::endl;
        return EInjectStatus::STATUS_ERROR;
    }

    std::cout << "[InjectIt] Dll injected successfully!" << std::endl;
    std::cout << "-------------------------------------" << std::endl;

	return EInjectStatus::STATUS_SUCCESS;
}

bool InjectIt::IsProcessRunning(const std::string& processName)
{
    DWORD processes[1024], cbNeeded, processCount;
    if (!EnumProcesses(processes, sizeof(processes), &cbNeeded))
    {
        return false;
    }

    processCount = cbNeeded / sizeof(DWORD);

    for (unsigned int i = 0; i < processCount; i++)
    {
        DWORD processID = processes[i];
        if (processID == 0) continue;

        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
        if (hProcess)
        {
            TCHAR processFileName[MAX_PATH];
            if (GetModuleFileNameEx(hProcess, NULL, processFileName, sizeof(processFileName) / sizeof(TCHAR)))
            {
                std::wstring wProcessFileName(processFileName);
                std::string processFileNameStr(wProcessFileName.begin(), wProcessFileName.end());

                size_t lastSlash = processFileNameStr.find_last_of("\\/");
                if (lastSlash != std::string::npos)
                {
                    processFileNameStr = processFileNameStr.substr(lastSlash + 1);
                }

                if (processName == processFileNameStr)
                {
                    CloseHandle(hProcess);
                    return true;
                }
            }
            CloseHandle(hProcess);
        }
    }

    return false;
}
