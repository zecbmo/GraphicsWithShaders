// Light shader.h
// Basic single light shader setup
#ifndef _LIGHTSHADER_H_
#define _LIGHTSHADER_H_

#include "../DXFramework/BaseShader.h"
#include "../DXFramework/Light.h"

using namespace std;
using namespace DirectX;



class LightShader : public BaseShader
{
private:
	struct LightBufferType
	{
		XMFLOAT4 diffuse;
		XMFLOAT3 direction;
		float padding;
		XMFLOAT4 ambient;
	};

	struct TimeBufferType
	{
		float time;
		float height;
		XMFLOAT2 padding;

	};

public:

	LightShader(ID3D11Device* device, HWND hwnd);
	~LightShader();

	void SetShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture, Light* light, float time, float height);
	void Render(ID3D11DeviceContext* deviceContext, int vertexCount);

private:
	void InitShader(WCHAR*, WCHAR*);

private:
	ID3D11Buffer* m_matrixBuffer;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_lightBuffer;
	ID3D11Buffer* m_TimeBuffer;

};

#endif