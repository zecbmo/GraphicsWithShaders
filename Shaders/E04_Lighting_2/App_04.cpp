// Lab1.cpp
// Lab 1 example, simple textured quad
#include "App_04.h"

App1::App1()
{
	m_Mesh = nullptr;
	shader = nullptr;
	light = nullptr;
	light2 = nullptr;
}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in)
{
	// Call super init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in);

	// Create Mesh object
	m_Mesh = new SphereMesh(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"../res/DefaultDiffuse.png");
	m_quad = new QuadMesh(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"../res/DefaultDiffuse.png");
	
	//create the shader
	shader = new LightShader(m_Direct3D->GetDevice(),hwnd);
	

	//create the light
	light = new Light;
	light->SetDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	light->SetAmbientColour(0.0f, 0.0f, 0.0f, 1.0f);
	light->SetPosition(-5.0f, 10.0f, 5.0f);
	

	light2 = new Light;
	light2->SetDiffuseColour(1.0f, 0.0f, 0.0f, 1.0f);
	light2->SetAmbientColour(0.0f, 0.0f, 0.0f, 1.0f);
	light2->SetPosition(5.0f, 10.0f, -5.0f);

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
	if (m_quad)
	{
		delete m_quad;
		m_quad = 0;
	}
	if (shader)
	{		
		delete shader;
		shader = 0;
	}
	if (light)
	{
		delete light;
		light = 0;
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
	XMMATRIX scale = XMMatrixIdentity();


	
	//rotation = XMMatrixRotationRollPitchYaw(90.0, 0.0, 0.0);

	transform = XMMatrixTranslation(0.0, -1, 0);
	rotation = XMMatrixRotationRollPitchYaw(1.5708, 0.0,0.0);
	scale = XMMatrixScaling(500, 500, 1);

	worldMatrix = XMMatrixMultiply(rotation,transform);
	worldMatrix = XMMatrixMultiply( scale, worldMatrix);


	//// Send geometry data (from mesh)
	//m_Mesh->SendData(m_Direct3D->GetDeviceContext());

	
	//// Set shader parameters (matrices and texture)
	//shader->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, m_Mesh->GetTexture(), light);

	//// Render object (combination of mesh geometry and shader process
	//shader->Render(m_Direct3D->GetDeviceContext(), m_Mesh->GetIndexCount());

	m_quad->SendData(m_Direct3D->GetDeviceContext());
	//// Set shader parameters (matrices and texture)
	shader->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, m_quad->GetTexture(), light);
	//// Render object (combination of mesh geometry and shader process
	shader->Render(m_Direct3D->GetDeviceContext(), m_quad->GetIndexCount());
	

	//// Present the rendered scene to the screen.
	m_Direct3D->EndScene();

	return true;
}


