#pragma once



#include "../DXFramework/BaseShader.h"
#include "../DXFramework/Light.h"
#include <list>

using namespace std;
using namespace DirectX;

#define MAX_LIGHTS 4 //Hard Coded in Shader. Must be changed ibn Light.ps if changed here 

class ManipulationShader : public BaseShader
{
private:
	struct LightBufferType
	{
		XMFLOAT4 diffuseColor[MAX_LIGHTS];
		XMFLOAT4 ambientColor[MAX_LIGHTS];
		XMFLOAT4 specularColor[MAX_LIGHTS];


	};

	struct LightArgsBufferType
	{
		XMFLOAT4 position[MAX_LIGHTS]; //w == 0 directional light ----- w == 1 then positional light
		XMINT4 Args; // (x = number of lights, y = texture on)
	};

	struct SpecPowerBufferType //issues with buffers and seperating them fixes the issue
	{
		XMFLOAT4 specularPower;
	};

	struct CameraBufferType
	{
		XMFLOAT3 position;
		float padding;

	};
	struct TimeBufferType
	{
		float time;
		float height;
		float isDisplacementMap;
		float padding;

	};

public:

	ManipulationShader(ID3D11Device* device, HWND hwnd);
	~ManipulationShader();

	void SetShaderParameters(ShaderArgs& ShaderArgs);
	void SetShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture, std::list<Light*>light, ShaderArgs& ShaderArgs);
	void Render(ID3D11DeviceContext* deviceContext, int vertexCount);

private:
	void InitShader(WCHAR*, WCHAR*);

private:
	ID3D11Buffer* m_matrixBuffer;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_lightBuffer;
	ID3D11Buffer* m_lightArgsBuffer;
	ID3D11Buffer* m_SpecPowerBuffer;
	ID3D11Buffer* m_TimeBuffer;


	ID3D11Buffer* cameraBuffer;
};

