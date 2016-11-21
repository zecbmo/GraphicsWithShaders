// texture shader.h
#ifndef _DISSOLVESHADER_H_
#define _DISSOLVESHADER_H_

#include "../DXFramework/BaseShader.h"

using namespace std;
using namespace DirectX;


class DissolveShader : public BaseShader
{

public:

	DissolveShader(ID3D11Device* device, HWND hwnd);
	~DissolveShader();

	void SetShaderParameters(ShaderArgs& ShaderArgs);
	void SetShaderParameters(
		ID3D11DeviceContext* deviceContext, 
		const XMMATRIX &world, 
		const XMMATRIX &view, 
		const XMMATRIX &projection, 
		ID3D11ShaderResourceView* texture,
		ID3D11ShaderResourceView* dissolveMap,
		float dissolveAmount,
		float fringeSize
		
		);
	void Render(ID3D11DeviceContext* deviceContext, int vertexCount);

private:
	void InitShader(WCHAR*, WCHAR*);

private:
	ID3D11Buffer* m_matrixBuffer;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_dissolveBuffer;



	struct DissoleBufferType //to be passed to dissolve shader to determine amount the object will be dissolved 
	{
		float dissolveAmount;
		float fringeAmount;
		XMFLOAT2 padding;
	};

};

#endif