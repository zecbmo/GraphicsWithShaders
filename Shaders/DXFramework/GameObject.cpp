#include "GameObject.h"
#include "SphereMesh.h"
#include "CubeMesh.h"
#include "PlaneMesh.h"
#include "QuadMesh.h"
#include "SpherePointMesh.h"
#include "MyTesselationMesh.h"
#include "Model.h"
#include "OrthoMesh.h"
#include "../Coursework/DissolveShader.h"

GameObject::GameObject(ID3D11Device* device, ID3D11DeviceContext* deviceContext, WCHAR* textureFilename)
{
	//Init the game object setting it's scale to 1.1.1.
	m_Device = device;
	m_DeviceContext = deviceContext;
	m_TextureFilename = textureFilename;

	m_Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
}

GameObject::~GameObject()
{
}

void GameObject::CreateSphereObject(int resolution)
{
	//Delete previous mesh if there is one already (For loading new meshes during runtime)
	if (m_Mesh)
	{
		delete m_Mesh;
	}
	//Load the selected mesh
	m_Mesh = new SphereMesh(m_Device, m_DeviceContext, m_TextureFilename, resolution);
}

void GameObject::CreateCubeObject(int resolution)
{
	if (m_Mesh)
	{
		delete m_Mesh;
	}
	m_Mesh = new CubeMesh(m_Device, m_DeviceContext, m_TextureFilename, resolution);
}

void GameObject::CreatePlaneObject(int resolution)
{
	if (m_Mesh)
	{
		delete m_Mesh;
	}
	m_Mesh = new PlaneMesh(m_Device, m_DeviceContext, m_TextureFilename, resolution);
}

void GameObject::LoadModel(WCHAR * filename)
{
	if (m_Mesh)
	{
		delete m_Mesh;
	}
	m_Mesh = new Model(m_Device, m_DeviceContext, m_TextureFilename, filename);
}

void GameObject::CreateTesselationMesh()
{
	if (m_Mesh)
	{
		delete m_Mesh;
	}
	m_Mesh = new MyTessellationMesh(m_Device, m_DeviceContext, m_TextureFilename);
}

void GameObject::CreateOrthoMesh(int width, int height, int xPosition, int yPosition)
{
	if (m_Mesh)
	{
		delete m_Mesh;
	}
	m_Mesh = new OrthoMesh(m_Device, m_DeviceContext, width, height, xPosition, yPosition);
}

void GameObject::CreateQuadMesh()
{
	if (m_Mesh)
	{
		delete m_Mesh;
	}
	m_Mesh = new QuadMesh(m_Device, m_DeviceContext, m_TextureFilename);
}

void GameObject::CreatePointMesh()
{
	if (m_Mesh)
	{
		delete m_Mesh;
	}
	m_Mesh = new SpherePointMesh(m_Device, m_DeviceContext, m_TextureFilename);
}

void GameObject::Render(D3D* Direct3D, Camera * Camera, BaseShader* Shader, ShaderArgs& shaderArgs)
{
	
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	//// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	Direct3D->GetWorldMatrix(worldMatrix);
	Camera->GetViewMatrix(viewMatrix);
	Direct3D->GetProjectionMatrix(projectionMatrix);

	//Do the Transforms on the Object
	XMMATRIX transform = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
	XMMATRIX rotation = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);	
	XMMATRIX scale = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);

	XMMATRIX preScale = XMMatrixMultiply(rotation, transform);
	worldMatrix = XMMatrixMultiply(scale, preScale);
		
	//Set the required shader args - means we don't have to do it manually (this is the base for what every shader needs)
	shaderArgs.m_DeviceContext = Direct3D->GetDeviceContext();
	shaderArgs.m_Texture = m_Mesh->GetTexture();
	shaderArgs.m_WorldMatrix = worldMatrix;
	shaderArgs.m_ProjectionMatrix = projectionMatrix;
	shaderArgs.m_ViewMatrix = viewMatrix;
	
	
	//Depending on the shader type we may want further options here
	if (Shader->GetShaderType() == kDissolveShader)
	{
		//turn on alpha blending for the dissolve shader
		Direct3D->TurnOnAlphaBlending();
	}

	//// Send geometry data (from mesh)
	m_Mesh->SendData(Direct3D->GetDeviceContext());



	//// Set shader parameters (matrices and texture)
	Shader->SetShaderParameters(shaderArgs);

	//// Render object (combination of mesh geometry and shader process
	Shader->Render(Direct3D->GetDeviceContext(), m_Mesh->GetIndexCount());

	//reset additional options based on shader type
	if (Shader->GetShaderType() == kDissolveShader)
	{
		//turn off alpha blending for dissolve shader
		Direct3D->TurnOffAlphaBlending();
	}

}

void GameObject::RenderFromLightSource(D3D* Direct3D, Light* light, BaseShader* Shader, ShaderArgs& shaderArgs)
{

	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	//// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	Direct3D->GetWorldMatrix(worldMatrix);
	//Camera->GetViewMatrix(viewMatrix);
	Direct3D->GetProjectionMatrix(projectionMatrix);

	//Do the Transforms on the Object
	XMMATRIX transform = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
	XMMATRIX rotation = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);
	XMMATRIX scale = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);

	XMMATRIX preScale = XMMatrixMultiply(rotation, transform);
	worldMatrix = XMMatrixMultiply(scale, preScale);

	XMMATRIX lightView = light->GetViewMatrix();
	XMMATRIX lightProjection = light->GetProjectionMatrix();

	//Set the required shader args - means we don't have to do it manually (this is the base for what every shader needs)
	shaderArgs.m_DeviceContext = Direct3D->GetDeviceContext();
	shaderArgs.m_Texture = m_Mesh->GetTexture();
	shaderArgs.m_WorldMatrix = worldMatrix;
	shaderArgs.m_ProjectionMatrix = lightProjection;
	shaderArgs.m_ViewMatrix = lightView;


	//// Send geometry data (from mesh)
	m_Mesh->SendData(Direct3D->GetDeviceContext());

	//Depending on the shader type we may want further options here
	if (Shader->GetShaderType() == kDissolveShader)
	{
		//turn on alpha blending for the dissolve shader
		Direct3D->TurnOnAlphaBlending();
	}

	//// Set shader parameters (matrices and texture)
	Shader->SetShaderParameters(shaderArgs);

	//// Render object (combination of mesh geometry and shader process
	Shader->Render(Direct3D->GetDeviceContext(), m_Mesh->GetIndexCount());

	//reset additional options based on shader type
	if (Shader->GetShaderType() == kDissolveShader)
	{
		//turn off alpha blending for dissolve shader
		Direct3D->TurnOffAlphaBlending();
	}

}

void GameObject::RenderTargetFunction(D3D* Direct3D,  BaseShader* Shader, ShaderArgs& shaderArgs)
{

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_Mesh->SendData(Direct3D->GetDeviceContext());

	Shader->SetShaderParameters(shaderArgs);
	// Render object (combination of mesh geometry and shader process
	Shader->Render(Direct3D->GetDeviceContext(), m_Mesh->GetIndexCount());
}
void GameObject::RenderSpinningObject(D3D* Direct3D, Camera * Camera, BaseShader* Shader, ShaderArgs& shaderArgs, float dt, float RotSpeed, float ScaleSpeed)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	//// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	Direct3D->GetWorldMatrix(worldMatrix);
	Camera->GetViewMatrix(viewMatrix);
	Direct3D->GetProjectionMatrix(projectionMatrix);

	m_Rotation.y += RotSpeed * dt;

	m_ScaleHelper += ScaleSpeed * dt;

	m_Scale.x = sin(m_ScaleHelper);
	m_Scale.z = sin(m_ScaleHelper);


	//Do the Transforms on the Object
	XMMATRIX transform = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
	XMMATRIX rotation = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);
	XMMATRIX scale = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);

	XMMATRIX preScale = XMMatrixMultiply(rotation, transform);
	worldMatrix = XMMatrixMultiply(scale, preScale);

	//Set the required shader args - means we don't have to do it manually (this is the base for what every shader needs)
	shaderArgs.m_DeviceContext = Direct3D->GetDeviceContext();
	shaderArgs.m_Texture = m_Mesh->GetTexture();
	shaderArgs.m_WorldMatrix = worldMatrix;
	shaderArgs.m_ProjectionMatrix = projectionMatrix;
	shaderArgs.m_ViewMatrix = viewMatrix;


	//Depending on the shader type we may want further options here
	if (Shader->GetShaderType() == kDissolveShader)
	{
		//turn on alpha blending for the dissolve shader
		Direct3D->TurnOnAlphaBlending();
	}

	//// Send geometry data (from mesh)
	m_Mesh->SendData(Direct3D->GetDeviceContext());



	//// Set shader parameters (matrices and texture)
	Shader->SetShaderParameters(shaderArgs);

	//// Render object (combination of mesh geometry and shader process
	Shader->Render(Direct3D->GetDeviceContext(), m_Mesh->GetIndexCount());

	//reset additional options based on shader type
	if (Shader->GetShaderType() == kDissolveShader)
	{
		//turn off alpha blending for dissolve shader
		Direct3D->TurnOffAlphaBlending();
	}

}