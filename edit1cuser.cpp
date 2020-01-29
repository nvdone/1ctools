//NVD 1C8 edit user utility
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

	wprintf(L"Edit1CUser 1.0\r\n(c) 2019, Nikolay Dudkin\r\n\r\n");

	cl = new CmdLine(GetCommandLine());
	if(cl->CountParams() < 4 || cl->CountCommands() != 3 || cl->HasParam(L"-?", 0))
	{
		wprintf(L"Usage: %s \"<1C connection string>\" <UserName> [-com:<ComObjectName>] [-enable1cauth:on|off] [-set1cpassword:<Password>] [-lockpassword:on|off] [-setvisible:on|off] [-enabledomainauth:on|off] [-setdomainaccount:<Account>] [-warnunsafe:on|off]\r\n\r\n",  cl->GetCommand(0)->GetName(0));
		wprintf(L"Examples:\r\n");
		wprintf(L"	edit1cuser \"file=C:\\1C;usr=\"\"Администратор\"\"\" Пользователь -enable1cauth:on\r\n");
		wprintf(L"	edit1cuser \"srvr=\"\"server.domain.com\"\";ref=\"\"Database\"\"\" \"Петров Иван\" -enabledomainauth:on -setdomainaccount:\"\\\\domain\\user\" -com:V82.COMConnector");

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

	int enable1cauth = -1;
	wchar_t *enable1cauth_s = cl->GetOptionValue(L"-enable1cauth", 1, 1);
	if(enable1cauth_s)
		enable1cauth = (wcscmp(enable1cauth_s, L"on") == 0);

	int lockpassword = -1;
	wchar_t *lockpassword_s = cl->GetOptionValue(L"-lockpassword", 1, 1);
	if(lockpassword_s)
		lockpassword = (wcscmp(lockpassword_s, L"on") == 0);

	int setvisible = -1;
	wchar_t *setvisible_s = cl->GetOptionValue(L"-setvisible", 1, 1);
	if(setvisible_s)
		setvisible = (wcscmp(setvisible_s, L"on") == 0);

	int enabledomainauth = -1;
	wchar_t *enabledomainauth_s = cl->GetOptionValue(L"-enabledomainauth", 1, 1);
	if(enabledomainauth_s)
		enabledomainauth = (wcscmp(enabledomainauth_s, L"on") == 0);

	int warnunsafe = -1;
	wchar_t *warnunsafe_s = cl->GetOptionValue(L"-warnunsafe", 1, 1);
	if(warnunsafe_s)
		warnunsafe = (wcscmp(warnunsafe_s, L"on") == 0);

	result = v8->EditUser(cl->GetCommand(2)->GetName(0), enable1cauth, cl->GetOptionValue(L"-set1cpassword", 1, 0), lockpassword, setvisible, enabledomainauth,  cl->GetOptionValue(L"-domainaccount", 1, 0), warnunsafe);
	if(result)
		die(L"V8::EnableUser", result);

	delete v8;
	delete cl;

	CoUninitialize();

	wprintf(L"Done.");

	return 0;
}
