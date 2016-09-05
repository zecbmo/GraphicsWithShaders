#pragma once
#include "../DXFramework/trianglemesh.h"

using namespace DirectX;

class DoubleTriangle : public BaseMesh
{
public:
	DoubleTriangle(ID3D11Device* device, ID3D11DeviceContext* deviceContext, WCHAR* textureFilename);
	~DoubleTriangle();

private:
	void InitBuffers(ID3D11Device* device);
};
