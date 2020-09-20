//NVD 1C8 automation library
//Copyright © 2019, Nikolay Dudkin

//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//GNU Lesser General Public License for more details.
//You should have received a copy of the GNU Lesser General Public License
//along with this program.If not, see<https://www.gnu.org/licenses/>.

#ifndef V8_HPP
#define V8_HPP

#define UNICODE

#include <windows.h>
#include <stdio.h>

class V8
{
	private:

		IDispatch *piV8, *piCon;

		int V8::autoWrap(IDispatch* piIDispatch, int autoType, LPOLESTR name, VARIANT* pvRes, int cArgs...);

	public:

		V8::V8();
		V8::~V8();

		int V8::Initialize(wchar_t *progId);
		int V8::Connect(wchar_t *connectionString);
		int V8::EditUser(wchar_t *user, int enable1cauth, wchar_t *pass, int lockpassword, int setvisible, int enabledomainauth, wchar_t *domainaccount, int warnunsafe);
		int V8::CancelTask(wchar_t *name, int log);
		int V8::Execute(wchar_t *code);
};

#endif