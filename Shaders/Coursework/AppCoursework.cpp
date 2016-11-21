// Lab1.cpp
// Lab 1 example, simple textured quad
#include "AppCoursework.h"

App::App()
{
	m_GameObject = nullptr;
	m_Light = nullptr;


}

void App::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in)
{
	// Call super init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in);

	//Initialise ImGUI
	ImGui_ImplDX11_Init(hwnd, m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext());
	m_ShaderNumber = 0;
	m_ModelNumber = 0;
	m_PrevModelNumber = 0;

		
	// Create Game object
	m_GameObject = new GameObject(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"../res/DefaultDiffuse.png");
	m_GameObject->CreateSphereObject();
	
	//Set up the Shaders
	m_TextureShader = new TextureShader(m_Direct3D->GetDevice(),hwnd);
	m_DissolveShader = new DissolveShader(m_Direct3D->GetDevice(), hwnd);

	//Some initial Shader Args
	m_GameObject->ModifyShaderArgs()->m_DissolveMap = new Texture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"../res/dissolveMap2.jpg");

	//create the light
	m_Light = new Light;
	m_Light->SetDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetAmbientColour(0.2f, 0.2f, 0.2f, 1.0f);
	m_Light->SetDirection(0.5f, -0.5f, 0.0f);
	m_Light->SetPosition(10.f, 5.f, 10.0f);

	m_Light->SetLookAt(0, 0, 0);
	m_Light->GenerateProjectionMatrix(1,100);
	m_Light->GenerateViewMatrix();
}

void App::InitShaders(HWND hwnd)
{
}


App::~App()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	ImGui_ImplDX11_Shutdown();
	// Release the Direct3D object.
	if (m_GameObject)
	{
		delete m_GameObject;
		m_GameObject = 0;
	}
	if (m_TextureShader)
	{		
		delete m_TextureShader;
		m_TextureShader = 0;
	}
	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
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

void App::ReloadShape()
{
	if (m_PrevModelNumber != m_ModelNumber)
	{
		m_PrevModelNumber = m_ModelNumber;

		//based off of int to work with ImGUI (An Enum would be nicer)
		
		switch (m_PrevModelNumber)
		{
		case 0: 
			m_GameObject->CreateCubeObject();
			break;
		case 1: m_GameObject->CreateSphereObject();
			break;
		case 2: m_GameObject->CreateQuadMesh();
			break;
		default:
			break;
		}


	}
}

bool App::Render()
{
	//Render Scene to texture
	

	ImGui_ImplDX11_NewFrame();
	{
		ShaderArgs* Args = m_GameObject->ModifyShaderArgs();
		
		ImGui::Combo("Mesh Select", &m_ModelNumber, "Cube\0Sphere\0Model\0");
		
		ImGui::Combo("Shader Select", &m_ShaderNumber, "Texture Shader\0Dissolve Shader\0\0");

		if (m_ShaderNumber == 1)
		{
			ImGui::DragFloat("Dissolve amount", &Args->m_DissolveAmount, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("fringe size", &Args->m_DissolveFringeSize, 0.01f, 0.0f, 10.0f);
		}
	}
	ReloadShape();
	//

	//RenderDissolve(dis_amount, fringe);

	//

	//
	

	//// Clear the scene. (default blue colour)
	m_Direct3D->BeginScene(1.0f, 0.0f, 1.0f, 1.0f);

	//// Generate the view matrix based on the camera's position.
	m_Camera->Update();

	if (m_ShaderNumber == 0)
	{
		m_GameObject->Render(m_Direct3D, m_Camera, m_TextureShader);
	}
	else if (m_ShaderNumber == 1)
	{
		m_GameObject->Render(m_Direct3D, m_Camera, m_DissolveShader);
	}
	



	ImGui::Render();
	m_Direct3D->EndScene();


	
	


	return true;
}

