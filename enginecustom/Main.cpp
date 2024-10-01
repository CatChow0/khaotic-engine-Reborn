#include "systemclass.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	SystemClass* System;
	bool result;

	wchar_t path[MAX_PATH];
	HMODULE hmodule = GetModuleHandle(NULL);
	if (hmodule != NULL)
	{
		GetModuleFileName(hmodule, path, (sizeof(path) / sizeof(wchar_t)));
	}

	std::filesystem::path exePath(path);
	std::filesystem::path WFolder = exePath.parent_path();

	// Create the system object.
	System = new SystemClass;

	// Initialize and run the system object.
	result = System->Initialize();
	if (result)
	{
		Logger::Get().Log("System initialized", __FILE__, __LINE__, Logger::LogLevel::Initialize);
		System->SendPath(path,WFolder);
		System->Run();
	}

	// Shutdown and release the system object.
	System->Shutdown();
	delete System;
	System = 0;

	return 0;
}