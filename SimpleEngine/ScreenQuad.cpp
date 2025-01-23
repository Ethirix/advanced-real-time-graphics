#include "ScreenQuad.h"

ScreenQuad::ScreenQuad()
{
	Vertex* v = new Vertex[4];
	
	v[0].Position = { -1.0f, -1.0f, 0.0f };
	v[1].Position = { -1.0f, 1.0f, 0.0f };
	v[2].Position = { 1.0f, 1.0f, 0.0f };
	v[3].Position = { 1.0f, -1.0f, 0.0f };

	v[0].Normal = { 0-1.0f, 1.0f, 0.0f };
	v[1].Normal = { 0-1.0f, 1.0f, 0.0f };
	v[2].Normal = { 0-1.0f, 1.0f, 0.0f };
	v[3].Normal = { 0-1.0f, 1.0f, 0.0f };

	v[0].TextureCoordinate = { 0.0f, 1.0f };
	v[1].TextureCoordinate = { 0.0f, 0.0f };
	v[2].TextureCoordinate = { 1.0f, 0.0f };
	v[3].TextureCoordinate = { 1.0f, 1.0f };

	UINT i[] = {0, 1, 2, 0, 2, 3};

	_mesh = {
		.Name = "SSQ",
		.Vertices = {v, 4},
		.VertexIndices = {i, 6},
		.SmoothShaded = ,
		.VertexBuffer = ,
		.IndexBuffer = ,
		.Bounds =
	};
}
