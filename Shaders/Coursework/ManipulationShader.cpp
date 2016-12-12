// texture shader.cpp
#include "ManipulationShader.h"
#include "../DXFramework/ShaderArgs.h"

ManipulationShader::ManipulationShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	InitShader(L"shaders/manipulation_vs.hlsl", L"shaders/manipulation_ps.hlsl");
}


ManipulationShader::~ManipulationShader()
{
	// Release the sampler state.
	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
	}

	// Release the matrix constant buffer.
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	// Release the layout.
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// Release the light constant buffer.
	if (m_lightBuffer)
	{
		m_lightBuffer->Release();
		m_lightBuffer = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}


void ManipulationShader::InitShader(WCHAR* vsFilename, WCHAR* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC lightBufferArgsDesc;
	D3D11_BUFFER_DESC SpecPowerBufferDesc;
	D3D11_BUFFER_DESC cameraBufferDesc;
	D3D11_BUFFER_DESC timeBufferDesc;

	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	m_device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	m_device->CreateSamplerState(&samplerDesc, &m_sampleState);

	// Setup light buffer
	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	m_device->CreateBuffer(&lightBufferDesc, NULL, &m_lightBuffer);

	// Setup light buffer
	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	lightBufferArgsDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferArgsDesc.ByteWidth = sizeof(LightArgsBufferType);
	lightBufferArgsDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferArgsDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferArgsDesc.MiscFlags = 0;
	lightBufferArgsDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	m_device->CreateBuffer(&lightBufferArgsDesc, NULL, &m_lightArgsBuffer);


	SpecPowerBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	SpecPowerBufferDesc.ByteWidth = sizeof(SpecPowerBufferType);
	SpecPowerBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	SpecPowerBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	SpecPowerBufferDesc.MiscFlags = 0;
	SpecPowerBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	m_device->CreateBuffer(&lightBufferArgsDesc, NULL, &m_SpecPowerBuffer);


	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;

	m_device->CreateBuffer(&cameraBufferDesc, NULL, &cameraBuffer);

	//Time buffer... Need more time buffer marty
	timeBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	timeBufferDesc.ByteWidth = sizeof(TimeBufferType);
	timeBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	timeBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	timeBufferDesc.MiscFlags = 0;
	timeBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	m_device->CreateBuffer(&timeBufferDesc, NULL, &m_TimeBuffer);

}
void ManipulationShader::SetShaderParameters(ShaderArgs& ShaderArgs)
{
	if (ShaderArgs.m_Lights.empty())
	{
		error::ErrorMessage("No Lights Set when passed to LightShader - Update the List of Lights");
	}
	if (ShaderArgs.m_Lights.size() != MAX_LIGHTS)
	{
		error::ErrorMessage("Not All Lights defined when passed to LightShader - Update the List of Lights");
	}
	if (ShaderArgs.m_NumberOfLights > MAX_LIGHTS)
	{
		error::ErrorMessage("Tried to select too many lights in LightShader");
	}
	if (!ShaderArgs.m_DisplacementMap)
	{
		error::ErrorMessage("Displacement Map not Loaded");
	}

	//reset light if changed - will not be edited in the manipulation shader
	ShaderArgs.m_NumberOfLights = 1;
	auto light = ShaderArgs.m_Lights.begin();

	if (ShaderArgs.m_IsDisplacementMap)
	{
		(*light)->SetPosition(-10,10, 100);
		(*light)->SetDiffuseColour(1,1,1,1);
		(*light)->SetAmbientColour(0.2f, 0.2f, 0.2f, 1.0f);
		(*light)->SetSpecularColour(0.0f, 0.0f, 0.0f, 1.0f);
		(*light)->SetSpecularPower(100);
	}

	SetShaderParameters(
		ShaderArgs.m_DeviceContext,
		ShaderArgs.m_WorldMatrix,
		ShaderArgs.m_ViewMatrix,
		ShaderArgs.m_ProjectionMatrix,
		ShaderArgs.m_Texture,
		ShaderArgs.m_Lights,
		ShaderArgs
		);

}

void ManipulationShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix, ID3D11ShaderResourceView* texture, std::list<Light*>light, ShaderArgs& ShaderArgs)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	LightBufferType* lightPtr;
	LightArgsBufferType* lightArgsPtr;
	SpecPowerBufferType* specPtr;
	TimeBufferType* timePtr;

	unsigned int bufferNumber;
	XMMATRIX tworld, tview, tproj;
	CameraBufferType* cameraPtr;

	// Transpose the matrices to prepare them for the shader.
	tworld = XMMatrixTranspose(worldMatrix);
	tview = XMMatrixTranspose(viewMatrix);
	tproj = XMMatrixTranspose(projectionMatrix);

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;

	// Unlock the constant buffer.
	deviceContext->Unmap(m_matrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Now set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);




	deviceContext->Map(cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	cameraPtr = (CameraBufferType*)mappedResource.pData;
	cameraPtr->position = ShaderArgs.m_CameraPos;
	cameraPtr->padding = 0;
	deviceContext->Unmap(cameraBuffer, 0);
	bufferNumber = 1;
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &cameraBuffer);

	//Vs buffer time
	deviceContext->Map(m_TimeBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	timePtr = (TimeBufferType*)mappedResource.pData;
	//set time
	timePtr->time = ShaderArgs.m_Time;
	timePtr->height = ShaderArgs.m_Height;
	timePtr->isDisplacementMap = ShaderArgs.m_IsDisplacementMap;
	//unlock buffer
	deviceContext->Unmap(m_TimeBuffer, 0);
	//set constant buffer in vs
	bufferNumber = 2;
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_TimeBuffer);


	//Additional
	// Send light data to pixel shader
	deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	lightPtr = (LightBufferType*)mappedResource.pData;

	auto iter = light.begin();
	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		lightPtr->diffuseColor[i] = (*iter)->GetDiffuseColour();
		lightPtr->ambientColor[i] = (*iter)->GetAmbientColour();
		lightPtr->specularColor[i] = (*iter)->GetSpecularColour();


		iter++;
	}

	deviceContext->Unmap(m_lightBuffer, 0);
	bufferNumber = 0;
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);


	deviceContext->Map(m_lightArgsBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	lightArgsPtr = (LightArgsBufferType*)mappedResource.pData;

	lightArgsPtr->Args.x = ShaderArgs.m_NumberOfLights;
	lightArgsPtr->Args.y = ShaderArgs.m_TextureOn;
	lightArgsPtr->Args.z = ShaderArgs.m_IsDisplacementMap;
	lightArgsPtr->Args.w = 1;

	auto iter2 = light.begin();
	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		//get the position and set it as directional or positional
		XMFLOAT3 pos = (*iter2)->GetPosition();
		lightArgsPtr->position[i] = XMFLOAT4(pos.x, pos.y, pos.z, ShaderArgs.m_IsPointLight);
		iter2++;
	}

	deviceContext->Unmap(m_lightArgsBuffer, 0);
	bufferNumber = 1;
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightArgsBuffer);

	deviceContext->Map(m_SpecPowerBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	specPtr = (SpecPowerBufferType*)mappedResource.pData;

	auto iter3 = light.begin();

	Light* lightID[MAX_LIGHTS];
	int j = 0;
	for (auto iterlight : light)
	{
		lightID[j] = iterlight;
		j++;
	}
	float spec1 = lightID[0]->GetSpecularPower();
	float spec2 = lightID[1]->GetSpecularPower();
	float spec3 = lightID[2]->GetSpecularPower();
	float spec4 = lightID[3]->GetSpecularPower();

	specPtr->specularPower = XMFLOAT4(spec1, spec2, spec3, spec4);


	deviceContext->Unmap(m_SpecPowerBuffer, 0);
	bufferNumber = 2;
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_SpecPowerBuffer);


	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);

	ID3D11ShaderResourceView* displacementText = ShaderArgs.m_DisplacementMap->GetTexture();

	deviceContext->VSSetShaderResources(0, 1, &displacementText);
}

void ManipulationShader::Render(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the sampler state in the pixel shader.
	deviceContext->VSSetSamplers(0, 1, &m_sampleState);
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);



	// Base render function.
	BaseShader::Render(deviceContext, indexCount);
}



