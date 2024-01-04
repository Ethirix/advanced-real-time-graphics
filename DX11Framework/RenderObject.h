#pragma once
#include "BlinnConstantBuffer.h"
#include "GameObject.h"
#include "Mesh.h"
#include "Textures.h"

#define SP_RENDEROBJECT std::shared_ptr<RenderObject>
#define WP_RENDEROBJECT std::weak_ptr<RenderObject>

class RenderObject : 
	public GameObject
{
	friend class GameObjectFactory;
public:
	~RenderObject() override = default;

	void Draw(const ComPtr<ID3D11DeviceContext>& context, const ComPtr<ID3D11Buffer>& constantBuffer, 
		const ComPtr<ID3D11Buffer>& lightBuffer,
		BlinnConstantBuffer constantBufferData, LightBuffer lightBufferData) override;

	void Draw(const ComPtr<ID3D11DeviceContext>& context,
	          const ComPtr<ID3D11Buffer>& constantBuffer,
	          PhongConstantBuffer constantBufferData) override;

	void Update(double deltaTime) override;

	void SetTextures(const SP_TEXTURES& textures);

	[[nodiscard]] SP_MESH GetMesh();
	[[nodiscard]] SP_TEXTURES GetTextures();
	
	
protected:
	RenderObject() = default;

	SP_MESH _mesh;
	SP_TEXTURES _textures;

	void ClearExpiredChildren();
};