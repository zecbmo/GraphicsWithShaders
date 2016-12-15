// texture shader.h
#ifndef _GEOMETRYSHADER_H_
#define _GEOMETRYSHADER_H_

#include "../DXFramework/BaseShader.h"

using namespace std;
using namespace DirectX;


class GeometryShader : public BaseShader
{

public:

	GeometryShader(ID3D11Device* device, HWND hwnd, WCHAR* gs_filename, WCHAR* ps_filename = L"shaders/GeometryShader_ps.hlsl");
	~GeometryShader();

	void SetShaderParameters(ShaderArgs& m_ShaderArgs);
	void SetShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture, XMFLOAT3 CameraPosition, XMFLOAT3 CameraUp, float offset);
	void Render(ID3D11DeviceContext* deviceContext, int vertexCount);

private:
	void InitShader(WCHAR*, WCHAR*);
	void InitShader(WCHAR* vsFilename, WCHAR* gsFilename, WCHAR* psFilename);

private:
	ID3D11Buffer* m_matrixBuffer;
	ID3D11SamplerState* m_sampleState;

	struct MatrixBufferCameraType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMFLOAT3 CameraPos;
		float padding;
		XMFLOAT3 CameraUpVec;
		float padding2;
	};
};

#endif