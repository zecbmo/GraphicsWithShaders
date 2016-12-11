#pragma once

#include "BaseMesh.h"

class SpherePointMesh : public BaseMesh
{
public:
	SpherePointMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, WCHAR* textureFilename, int resolution = 12);
	~SpherePointMesh();

	void SendData(ID3D11DeviceContext* deviceContext);

private:
	void InitBuffers(ID3D11Device* device);
	int m_resolution;
};


