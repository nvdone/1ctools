//Clear 1C cache utility
//Copyright © 2020, Nikolay Dudkin

//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//GNU General Public License for more details.
//You should have received a copy of the GNU General Public License
//along with this program.If not, see<https://www.gnu.org/licenses/>.

#define UNICODE
#define __STDC_WANT_LIB_EXT1__ 1
#define APPNAME L"Clear 1C cache"

#include <windows.h>
#include <wchar.h>
#include <TlHelp32.h>
#include <Shlobj.h>

int DeleteDirectory(wchar_t *victim)
{
	HANDLE find;
	WIN32_FIND_DATA ffd;
	wchar_t path[MAX_PATH];
	int retval = 0;

	memset(path, 0, sizeof(wchar_t) * MAX_PATH);
	memcpy(path, victim, sizeof(wchar_t) * wcslen(victim));
	if(path[wcslen(path)-1] != L'\\')
		wcscat_s(path, MAX_PATH, L"\\");

	wcscat_s(path, MAX_PATH, L"*");

	find = FindFirstFile(path, &ffd);

	if(find != INVALID_HANDLE_VALUE)
	{
		do
		{
			if(!wcscmp(ffd.cFileName, L".") || !wcscmp(ffd.cFileName, L".."))
				continue;

			memset(path + wcslen(victim) + 1, 0, sizeof(wchar_t) * (MAX_PATH - wcslen(victim) - 1));
			wcscat_s(path, MAX_PATH, ffd.cFileName);

			if(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				retval |= DeleteDirectory(path);
			}
			else
			{
				if(ffd.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
					SetFileAttributes(path, FILE_ATTRIBUTE_NORMAL);
				if(!DeleteFile(path))
					retval = 1;
			}
		} while(FindNextFile(find, &ffd));

		FindClose(find);
	}

	if(!RemoveDirectory(victim))
		retval = 2;

	return retval;
}

int ClearProfileFolder(wchar_t *victim)
{
	HANDLE find;
	WIN32_FIND_DATA ffd;
	wchar_t path[MAX_PATH];
	int retval = 0;

	memset(path, 0, sizeof(wchar_t) * MAX_PATH);
	memcpy(path, victim, sizeof(wchar_t) * wcslen(victim));
	if(path[wcslen(path)-1] != L'\\')
		wcscat_s(path, MAX_PATH, L"\\");

	wcscat_s(path, MAX_PATH, L"*-*-*-*-*");

	find = FindFirstFile(path, &ffd);

	if(find != INVALID_HANDLE_VALUE)
	{
		do
		{
			if(!wcscmp(ffd.cFileName, L".") || !wcscmp(ffd.cFileName, L".."))
				continue;

			if(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				memset(path + wcslen(victim) + 1, 0, sizeof(wchar_t) * (MAX_PATH - wcslen(victim) - 1));
				wcscat_s(path, MAX_PATH, ffd.cFileName);
				retval |= DeleteDirectory(path);
			}
		} while(FindNextFile(find, &ffd));

		FindClose(find);
	}

	return retval;
}

int TerminateMyProcesses(wchar_t *victim)
{
	HANDLE snapshot;
	PROCESSENTRY32 pe = {0};
	HANDLE process = 0;
	HANDLE token = 0;

	PTOKEN_OWNER pme;
	PTOKEN_OWNER powner;

	DWORD len = 0;

	process = GetCurrentProcess();
	OpenProcessToken(process, TOKEN_QUERY, &token);
	GetTokenInformation(token, TokenOwner, NULL, 0, &len);
	pme = (PTOKEN_OWNER) LocalAlloc(LPTR, len);
	if(!pme)
		return 8;

	GetTokenInformation(token, TokenOwner, pme, len, &len);
	CloseHandle(token);
	CloseHandle(process);

	snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);

	if(snapshot == INVALID_HANDLE_VALUE)
	{
		LocalFree(pme);
		return 16;
	}

	pe.dwSize = sizeof(PROCESSENTRY32);
	Process32First(snapshot, &pe);
	do
	{
		if(!_wcsicmp(pe.szExeFile, victim))
		{
			process = OpenProcess(PROCESS_QUERY_INFORMATION, 0, pe.th32ProcessID);
			if(process)
			{
				if(OpenProcessToken(process, TOKEN_READ, &token))
				{
					CloseHandle(process);

					GetTokenInformation(token, TokenOwner, NULL, 0, &len);
					powner = (PTOKEN_OWNER) LocalAlloc(LPTR, len);
					if(!powner)
					{
						CloseHandle(token);
						CloseHandle(snapshot);
						LocalFree(pme);
						return 32;
					}
					if(GetTokenInformation(token, TokenOwner, powner, len, &len))
					{
						if(EqualSid(pme->Owner, powner->Owner))
						{
							process = OpenProcess(PROCESS_TERMINATE, 0, pe.th32ProcessID);
							if(process)
							{
								if(TerminateProcess(process, 0))
									WaitForSingleObject(process, 5000);
								CloseHandle(process);
							}
						}
					}
					
					LocalFree(powner);
					CloseHandle(token);
				}
				else
					CloseHandle(process);
			}
		}
	} while(Process32Next(snapshot, &pe));

	CloseHandle(snapshot);
	LocalFree(pme);

	return 0;
}

int WINAPI wWinMain(HINSTANCE a, HINSTANCE b, LPTSTR cmdline, int d)
{
	wchar_t path[MAX_PATH];
	int silent = 0;
	int retval = 0;

	if(!_wcsicmp(cmdline, L"-?") || !_wcsicmp(cmdline, L"/?"))
	{
		MessageBox(NULL, L"Clear 1C cache\r\n(C) Nikolay Dudkin, 2020\r\n\r\nUsage: c1cc.exe [-silent]", APPNAME, MB_OK | MB_ICONINFORMATION);
		return 0;
	}

	if(!_wcsicmp(cmdline, L"-silent") || !_wcsicmp(cmdline, L"/silent"))
		silent = 1;

	if(!silent && MessageBox(NULL, L"All instances of 1C application will be closed and 1C cache will be cleared.\r\n\r\nDo you want to continue?", APPNAME, MB_YESNO | MB_ICONQUESTION) != IDYES)
		return 64;

	TerminateMyProcesses(L"1cv8.exe");
	TerminateMyProcesses(L"1cv8s.exe");
	TerminateMyProcesses(L"1cv8c.exe");

	memset(path, 0, sizeof(wchar_t) * MAX_PATH);
	if(SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, path) != S_OK)
	{
		if(!silent)
			MessageBox(NULL, L"Internal error: SHGetFolderPath #1 failed!", APPNAME, MB_OK | MB_ICONSTOP);
		return 128;
	}
	wcscat_s(path, MAX_PATH, L"\\1C\\1cv8");
	retval |= ClearProfileFolder(path);

	memset(path, 0, sizeof(wchar_t) * MAX_PATH);
	if(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, path) != S_OK)
	{
		if(!silent)
			MessageBox(NULL, L"Internal error: SHGetFolderPath #2 failed!", APPNAME, MB_OK | MB_ICONSTOP);
		return 256;
	}
	wcscat_s(path, MAX_PATH, L"\\1C\\1cv8");
	retval |= ClearProfileFolder(path);

	if(!silent)
	{
		if(!retval)
			MessageBox(NULL, L"1C cache has been cleared completely.", APPNAME, MB_OK | MB_ICONINFORMATION);
		else
			MessageBox(NULL, L"1C cache has not been cleared completely.", APPNAME, MB_OK | MB_ICONEXCLAMATION);
	}

	return retval;
}
