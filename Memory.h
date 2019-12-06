#pragma once

#include <functional>
#include <Windows.h>

namespace palace
{
	class Memory
	{
	public:

		static bool AsEditable(void* AddressBeg, uint32_t size, std::function<void(void)> callback)
		{
			DWORD dwOldProtect, dwNewProtect;

			// Initial infos
			MEMORY_BASIC_INFORMATION mbi;
			if (!VirtualQuery(reinterpret_cast<void*>(AddressBeg), &mbi, sizeof(mbi))) return false;

			// remove single protection attributes and replace them with PAGE_EXECUTE_READWRITE
			dwNewProtect = mbi.Protect;
			dwNewProtect &= ~(PAGE_READONLY | PAGE_EXECUTE_READ | PAGE_EXECUTE);
			dwNewProtect |= PAGE_EXECUTE_READWRITE;

			// Unprotect memory
			if (!VirtualProtect(reinterpret_cast<void*>(AddressBeg), size, dwNewProtect, &dwOldProtect)) return false;

			callback();

			// Reprotect memory
			if (!VirtualProtect(reinterpret_cast<void*>(AddressBeg), size, dwOldProtect, &dwNewProtect)) return false;

			return true;
		}
	};
}