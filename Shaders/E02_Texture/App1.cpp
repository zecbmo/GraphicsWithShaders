// Lab1.cpp
// Lab 1 example, simple textured quad
#include "App1.h"

App1::App1()
{
	m_Mesh = nullptr;
	m_TextureShader = nullptr;
	my_Mesh = nullptr;
	rot_num = 0;
}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in)
{
	// Call super init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in);

	// Create Mesh object
	m_Mesh = new QuadMesh(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"../res/bunny.png");
	my_Mesh = new QuadMesh(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"../res/checkerboard.png");

	m_TextureShader = new TextureShader(m_Direct3D->GetDevice(), hwnd);
	
}


App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the Direct3D object.
	if (m_Mesh)
	{
		delete m_Mesh;
		m_Mesh = 0;
	}
	if (my_Mesh)
	{
		delete my_Mesh;
		my_Mesh = 0;
	}

	if (m_TextureShader)
	{
		delete m_TextureShader;
		m_TextureShader = 0;
	}
}


bool App1::Frame()
{
	bool result;

	result = BaseApplication::Frame();
	if (!result)
	{
		return false;
	}

	// Render the graphics.
	result = Render();
	if (!result)
	{
		return false;
	}

	rot_num += 0.1f;
	return true;
}

bool App1::Render()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	//// Clear the scene. (default blue colour)
	m_Direct3D->BeginScene(0.39f, 0.58f, 0.92f, 1.0f);

	//// Generate the view matrix based on the camera's position.
	m_Camera->Update();

	//// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);


	XMMATRIX transform = XMMatrixIdentity();
	XMMATRIX rotation = XMMatrixIdentity();

	transform = XMMatrixTranslation(4.0, 0, 0);
	rotation = XMMatrixRotationRollPitchYaw(0.0, rot_num, rot_num);

	
	worldMatrix = XMMatrixRotationRollPitchYaw(0.0, 0.0, rot_num);
	

	//// Send geometry data (from mesh)
	m_Mesh->SendData(m_Direct3D->GetDeviceContext());

	my_Mesh->SendData(m_Direct3D->GetDeviceContext());


	//// Set shader parameters (matrices and texture)
	m_TextureShader->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, m_Mesh->GetTexture());
	
	//// Render object (combination of mesh geometry and shader process
	m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Mesh->GetIndexCount());


	//my mesh


	worldMatrix = XMMatrixMultiply( rotation, transform);
	//worldMatrix = XMMatrixMultiply(transform, rotation);


	m_TextureShader->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, my_Mesh->GetTexture());
	m_TextureShader->Render(m_Direct3D->GetDeviceContext(), my_Mesh->GetIndexCount());

	//// Present the rendered scene to the screen.
	m_Direct3D->EndScene();

	return true;
}


