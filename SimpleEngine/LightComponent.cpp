#include "LightComponent.h"
#include "GameObject.h"

void LightComponent::Update(double deltaTime)
{
	auto position = GameObject.lock()->Transform->GetPosition();
	Light.Position = { position.x, position.y, position.z, 1 };
}
