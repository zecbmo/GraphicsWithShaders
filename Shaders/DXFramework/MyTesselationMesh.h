
#ifndef _SMESH_H_
#define _SMESH_H_

#include "BaseMesh.h"

using namespace DirectX;

class MyTessellationMesh : public BaseMesh
{

public:
	MyTessellationMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, WCHAR* textureFilename, int resolution = 20);
	~MyTessellationMesh();

	void SendData(ID3D11DeviceContext*);
protected:
	void InitBuffers(ID3D11Device* device);
	int m_resolution;
};

#endif