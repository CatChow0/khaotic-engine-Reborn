#include "systemclass.h"
#include <iostream>
#include <shellapi.h> // Include for DragAcceptFiles and DragQueryFile
#include <windows.h>
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

SystemClass::SystemClass()
{
	m_Input = 0;
	m_Application = 0;
	m_imguiManager = 0;
	
}

SystemClass::SystemClass(const SystemClass& other)
{
}

SystemClass::~SystemClass()
{
}

bool SystemClass::Initialize()
{
	int screenWidth, screenHeight;
	bool result;

	Logger::Get().Log("Initializing system class", __FILE__, __LINE__, Logger::LogLevel::Initialize);

	try
	{
		// Initialize the width and height of the screen to zero before sending the variables into the function.
		screenWidth = 0;
		screenHeight = 0;

		m_initialWindowWidth = 0;
		m_initialWindowHeight = 0;
		m_isDirect3DInitialized = false;

		// Initialize the windows api.
		InitializeWindows(screenWidth, screenHeight);

		// Create and initialize the input object.  This object will be used to handle reading the keyboard input from the user.
		m_Input = new InputClass;

		result = m_Input->Initialize(m_hinstance, m_hwnd, screenWidth, screenHeight);
		if (!result)
		{
			Logger::Get().Log("Failed to initialize input class", __FILE__, __LINE__, Logger::LogLevel::Error);
			return false;
		}

		// Create and initialize the application class object.  This object will handle rendering all the graphics for this application.
		m_Application = new ApplicationClass;

		result = m_Application->Initialize(screenWidth, screenHeight, m_hwnd);
		if (!result)
		{
			return false;
		}

		m_isDirect3DInitialized = true;

		// If we received a WM_SIZE message before Direct3D was initialized, resize the swap chain now
		if (m_initialWindowWidth > 0 && m_initialWindowHeight > 0)
		{
			m_Application->GetDirect3D()->ResizeSwapChain(m_initialWindowWidth, m_initialWindowHeight);
		}

		// Initialize imgui
		m_imguiManager = new imguiManager;
		result = m_imguiManager->Initialize(m_hwnd, m_Application->GetDirect3D()->GetDevice(), m_Application->GetDirect3D()->GetDeviceContext());
		if (!result)
		{
			return false;
		}
	}
	catch (const std::exception& e)
	{
		Logger::Get().Log(std::string("Exception caught during initialization: ") + e.what(), __FILE__, __LINE__, Logger::LogLevel::Error);
		return false;
	}

	Logger::Get().Log("System class initialized", __FILE__, __LINE__, Logger::LogLevel::Initialize);

	return true;
}

void SystemClass::Shutdown()
{
	Logger::Get().Log("Shutting down system class", __FILE__, __LINE__, Logger::LogLevel::Shutdown);

	std::lock_guard<std::mutex> guard(renderMutex);

	// Shutdown imgui
	if (m_imguiManager)
	{
		Logger::Get().Log("Shutting down imgui manager", __FILE__, __LINE__, Logger::LogLevel::Shutdown);

		m_imguiManager->Shutdown();
		delete m_imguiManager;
		m_imguiManager = 0;

		Logger::Get().Log("Imgui manager shut down", __FILE__, __LINE__, Logger::LogLevel::Shutdown);
	}

	// Release the application class object.
	if (m_Application)
	{
		Logger::Get().Log("Shutting down application", __FILE__, __LINE__, Logger::LogLevel::Shutdown);

		m_Application->Shutdown();
		delete m_Application;
		m_Application = 0;

		Logger::Get().Log("Application shut down", __FILE__, __LINE__, Logger::LogLevel::Shutdown);
	}

	// Release the input object.
	if (m_Input)
	{
		Logger::Get().Log("Shutting down input", __FILE__, __LINE__, Logger::LogLevel::Shutdown);

		delete m_Input;
		m_Input = 0;

		Logger::Get().Log("Input shut down", __FILE__, __LINE__, Logger::LogLevel::Shutdown);
	}



	// Shutdown the window.
	ShutdownWindows();

	Logger::Get().Log("System class shut down", __FILE__, __LINE__, Logger::LogLevel::Shutdown);

	return;
}

void SystemClass::Run()
{
	MSG msg;
	bool done, result;

	Logger::Get().Log("Running the system", __FILE__, __LINE__);

	// Initialize the message structure.
	ZeroMemory(&msg, sizeof(MSG));

	// Loop until there is a quit message from the window or the user.
	done = false;
	while (!done)
	{
		// Handle the windows messages.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{
				done = true;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		// If windows signals to end the application then exit out.
		if (m_Application != nullptr && m_Application->GetShouldQuit())
		{
			Logger::Get().Log("Received quit signal from application", __FILE__, __LINE__);
			done = true;
		}
		else
		{
			// Otherwise do the frame processing.
			result = Frame();
			if (!result)
			{
				Logger::Get().Log("Failed to process frame", __FILE__, __LINE__, Logger::LogLevel::Error);
				done = true;
			}
		}

	}

	return;
}

bool SystemClass::Frame()
{
	// Clear the buffers to begin the scene.
	m_Application->GetDirect3D()->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	std::lock_guard<std::mutex> guard(renderMutex);
	bool result;

	// Do the input frame processing.
	result = m_Input->Frame();
	if (!result)
	{
		Logger::Get().Log("Failed to process input frame", __FILE__, __LINE__, Logger::LogLevel::Error);
		return false;
	}

	// Do the frame processing for the application class object.
	result = m_Application->Frame(m_Input);
	if (!result)
	{
		Logger::Get().Log("Failed to process application frame", __FILE__, __LINE__, Logger::LogLevel::Error);
		return false;
	}

	// Render ImGui 
	result = m_imguiManager->ImGuiWidgetRenderer(m_Application);
	if (!result)
	{
		Logger::Get().Log("Failed to render ImGui widgets", __FILE__, __LINE__, Logger::LogLevel::Error);
		return false;
	}

	m_Application->GetDirect3D()->EndScene();

	return true;
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{

	if (ImGui_ImplWin32_WndProcHandler(hwnd, umsg, wparam, lparam))
	{
		return true;
	}

	switch (umsg)
	{
		// Check if a key has been pressed on the keyboard.
		case WM_KEYDOWN:
		{
			// If a key is pressed send it to the input object so it can record that state.
			m_Input->KeyDown((unsigned int)wparam);
			return 0;
		}

		// Check if a key has been released on the keyboard.
		case WM_KEYUP:
		{
			// If a key is released then send it to the input object so it can unset the state for that key.
			m_Input->KeyUp((unsigned int)wparam);
			return 0;
		}
		case WM_SIZE:
		{
			int newWidth = LOWORD(lparam);
			int newHeight = HIWORD(lparam);

			//  If Direct3D is initialized, update the swap chain. Otherwise, store the window dimensions
			if (m_isDirect3DInitialized && m_Application && m_Application->GetDirect3D())
			{
				m_Application->GetDirect3D()->ResizeSwapChain(newWidth, newHeight);
			}
			else
			{
				m_initialWindowWidth = newWidth;
				m_initialWindowHeight = newHeight;
			}
		}
		case WM_ENTERSIZEMOVE:
		{
			m_isResizing = true;
			break;
		}
		case WM_EXITSIZEMOVE:
		{
			m_isResizing = false;
			break;
		}
		case WM_DROPFILES:
		{
			HDROP hDrop = reinterpret_cast<HDROP>(wparam);
			UINT numFiles = DragQueryFile(hDrop, 0xFFFFFFFF, nullptr, 0);

			if (numFiles > 0) {
				for (UINT i = 0; i < numFiles; ++i) {
					WCHAR filePath[MAX_PATH];
					DragQueryFile(hDrop, i, filePath, MAX_PATH);

					// Get the file extension
					std::wstring fileName = filePath;
					std::wstring extension = fileName.substr(fileName.find_last_of(L".") + 1);

					// Check if the file has a valid extension
					if (extension == L"txt" || extension == L"kobj") {
						// Handle dropped files with valid extensions
						std::wcout << L"File dropped: " << filePath << std::endl;
						m_Application->AddKobject(filePath);
					}
					else {
						// Handle files with invalid extensions (optional)
						std::wcout << L"Ignored file: " << filePath << std::endl;
					}
				}
			}

			DragFinish(hDrop);
			return 0;
		}
		case WM_CLOSE:
		{
			Logger::Get().Log("WM_CLOSE message received", __FILE__, __LINE__);
			m_Application->SetShouldQuit(true);
			return 0;
		}
		// Any other messages send to the default message handler as our application won't make use of them.
		default:
		{
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}

	}

	return 0;
}

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	Logger::Get().Log("Initializing windows", __FILE__, __LINE__, Logger::LogLevel::Initialize);
	// Get an external pointer to this object.	
	ApplicationHandle = this;

	// Get the instance of this application.
	m_hinstance = GetModuleHandle(NULL);

	// Give the application a name.
	m_applicationName = L"Khaotic Engine";

	// Setup the windows class with default settings.
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(m_hinstance,MAKEINTRESOURCE(IDI_ICON1));
	wc.hIconSm = LoadIcon(m_hinstance, MAKEINTRESOURCE(IDI_ICON1));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	RegisterClassEx(&wc);

	// Determine the resolution of the clients desktop screen.
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	if (FULL_SCREEN)
	{
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		posX = posY = 0;
	}
	else
	{
		// If windowed then set it to 1600x900 resolution.
		screenWidth = 1600;
		screenHeight = 900;

		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// Create the window with the screen settings and get the handle to it.
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// Hide the mouse cursor.
	ShowCursor(true);

	//drag and drop 
	DragAcceptFiles(m_hwnd, TRUE);

	return;
}

void SystemClass::ShutdownWindows()
{
	Logger::Get().Log("Shutting down the windows", __FILE__, __LINE__, Logger::LogLevel::Shutdown);
	// Show the mouse cursor.
	ShowCursor(true);

	// Fix the display settings if leaving full screen mode.
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// Remove the window.
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// Remove the application instance.
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	// Release the pointer to this class.
	ApplicationHandle = NULL;

	return;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		// Check if the window is being destroyed.
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}

	// Check if the window is being closed.
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}

	case WM_DROPFILES:
	{
		ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		return(0);
	}

	// All other messages pass to the message handler in the system class.
	default:
	{
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
}

void SystemClass::SendPath(wchar_t* path, std::filesystem::path WFolder)
{
	m_Application->SetPath(path);
	m_Application->SetWFolder(WFolder);
}