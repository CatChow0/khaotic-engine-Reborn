#ifndef _SYSTEMCLASS_H_
#define _SYSTEMCLASS_H_

#define WIN32_LEAN_AND_MEAN

#include "Logger.h"

#include "inputclass.h"
#include "applicationclass.h"
#include "imguiManager.h"
#include <mutex>
#include "resources.h"

class SystemClass
{
public:
	SystemClass();
	SystemClass(const SystemClass&);
	~SystemClass();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

	void SendPath(wchar_t* path, std::filesystem::path WFolder);

private:
	bool Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	InputClass* m_Input;
	ApplicationClass* m_Application;
	imguiManager* m_imguiManager;

	int m_initialWindowWidth;
	int m_initialWindowHeight;
	bool m_isDirect3DInitialized;
	bool m_isResizing = false;

	std::mutex renderMutex;
};


/////////////////////////
// FUNCTION PROTOTYPES //
/////////////////////////
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


/////////////
// GLOBALS //
/////////////
static SystemClass* ApplicationHandle = 0;


#endif