#include "GameObjectFactory.h"

#include "ConfigManager.h"
#include "Light.h"
#include "LookToCamera.h"

std::shared_ptr<RenderObject> GameObjectFactory::CreateRenderObject(
	const std::string& meshPath,
	MeshType meshType,
	const std::string& texturePath, 
	const ComPtr<ID3D11Device>& device)
{
	SP_RENDEROBJECT obj = std::make_shared<RenderObject>(RenderObject());
	if (OPT_SP_MESH mesh = Mesh::MeshFromOBJ(meshPath, device, meshType); mesh.has_value())
		obj->_mesh = mesh.value();
	else
		obj->_mesh = nullptr;

	if (OPT_SP_TEXTURES textures = Textures::CreateTextures(texturePath, device); textures.has_value())
		obj->_textures = textures.value();
	else
		obj->_textures = nullptr;

	obj->Transform->GameObject = obj->GetPtr();
	obj->SetParent({});

	return obj;
}

std::shared_ptr<Skybox> GameObjectFactory::CreateSkybox(
	const ComPtr<ID3D11Device>& device)
{
	std::shared_ptr<Skybox> skybox = std::make_shared<Skybox>(Skybox());

	OPT_SP_MESH mesh = Mesh::MeshFromOBJ("Meshes/InvertedCube2.obj", device, LeftHanded);
	if (!mesh.has_value())
		return {};
	skybox->_mesh = mesh.value();

	OPT_SP_TEXTURES textures = Textures::CreateTextures("Skybox", device);
	if (!textures.has_value())
		skybox->_textures = nullptr;
	skybox->_textures = textures.value();

	skybox->Transform->SetScale(1.0f, 1.0f, 1.0f);
	skybox->Transform->GameObject = skybox->GetPtr();
	skybox->_parent = {};

	return skybox;
}

std::shared_ptr<Camera> GameObjectFactory::CreateCamera(
	DirectX::XMFLOAT3 position, 
	DirectX::XMFLOAT3 at, 
	DirectX::XMFLOAT3 up,
	const std::shared_ptr<Screen>& screen)
{
	auto config = ConfigManager::GetConfig()->CameraConfig;

	auto camera = std::make_shared<Camera>(Camera());
	camera->_eye = position;
	camera->_at = at;
	camera->_up = up;
	camera->_screen = screen;
	camera->_nearDepth = config.NearPlane;
	camera->_farDepth = config.FarPlane;
	camera->_fieldOfView = config.FieldOfView;

	XMStoreFloat4x4(&camera->_view,
		DirectX::XMMatrixLookAtLH(
			XMLoadFloat3(&camera->_eye),
			XMLoadFloat3(&camera->_at),
			XMLoadFloat3(&camera->_up))
	);

	XMStoreFloat4x4(&camera->_projection,
		DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(camera->_fieldOfView),
			static_cast<float>(camera->_screen->Width) / static_cast<float>(camera->_screen->Height),
			camera->_nearDepth,
			camera->_farDepth)
	);

	camera->Transform->GameObject = camera->GetPtr();

	return camera;
}

std::shared_ptr<LookAtCamera> GameObjectFactory::CreateLookAtCamera(
		DirectX::XMFLOAT3 position,
		DirectX::XMFLOAT3 at,
		DirectX::XMFLOAT3 up,
		const std::shared_ptr<Screen>& screen)
{
	auto config = ConfigManager::GetConfig()->CameraConfig;

	auto camera = std::make_shared<LookAtCamera>(LookAtCamera());
	camera->_eye = position;
	camera->_at = at;
	camera->_up = up;
	camera->_screen = screen;
	camera->_nearDepth = config.NearPlane;
	camera->_farDepth = config.FarPlane;
	camera->_fieldOfView = config.FieldOfView;

	XMStoreFloat4x4(&camera->_view,
		DirectX::XMMatrixLookAtLH(
			XMLoadFloat3(&camera->_eye),
			XMLoadFloat3(&camera->_at),
			XMLoadFloat3(&camera->_up))
	);

	XMStoreFloat4x4(&camera->_projection,
		DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(camera->_fieldOfView),
			static_cast<float>(camera->_screen->Width) / static_cast<float>(camera->_screen->Height),
			camera->_nearDepth,
			camera->_farDepth)
	);

	camera->Transform->GameObject = camera->GetPtr();

	return camera;
}

std::shared_ptr<LookToCamera> GameObjectFactory::CreateLookToCamera(
	DirectX::XMFLOAT3 position,
	DirectX::XMFLOAT3 at,
	DirectX::XMFLOAT3 up,
	const std::shared_ptr<Screen>& screen)
{
	auto config = ConfigManager::GetConfig()->CameraConfig;

	auto camera = std::make_shared<LookToCamera>(LookToCamera());
	camera->_eye = position;
	camera->_at = at;
	camera->_up = up;
	camera->_screen = screen;
	camera->_nearDepth = config.NearPlane;
	camera->_farDepth = config.FarPlane;
	camera->_fieldOfView = config.FieldOfView;

	XMStoreFloat4x4(&camera->_view,
		DirectX::XMMatrixLookToLH(
			XMLoadFloat3(&camera->_eye),
			XMLoadFloat3(&camera->_at),
			XMLoadFloat3(&camera->_up))
	);

	XMStoreFloat4x4(&camera->_projection,
		DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(camera->_fieldOfView),
			static_cast<float>(camera->_screen->Width) / static_cast<float>(camera->_screen->Height),
			camera->_nearDepth,
			camera->_farDepth)
	);

	camera->Transform->GameObject = camera->GetPtr();

	return camera;
}

std::shared_ptr<Light> GameObjectFactory::CreateDirectionalLight(
	DirectX::XMFLOAT3 lightDirection,
	DirectX::XMFLOAT3 diffuseColor,
	float diffusePower,
	DirectX::XMFLOAT3 specularColor,
	float specularPower,
	DirectX::XMFLOAT3 ambientColor)
{
	auto light = std::make_shared<Light>(Light());
	light->DiffuseColor = diffuseColor;
	light->DiffusePower = diffusePower;
	light->SpecularColor = specularColor;
	light->SpecularPower = specularPower;
	light->AmbientColor = ambientColor;
	light->ConstantAttenuation = 1;

	light->Transform->SetPosition(lightDirection);
	light->Transform->GameObject = light->GetPtr();

	light->_mesh = nullptr;

	return light;
}

std::shared_ptr<Light> GameObjectFactory::CreatePointLight(
	DirectX::XMFLOAT3 lightPosition,
	DirectX::XMFLOAT3 diffuseColor,
	float diffusePower,
	DirectX::XMFLOAT3 specularColor,
	float specularPower,
	DirectX::XMFLOAT3 ambientColor,
	float constantAttenuation,
	float linearAttenuation,
	float quadraticAttention,
	float lightRadius,
	const ComPtr<ID3D11Device>& device)
{
	auto light = std::make_shared<Light>(Light());
	light->DiffuseColor = diffuseColor;
	light->DiffusePower = diffusePower;
	light->SpecularColor = specularColor;
	light->SpecularPower = specularPower;
	light->AmbientColor = ambientColor;
	light->ConstantAttenuation = constantAttenuation;
	light->LinearAttenuation = linearAttenuation;
	light->QuadraticAttenuation = quadraticAttention;
	light->LightRadius = lightRadius;

	light->_mesh = std::make_shared<Mesh>(Mesh("Point Light"));
	light->_mesh->Vertices = SafePointerArray<Vertex>();
	light->_mesh->Vertices.Length = 8;
	light->_mesh->Vertices.Elements = new Vertex[8];
	light->_mesh->Vertices.Elements[0] = Vertex({-1, 1, -1}, {-1, 1, -1}, {0, 1});
	light->_mesh->Vertices.Elements[1] = Vertex({1, 1, -1}, {1, 1, -1}, {1, 1});
	light->_mesh->Vertices.Elements[2] = Vertex({-1, -1, -1}, {-1, -1, -1}, {0, 0});
	light->_mesh->Vertices.Elements[3] = Vertex({1, -1, -1}, {1, -1, -1}, {1, 0});
	light->_mesh->Vertices.Elements[4] = Vertex({-1, 1, 1}, {-1, 1, 1}, {1, 0});
	light->_mesh->Vertices.Elements[5] = Vertex({1, 1, 1}, {1, 1, 1}, {0, 0});
	light->_mesh->Vertices.Elements[6] = Vertex({-1, -1, 1}, {-1, -1, 1}, {1, 1});
	light->_mesh->Vertices.Elements[7] = Vertex({1, -1, 1}, {1, -1, 1}, {0, 1});
	
	light->_mesh->VertexIndices = SafePointerArray<UINT>();
	light->_mesh->VertexIndices.Length = 36;
	light->_mesh->VertexIndices.Elements = new UINT[36]
	{
		0, 1, 2,
		2, 1, 3,
        3, 5, 7,
        1, 5, 3,
        6, 0, 2,
        6, 4, 0,
        5, 4, 7,
        7, 4, 6,
        4, 1, 0,
        5, 1, 4,
        2, 3, 6,
        6, 3, 7,
	};

	OPT_SP_MATERIAL material = Material::MaterialFromPath("Materials/defaultCube.mtl");
	if (!material.has_value())
		return {};
	light->_mesh->Material = material.value();
	light->_mesh->SmoothShaded = true;


	OPT_SP_TEXTURES textures = Textures::CreateTextures("Light", device);
	if (!textures.has_value())
		light->_textures = nullptr;
	light->_textures = textures.value();

	OPT_COM_BUFFER vertexBuffer = Mesh::CreateVertexBuffer("Default Cube", light->GetMesh(), device);
	if (!vertexBuffer.has_value())
		return {};
	light->_mesh->VertexBuffer = vertexBuffer.value();

	OPT_COM_BUFFER indexBuffer = Mesh::CreateIndexBuffer("Default Cube", light->GetMesh(), device);
	if (!indexBuffer.has_value())
		return {};
	light->_mesh->IndexBuffer = indexBuffer.value();

	light->Transform->SetPosition(lightPosition);
	light->Transform->SetScale(0.1f, 0.1f, 0.1f);
	light->Transform->GameObject = light->GetPtr();

	return light;
}