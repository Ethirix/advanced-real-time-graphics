#include "Helpers.h"

#include "DataStore.h"

//Must be deleted after usage!
wchar_t* Helpers::StringToWideString(std::string str)
{
	str.append(" ");
	auto* wideString = new wchar_t[str.size() + 1];
	size_t outSize;
	auto error = mbstowcs_s(&outSize, wideString,
		str.size(), str.c_str(), str.size() - 1);

	return wideString;
}

void Helpers::UpdateTitleBar(double deltaTime, const std::weak_ptr<GameObject>& obj, Vector3 force, Vector3 posForce, HWND hwnd)
{
	static double time = 0;
	time += deltaTime;

	if (constexpr double updateTime = 0.1; time >= updateTime)
	{
		std::string title = "SimpleEngine";
		// - FPS: ";
	    //title.append(std::to_string(static_cast<int>(1 / deltaTime)));

		if (!obj.expired())
		{
			title.append(" - Selected: [" + obj.lock()->Name + "]");

			auto pos = obj.lock()->Transform->GetWorldPosition();
			title.append(" Pos: " 
				+ std::to_string(pos.x) + " " 
				+ std::to_string(pos.y) + " " 
				+ std::to_string(pos.z));

			auto rot = MakeEulerAnglesFromQ(obj.lock()->Transform->GetRotation());
			title.append(" Rot: "
				+ std::to_string(rot.X) + " " 
				+ std::to_string(rot.Y) + " " 
				+ std::to_string(rot.Z));
			
			title.append(" - Force: "
				+ std::to_string(force.X) + " " 
				+ std::to_string(force.Y) + " " 
				+ std::to_string(force.Z));

			title.append(" - Position Force: "
				+ std::to_string(posForce.X) + " " 
				+ std::to_string(posForce.Y) + " " 
				+ std::to_string(posForce.Z));
		}
		else
		{
			title.append(" - Selected: None");
		}

		auto wTitle = StringToWideString(title);
	    SetWindowText(hwnd, wTitle);
	    delete[] wTitle;

		time -= updateTime;
	}
}