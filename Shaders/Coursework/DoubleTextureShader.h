#pragma once


#include "../DXFramework/BaseShader.h"
#include "../DXFramework/ShaderArgs.h"

using namespace std;
using namespace DirectX;


class DoubleTextureShader : public BaseShader
{

public:

	DoubleTextureShader(ID3D11Device* device, HWND hwnd, WCHAR* vs_filename, WCHAR* ps_filename);
	~DoubleTextureShader();

	void SetShaderParameters(ShaderArgs& m_ShaderArgs);
	void SetShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* texture2);
	void Render(ID3D11DeviceContext* deviceContext, int vertexCount);

private:
	void InitShader(WCHAR*, WCHAR*);

private:
	ID3D11Buffer* m_matrixBuffer;
	ID3D11SamplerState* m_sampleState;
};
