// Lab1.cpp
// Lab 1 example, simple textured quad
#include "App6.h"

App1::App1()
{
	m_Mesh = nullptr;
	shader = nullptr;
	light = nullptr;
}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in)
{
	// Call super init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in);

	// Create Mesh object
	m_Mesh = new SphereMesh(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"../res/DefaultDiffuse.png");
	
	//create the shader
	shader = new LightShader(m_Direct3D->GetDevice(),hwnd);
	

	//create the light
	light = new Light;
	light->SetDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	light->SetAmbientColour(0.2f, 0.2f, 0.2f, 1.0f);
	light->SetDirection(0.5f, -0.5f, 0.0f);

	//Render to texture stuff here
	m_render_texture = new RenderTexture(m_Direct3D->GetDevice(), screenWidth, screenHeight,
		SCREEN_NEAR, SCREEN_DEPTH);

	m_ortho_mesh = new OrthoMesh(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), 200, 150, -300, 225);

	text_shader = new TextureShader(m_Direct3D->GetDevice(), hwnd);
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
	//XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	////// Clear the scene. (default blue colour)
	//m_Direct3D->BeginScene(0.39f, 0.58f, 0.92f, 1.0f);

	////// Generate the view matrix based on the camera's position.
	//m_Camera->Update();

	////// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	//m_Direct3D->GetWorldMatrix(worldMatrix);
	//m_Camera->GetViewMatrix(viewMatrix);
	//m_Direct3D->GetProjectionMatrix(projectionMatrix);


	////// Send geometry data (from mesh)
	//m_Mesh->SendData(m_Direct3D->GetDeviceContext());

	//
	////// Set shader parameters (matrices and texture)
	//shader->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, m_Mesh->GetTexture(), light);

	////// Render object (combination of mesh geometry and shader process
	//shader->Render(m_Direct3D->GetDeviceContext(), m_Mesh->GetIndexCount());

	//
	//
	RenderToTexture();
	RenderScene();

	//// Present the rendered scene to the screen.
	//m_Direct3D->EndScene();

	return true;
}

void App1::RenderToTexture()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	// Set the render target to be the render to texture.
	m_render_texture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// Clear the render to texture.
	m_render_texture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 1.0f, 0.0f, 1.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Update();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_Mesh->SendData(m_Direct3D->GetDeviceContext());

	shader->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, m_Mesh->GetTexture(), light);
	// Render object (combination of mesh geometry and shader process
	shader->Render(m_Direct3D->GetDeviceContext(), m_Mesh->GetIndexCount());

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_Direct3D->SetBackBufferRenderTarget();


}

void App1::RenderScene()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, baseViewMatrix, orthoMatrix;

	// Clear the scene. (default blue colour)
	m_Direct3D->BeginScene(0.39f, 0.58f, 0.92f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Update();

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// Send geometry data (from mesh)
	m_Mesh->SendData(m_Direct3D->GetDeviceContext());
	// Set shader parameters (matrices and texture)
	shader->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, m_Mesh->GetTexture(), light);
	// Render object (combination of mesh geometry and shader process
	shader->Render(m_Direct3D->GetDeviceContext(), m_Mesh->GetIndexCount());

	m_Direct3D->TurnZBufferOff();

	m_Direct3D->GetOrthoMatrix(orthoMatrix);// ortho matrix for 2D rendering
	m_Camera->GetBaseViewMatrix(baseViewMatrix);

	m_ortho_mesh->SendData(m_Direct3D->GetDeviceContext());
	text_shader->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, m_render_texture->GetShaderResourceView());
	text_shader->Render(m_Direct3D->GetDeviceContext(), m_ortho_mesh->GetIndexCount());

	m_Direct3D->TurnZBufferOn();

	// Present the rendered scene to the screen.
	m_Direct3D->EndScene();

}

