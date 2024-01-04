#include "Helpers.h"

#include "RenderObject.h"

wchar_t* Helpers::StringToWideString(std::string str)
{
	str.append(" ");
	auto* wideString = new wchar_t[str.size() + 1];
	size_t outSize;
	auto error = mbstowcs_s(&outSize, wideString,
	str.size(), str.c_str(), str.size() - 1);

	return wideString;
}

void Helpers::UpdateTitleFPS(double deltaTime, std::weak_ptr<GameObject> obj, HWND hwnd)
{
	static double time = 0;
	time += deltaTime;

	if (time >= 0.25)
	{
		std::string title = "DirectX11 Framework - FPS: ";
	    title.append(std::to_string(static_cast<int>(1 / deltaTime)));

		if (!obj.expired())
		{
			if (auto ro = std::dynamic_pointer_cast<RenderObject>(obj.lock()))
				if (ro->GetMesh())
					title.append(" - Selected: [" + ro->GetMesh()->Name + "]");
				else
					title.append(" - Selected: [Object]");
			else
				title.append(" - Selected: [Object]");

			auto pos = obj.lock()->Transform->GetPosition();
			title.append(" Pos: " 
				+ std::to_string(pos.x) + " " 
				+ std::to_string(pos.y) + " " 
				+ std::to_string(pos.z));

			auto rot = obj.lock()->Transform->GetRotation();
			title.append(" Rot: "
				+ std::to_string(rot.x) + " " 
				+ std::to_string(rot.y) + " " 
				+ std::to_string(rot.z));
		}
		else
		{
			title.append(" - Selected: None");
		}

		auto wTitle = StringToWideString(title);
	    SetWindowText(hwnd, wTitle);
	    delete[] wTitle;

		time = 0;
	}
}
