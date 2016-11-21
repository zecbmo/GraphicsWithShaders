#include "GameObject.h"
#include "SphereMesh.h"
#include "CubeMesh.h"
#include "PlaneMesh.h"
#include "QuadMesh.h"
#include "PointMesh.h"
#include "TessellationMesh.h"
#include "Model.h"
#include "OrthoMesh.h"

GameObject::GameObject(ID3D11Device* device, ID3D11DeviceContext* deviceContext, WCHAR* textureFilename)
{
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
	m_Mesh = new SphereMesh(m_Device, m_DeviceContext, m_TextureFilename, resolution);
}

void GameObject::CreateCubeObject(int resolution)
{
	m_Mesh = new CubeMesh(m_Device, m_DeviceContext, m_TextureFilename, resolution);
}

void GameObject::CreatePlaneObject(int resolution)
{
	m_Mesh = new PlaneMesh(m_Device, m_DeviceContext, m_TextureFilename, resolution);
}

void GameObject::LoadModel(WCHAR * filename)
{
	m_Mesh = new Model(m_Device, m_DeviceContext, m_TextureFilename, filename);
}

void GameObject::CreateTesselationMesh()
{
	m_Mesh = new TessellationMesh(m_Device, m_DeviceContext, m_TextureFilename);
}

void GameObject::CreateOrthoMesh(int width, int height, int xPosition, int yPosition)
{
	m_Mesh = new OrthoMesh(m_Device, m_DeviceContext, width, height, xPosition, yPosition);
}

void GameObject::CreateQuadMesh()
{
	m_Mesh = new QuadMesh(m_Device, m_DeviceContext, m_TextureFilename);
}

void GameObject::CreatePointMesh()
{
	m_Mesh = new PointMesh(m_Device, m_DeviceContext, m_TextureFilename);
}

void GameObject::Render(D3D* Direct3D, Camera * Camera, BaseShader* Shader)
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
	worldMatrix = XMMatrixMultiply(preScale, scale);
		
	//Set the required shader args
	m_ShaderArgs.m_DeviceContext = Direct3D->GetDeviceContext();
	m_ShaderArgs.m_Texture = m_Mesh->GetTexture();
	m_ShaderArgs.m_WorldMatrix = worldMatrix;
	m_ShaderArgs.m_ViewMatrix = viewMatrix;
	m_ShaderArgs.m_ProjectionMatrix = projectionMatrix;
	
	//// Send geometry data (from mesh)
	m_Mesh->SendData(Direct3D->GetDeviceContext());

	//// Set shader parameters (matrices and texture)
	Shader->SetShaderParameters(m_ShaderArgs);

	//// Render object (combination of mesh geometry and shader process
	Shader->Render(Direct3D->GetDeviceContext(), m_Mesh->GetIndexCount());
}
