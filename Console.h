#pragma once

#include <Windows.h>
#include <string>
#include <sstream>
#include <iostream>

namespace palace
{

	class Console
	{
	public:
		inline static HANDLE m_Handle;

		BOOL PushWindow(const std::string& strConsoleTitle)
		{
			if (!AllocConsole()) return false;

			std::freopen("conin$", "r+t", stdin);
			std::freopen("conout$", "w+t", stdout);
			std::freopen("conout$", "w+t", stderr);

			if (!strConsoleTitle.empty())
				SetConsoleTitle(strConsoleTitle.c_str());

			m_Handle = GetStdHandle(STD_OUTPUT_HANDLE);
			return SetConsoleTextAttribute(m_Handle, 2 | 1 | 4);
		}

		void PopWindow()
		{
			FreeConsole();
		}

		void ShowConsoleWindow()
		{
			auto hConsole = GetConsoleWindow();
			if (hConsole != nullptr)
				ShowWindow(hConsole, SW_SHOW);
		}

		void HideConsoleWindow()
		{
			auto hConsole = GetConsoleWindow();
			if (hConsole != nullptr)
				ShowWindow(hConsole, SW_HIDE);
		}
	};
}
