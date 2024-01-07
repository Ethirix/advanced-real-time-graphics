#include <windows.h>
#include "SimpleEngine.h"

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
	SimpleEngine simpleEngine = SimpleEngine();

	if (FAILED(simpleEngine.Initialize(hInstance, nShowCmd)))
		return -1;

	MSG msg = {nullptr, 0, WPARAM(), LPARAM(), DWORD(), POINT() };

	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			simpleEngine.Update();
			simpleEngine.Draw();
		}
	}

	return static_cast<int>(msg.wParam);
}