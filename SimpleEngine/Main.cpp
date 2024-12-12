#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <windows.h>

#include "imgui.h"
#include "SimpleEngine.h"

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
	SimpleEngine simpleEngine = SimpleEngine();

	if (FAILED(simpleEngine.Initialise(hInstance, nShowCmd)))
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

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	return static_cast<int>(msg.wParam);
}
