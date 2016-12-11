// Lab1.cpp
// Lab 1 example, simple textured quad
#include "AppGeometry.h"

App::App()
{
	m_Mesh = nullptr;
	shader = nullptr;
	light = nullptr;
	point1 = nullptr;
	point2 = nullptr;
	point3 = nullptr;

	tess_value = 1.0f;
}

void App::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in)
{
	// Call super init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in);

	//Initialise ImGUI
	ImGui_ImplDX11_Init(hwnd, m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext());
		
	// Create Mesh object
	m_Mesh = new CubeMesh(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"../res/DefaultDiffuse.png");
	m_plane_mesh = new PlaneMesh(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"../res/DefaultDiffuse.png");
	
	//create the shader
	shader = new LightShader(m_Direct3D->GetDevice(),hwnd);
	

	//create the light
	light = new Light;
	light->SetDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	light->SetAmbientColour(0.2f, 0.2f, 0.2f, 1.0f);
	light->SetDirection(0.5f, -0.5f, 0.0f);
	light->SetPosition(10.f, 5.f, 10.0f);

	light->SetLookAt(0, 0, 0);
	light->GenerateProjectionMatrix(1,100);
	light->GenerateViewMatrix();

	time = 0;

	//Render to texture stuff here
	m_render_texture = new RenderTexture(m_Direct3D->GetDevice(), screenWidth, screenHeight,
		SCREEN_NEAR, SCREEN_DEPTH);
	m_box_render_texture = new RenderTexture(m_Direct3D->GetDevice(), screenWidth, screenHeight,
		SCREEN_NEAR, SCREEN_DEPTH);
	m_vert_render_texture = new RenderTexture(m_Direct3D->GetDevice(), screenWidth, screenHeight,
		SCREEN_NEAR, SCREEN_DEPTH);
	m_hor_render_texture = new RenderTexture(m_Direct3D->GetDevice(), screenWidth, screenHeight,
		SCREEN_NEAR, SCREEN_DEPTH);
	m_downsample_render_texture = new RenderTexture(m_Direct3D->GetDevice(), screenWidth/2, screenHeight/2,
		SCREEN_NEAR, SCREEN_DEPTH);
	m_upsample_render_texture = new RenderTexture(m_Direct3D->GetDevice(), screenWidth, screenHeight,
		SCREEN_NEAR, SCREEN_DEPTH);
	m_depth_texture = new RenderTexture(m_Direct3D->GetDevice(), screenWidth, screenHeight,
		SCREEN_NEAR, SCREEN_DEPTH);

	m_ortho_mesh = new OrthoMesh(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight,0,0);

	text_shader = new TextureShader(m_Direct3D->GetDevice(), hwnd);

	box_blur_shader = new BlurShader(m_Direct3D->GetDevice(), hwnd, L"shaders/BoxBlur_vs.hlsl", L"shaders/BoxBlur_ps.hlsl");
	vert_blur_shader = new BlurShader(m_Direct3D->GetDevice(), hwnd, L"shaders/verticalBlur_vs.hlsl", L"shaders/verticalBlur_ps.hlsl");
	hor_blur_shader = new BlurShader(m_Direct3D->GetDevice(), hwnd, L"shaders/horizontalBlur_vs.hlsl", L"shaders/horizontalBlur_ps.hlsl");


	width = screenWidth;
	height = screenHeight;

	//Tesselation
	tes_mesh = new MyTessellationMesh(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"../res/DefaultDiffuse.png");
	tes_shader = new TessellationShader(m_Direct3D->GetDevice(), hwnd);


	//disshader
	dis_shader = new DissolveShader(m_Direct3D->GetDevice(), hwnd);
	dissolveMap = new Texture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"../res/dissolveMap2.jpg");

	//Shadows and depth
	depth_shader = new DepthShader(m_Direct3D->GetDevice(), hwnd);
	shadow_shader = new ShadowShader(m_Direct3D->GetDevice(), hwnd);

	//Geometry
	geo_shader = new GeometryShader(m_Direct3D->GetDevice(), hwnd);
	point1 = new PointMesh(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"../res/DefaultDiffuse.png");
	point2 = new PointMesh(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"../res/DefaultDiffuse.png");
	point3 = new PointMesh(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"../res/DefaultDiffuse.png");
	
	

}


App::~App()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	ImGui_ImplDX11_Shutdown();
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


bool App::Frame()
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

bool App::Render()
{
	//Render Scene to texture
	

	//ImGui_ImplDX11_NewFrame();
	//{

	//
	//	ImGui::DragFloat("Tesselation Int", &tess_value, 0.05f, 1.0f, 32.0f);
	//	ImGui::DragFloat("Dissolve amount", &dis_amount, 0.01f, 0.0f, 1.0f);
	//	ImGui::DragFloat("fringe size", &fringe, 0.01f, 0.0f, 10.0f);

	//}

	//

	//RenderDissolve(dis_amount, fringe);

	//

	//
	////ImGui::Render();

	RenderGeometryShaderScene();

	




	m_Direct3D->EndScene();


	
	


	return true;
}

void App::RenderToTexture()
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

	shader->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, m_Mesh->GetTexture(), light,1);
	// Render object (combination of mesh geometry and shader process
	shader->Render(m_Direct3D->GetDeviceContext(), m_Mesh->GetIndexCount());

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_Direct3D->SetBackBufferRenderTarget();


}

void App::RenderDissolve(float dissolveAmount,float fringeSize)
{
	

	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	//// Clear the scene. (default blue colour)
	m_Direct3D->BeginScene(1.0f, 0.0f, 1.0f, 1.0f);

	//// Generate the view matrix based on the camera's position.
	m_Camera->Update();

	//// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);


	//// Send geometry data (from mesh)
	m_Mesh->SendData(m_Direct3D->GetDeviceContext());
	
	m_Direct3D->TurnOnAlphaBlending();

	//// Set shader parameters (matrices and texture)
	dis_shader->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, m_Mesh->GetTexture(), dissolveMap->GetTexture(), dissolveAmount, fringeSize);

	//// Render object (combination of mesh geometry and shader process
	dis_shader->Render(m_Direct3D->GetDeviceContext(), m_Mesh->GetIndexCount());
	
	m_Direct3D->TurnOffAlphaBlending();



}



void App::RenderScene(RenderTexture* rend)
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
	

	//turn off z depth and set ortho view
	m_Direct3D->TurnZBufferOff();

	m_Direct3D->GetOrthoMatrix(orthoMatrix);// ortho matrix for 2D rendering
	m_Camera->GetBaseViewMatrix(baseViewMatrix);

	//render final ortho mesh/texture

	m_ortho_mesh->SendData(m_Direct3D->GetDeviceContext());
	text_shader->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, rend->GetShaderResourceView());
	text_shader->Render(m_Direct3D->GetDeviceContext(), m_ortho_mesh->GetIndexCount());

	m_Direct3D->TurnZBufferOn();

	

}

void App::RenderBlurToTexture(RenderTexture* target, RenderTexture* from, BlurShader* b_shader)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	// Set the render target to be the render to texture.
	target->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// Clear the render to texture.
	target->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 1.0f, 0.0f, 1.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Update();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetBaseViewMatrix(viewMatrix);
	m_Direct3D->GetOrthoMatrix(projectionMatrix);

	//turn off z depth and set ortho view
	m_Direct3D->TurnZBufferOff();


	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_ortho_mesh->SendData(m_Direct3D->GetDeviceContext());

	b_shader->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, from->GetShaderResourceView(), width, height);
	// Render object (combination of mesh geometry and shader process
	b_shader->Render(m_Direct3D->GetDeviceContext(), m_ortho_mesh->GetIndexCount());

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_Direct3D->SetBackBufferRenderTarget();

	//turn off z depth and set ortho view
	m_Direct3D->TurnZBufferOn();

}

void App::SampleToDifferentTarget(RenderTexture * target, RenderTexture * from)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	// Set the render target to be the render to texture.
	target->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// Clear the render to texture.
	target->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 1.0f, 0.0f, 1.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Update();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetBaseViewMatrix(viewMatrix);
	m_Direct3D->GetOrthoMatrix(projectionMatrix);

	//turn off z depth and set ortho view
	m_Direct3D->TurnZBufferOff();


	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_ortho_mesh->SendData(m_Direct3D->GetDeviceContext());

	text_shader->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, from->GetShaderResourceView());
	// Render object (combination of mesh geometry and shader process
	text_shader->Render(m_Direct3D->GetDeviceContext(), m_ortho_mesh->GetIndexCount());

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_Direct3D->SetBackBufferRenderTarget();

	//turn off z depth and set ortho view
	m_Direct3D->TurnZBufferOn();
}
void App::RenderTesselation()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	//// Clear the scene. (default blue colour)
	m_Direct3D->BeginScene(1.0f, 0.0f, 1.0f, 1.0f);

	//// Generate the view matrix based on the camera's position.
	m_Camera->Update();

	//// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);


	//// Send geometry data (from mesh)
	tes_mesh->SendData(m_Direct3D->GetDeviceContext());



	//// Set shader parameters (matrices and texture)
	tes_shader->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, tes_mesh->GetTexture(), tess_value);

	//// Render object (combination of mesh geometry and shader process
	tes_shader->Render(m_Direct3D->GetDeviceContext(), m_Mesh->GetIndexCount());


	m_Direct3D->GetDeviceContext()->HSSetShader(NULL, NULL, 0);
	m_Direct3D->GetDeviceContext()->DSSetShader(NULL, NULL, 0);
}

void App::RenderDepthToTexture()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	// Set the render target to be the render to texture.
	m_depth_texture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// Clear the render to texture.
	m_depth_texture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 1.0f, 0.0f, 1.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Update();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);


	XMMATRIX lightView = light->GetViewMatrix();
	XMMATRIX lightProjection = light->GetProjectionMatrix();


	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_Mesh->SendData(m_Direct3D->GetDeviceContext());

	depth_shader->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, lightView, lightProjection);
	// Render object (combination of mesh geometry and shader process
	depth_shader->Render(m_Direct3D->GetDeviceContext(), m_Mesh->GetIndexCount());

	XMMATRIX transform = XMMatrixIdentity();
	XMMATRIX rotation = XMMatrixIdentity();
	transform = XMMatrixTranslation(-25.0, -1.0, -25);
	worldMatrix = transform;

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_plane_mesh->SendData(m_Direct3D->GetDeviceContext());

	depth_shader->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, lightView, lightProjection);
	// Render object (combination of mesh geometry and shader process
	depth_shader->Render(m_Direct3D->GetDeviceContext(), m_plane_mesh->GetIndexCount());
	
	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_Direct3D->SetBackBufferRenderTarget();

}

void App::RenderShadowsFromDepthTexture(RenderTexture * rend)
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

	time += m_Timer->GetTime();

	float bias = sinf(time)/1000.f;

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_Mesh->SendData(m_Direct3D->GetDeviceContext());

	shadow_shader->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, m_Mesh->GetTexture(), rend->GetShaderResourceView(), light, bias);
	// Render object (combination of mesh geometry and shader process
	shadow_shader->Render(m_Direct3D->GetDeviceContext(), m_Mesh->GetIndexCount());

	XMMATRIX transform = XMMatrixIdentity();
	XMMATRIX rotation = XMMatrixIdentity();
	transform = XMMatrixTranslation(-25.0, -1.0, -25);
	worldMatrix = transform;

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_plane_mesh->SendData(m_Direct3D->GetDeviceContext());

	shadow_shader->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, m_Mesh->GetTexture(), rend->GetShaderResourceView(), light, bias);
	// Render object (combination of mesh geometry and shader process
	shadow_shader->Render(m_Direct3D->GetDeviceContext(), m_plane_mesh->GetIndexCount());






}

void App::RenderGeometryShaderScene()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	//// Clear the scene. (default blue colour)
	m_Direct3D->BeginScene(1.0f, 1.0f, 1.25f, 1.0f);

	//// Generate the view matrix based on the camera's position.
	m_Camera->Update();

	//// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	XMMATRIX transform = XMMatrixIdentity();
	
	transform = XMMatrixTranslation(0, 0, 0);
	worldMatrix = transform;
	//// Send geometry data (from mesh)
	point1->SendData(m_Direct3D->GetDeviceContext());
	geo_shader->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, point1->GetTexture(), m_Camera->GetPosition(), m_Camera->GetUpVector());
	geo_shader->Render(m_Direct3D->GetDeviceContext(), point1->GetIndexCount());
	//transform = XMMatrixTranslation(1, 0, 0);
	//worldMatrix = transform;
	////// Send geometry data (from mesh)
	//point2->SendData(m_Direct3D->GetDeviceContext());
	//geo_shader->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, point2->GetTexture());


	//transform = XMMatrixTranslation(0, 1, 0);
	//worldMatrix = transform;
	////// Send geometry data (from mesh)
	//point3->SendData(m_Direct3D->GetDeviceContext());
	//geo_shader->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, point3->GetTexture());

	//// Set shader parameters (matrices and texture)
	//dis_shader->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, m_Mesh->GetTexture(), dissolveMap->GetTexture(), dissolveAmount, fringeSize);

	//// Render object (combination of mesh geometry and shader process
	//dis_shader->Render(m_Direct3D->GetDeviceContext(), m_Mesh->GetIndexCount());




}





