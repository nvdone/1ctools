//NVD 1C8 execute code utility
//Copyright © 2019, Nikolay Dudkin

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

#include <windows.h>
#include <stdio.h>

#include "lib\cmdline.hpp"
#include "lib\v8.hpp"

V8 *v8 = NULL;
CmdLine *cl = NULL;

void die(wchar_t * location, int error)
{
	wchar_t buf[256];
	memset(buf, 0, sizeof(wchar_t) * 256);
	swprintf(buf, 255, L"Error %d at %s", error, location);

	wprintf(buf);
	fflush(stdout);

	if(v8)
		delete v8;
	if(cl)
		delete cl;

	CoUninitialize();
	ExitProcess(error);
}

int wmain()
{
	int result = 0;
	HRESULT hr;

	wprintf(L"Exec1C 1.0\r\n(c) 2019, Nikolay Dudkin\r\n\r\n");

	cl = new CmdLine(GetCommandLine());
	if(cl->CountParams() < 3 || cl->CountCommands() != 2 || cl->HasParam(L"-?", 0))
	{
		wprintf(L"Usage: %s \"<1C connection string>\" -code:\"<1C code to execute>\"|-file:<PathToFileWithCode> [-com:<ComObjectName>]\r\n\r\n",  cl->GetCommand(0)->GetName(0));
		wprintf(L"1C configuration should export the following method:\r\n\r\n");
		wprintf(L"Процедура Exec1C(Код) Экспорт\r\n");
		wprintf(L"	Выполнить(Код);\r\n");
		wprintf(L"КонецПроцедуры\r\n\r\n");
		wprintf(L"Examples:\r\n");
		wprintf(L"	exec1c \"file=C:\\1C;usr=\"\"Администратор\"\"\" -file:code.txt\r\n");
		wprintf(L"	exec1c \"srvr=\"\"server.domain.com\"\";ref=\"\"Database\"\"\" -code:\"ОбщийМодуль.ВыполнитьПроцедуру();\" -com:V82.COMConnector");

		delete cl;

		return 0;
	}

	hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	if(FAILED(hr))
		die(L"CoInitializeEx", 1001);

	V8 *v8 = new V8();
	if(!v8)
		die(L"V8::V8", 1002);

	result = v8->Initialize(cl->GetOptionValue(L"-com", 1, 0) ? cl->GetOptionValue(L"-com", 1, 0) : L"V83.COMConnector");
	if(result)
		die(L"V8::Initialize", result);

	result = v8->Connect(cl->GetCommand(1)->GetName(0));
	if(result)
		die(L"V8::Connect", result);

	wchar_t *code = NULL;
	if(cl->HasParam(L"-code", 0))
	{
		code = cl->GetOptionValue(L"-code", 1, 0);
	}

	if(cl->HasParam(L"-file", 0))
	{
		HANDLE hFile = CreateFile(cl->GetOptionValue(L"-file", 1, 0),  GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hFile == INVALID_HANDLE_VALUE)
			die(L"CreateFile", 1003);
		DWORD fileSize = GetFileSize(hFile, NULL);

		char *buf = new char[fileSize + 1];
		memset(buf, 0, fileSize + 1);

		DWORD read;

		if(!ReadFile(hFile, buf, fileSize, &read, NULL))
		{
			CloseHandle(hFile);
			delete []buf;
			die(L"ReadFile", 1004);
		}

		CloseHandle(hFile);

		if(read != fileSize)
		{
			delete []buf;
			die(L"ReadFile", 1005);
		}

		wchar_t *buf2 = new wchar_t[fileSize + 1];
		memset(buf2, 0, sizeof(wchar_t) * (fileSize + 1));

		if(!MultiByteToWideChar(1251,MB_PRECOMPOSED,buf,fileSize,buf2,fileSize * 2))
		{
			delete []buf;
			delete []buf2;
			die(L"MultiByteToWideChar", 1006);
		}

		delete []buf;
		code = buf2;
	}

	if(!code)
		die(L"Main", 1007);

	result = v8->Execute(code);

	if(cl->HasParam(L"-file", 0))
		delete []code;

	if(result)
		die(L"V8::Execute", result);

	delete v8;
	delete cl;

	CoUninitialize();

	wprintf(L"Done.");

	return 0;
}
