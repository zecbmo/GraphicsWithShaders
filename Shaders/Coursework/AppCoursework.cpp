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
	m_TextureNumber = 0;
	m_PrevTextureNumber = 0;

		
	// Create Game object
	m_GameObject = new GameObject(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"../res/DefaultDiffuse.png");
	m_GameObject->CreateCubeObject();
	
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

void App::CreateGUIWindow()
{

	ImGui_ImplDX11_NewFrame();
	{
		if (ImGui::BeginMainMenuBar())
		{
			ShaderArgs* Args = m_GameObject->ModifyShaderArgs();

			if (ImGui::BeginMenu("File2"))
			{
				if (ImGui::BeginMenu("Model Options"))
				{
					//DropDown to Pick Which Mesh
					ImGui::Combo("Mesh Select", &m_ModelNumber, "Cube\0Sphere\0Qaud\0Patrick Star\0SpongeBob\0Gary\0");

					//DropDown to Pick Which Texture
					ImGui::Combo("Texture Select", &m_TextureNumber,
						"Default\0Checker Board\0Bunny Rabbit\0Dissolve Map\0Gradient Map\0Patrick Star\0Spongebob\0Gary\0");

					ModelTransFormGUI();			

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Shader Options"))
				{
					//DropDown to Select Shader
					ImGui::Combo("Shader Select", &m_ShaderNumber, "Texture Shader\0Dissolve Shader\0\0");

					//Switch the GUI Based on what shader is selected (e.g. dissolve sliders will only show up when dissolve shader selected)
					switch (m_ShaderNumber)
					{
					case kTextureShader:
						break;
					case kDissolveShader:
						ImGui::SliderFloat("Dissolve amount", &Args->m_DissolveAmount,  0.0f, 1.0f);
						ImGui::SliderFloat("Fringe size", &Args->m_DissolveFringeSize,  0.0f, 1.0f);
						break;
					default:
						break;
					}
					ImGui::EndMenu();
				}

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		ImGui::ShowTestWindow();

	}
	/****************************************  Old window code - was juust a pop up window  ***************************************************/

	//Sets up the GUI window for the Project
	//ImGui_ImplDX11_NewFrame();
	//{
	//	//Shader Args, used to pass in controllable values to the GUI
	//	ShaderArgs* Args = m_GameObject->ModifyShaderArgs();
	//	//DropDown to Pick Which Mesh
	//	ImGui::Combo("Mesh Select", &m_ModelNumber, "Cube\0Sphere\0Qaud\0Patrick Star\0SpongeBob\0Gary\0");

	//	//DropDown to Pick Which Texture
	//	ImGui::Combo("Texture Select", &m_TextureNumber, 
	//		"Default\0Checker Board\0Bunny Rabbit\0Dissolve Map\0Gradient Map\0Patrick Star\0Spongebob\0Gary\0");

	//	//DropDown to Select Shader
	//	ImGui::Combo("Shader Select", &m_ShaderNumber, "Texture Shader\0Dissolve Shader\0\0");

	//	//Switch the GUI Based on what shader is selected (e.g. dissolve sliders will only show up when dissolve shader selected)
	//	switch (m_ShaderNumber)
	//	{
	//	case kTextureShader:
	//		break;
	//	case kDissolveShader:
	//		ImGui::DragFloat("Dissolve amount", &Args->m_DissolveAmount, 0.01f, 0.0f, 1.0f);
	//		ImGui::DragFloat("fringe size", &Args->m_DissolveFringeSize, 0.01f, 0.0f, 10.0f);
	//		break;
	//	default:
	//		break;
	//	}	
	//}
}

void App::ModelTransFormGUI()
{
	static bool a = false;
	if (ImGui::Button("Position Reset")) { printf("Clicked\n"); a ^= 1; }
	if (a)
	{
		m_GameObject->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
		a = false;
	}

	XMFLOAT3& pos_pointer = *m_GameObject->GetPositionPointer();
	float* pos[3] = { &pos_pointer.x, &pos_pointer.y, &pos_pointer.z };
	ImGui::SliderFloat3("Position", *pos, -10.0f, 10.0f);

	static bool b = false;
	if (ImGui::Button("Rotation Reset")) { printf("Clicked\n"); b ^= 1; }
	if (b)
	{
		m_GameObject->SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));
		b = false;
	}

	XMFLOAT3& rot_pointer = *m_GameObject->GetRotationPointer();
	float* rot[3] = { &rot_pointer.x, &rot_pointer.y, &rot_pointer.z };
	ImGui::SliderFloat3("Rotation", *rot, -10.0f, 10.0f);

	static bool c = false;
	if (ImGui::Button("Scale Reset")) { printf("Clicked\n"); c ^= 1; }
	if (c)
	{
		m_GameObject->SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
		c = false;
	}


	XMFLOAT3& scale_pointer = *m_GameObject->GetScalePointer();
	float* scale[3] = { &scale_pointer.x, &scale_pointer.y, &scale_pointer.z };
	ImGui::SliderFloat3("Scale", *scale, -10.0f, 10.0f);
}

void App::ReloadShape()
{
	if (m_PrevModelNumber != m_ModelNumber || m_PrevTextureNumber != m_TextureNumber)
	{
		

		//based off of int to work with ImGUI (A full Enum system would be nicer)

		switch (m_TextureNumber)
		{
		case kDefault: m_GameObject->ChangeTexture(L"../res/DefaultDiffuse.png");
			break;
		case kCheckerBoard: m_GameObject->ChangeTexture(L"../res/checkerboard.jpg");
			break;
		case kBunny: m_GameObject->ChangeTexture(L"../res/bunny.png");
			break;
		case kDissolveMap: m_GameObject->ChangeTexture(L"../res/dissolveMap2.jpg");
			break;
		case kGradientMap: m_GameObject->ChangeTexture(L"../res/dissolveMap.png");
			break;
		case kPatrickStarT: m_GameObject->ChangeTexture(L"../res/patrick.png");
			break;
		case kSpongeBobT: m_GameObject->ChangeTexture(L"../res/spongebob.png");
			break;
		case kGaryT: m_GameObject->ChangeTexture(L"../res/gary.png");
			break;
		default:
			break;
		}
		


		switch (m_ModelNumber)
		{
		case kCube: 
			m_GameObject->CreateCubeObject();
			break;
		case kSphere: m_GameObject->CreateSphereObject();
			break;
		case kQuad: m_GameObject->CreateQuadMesh();
			break;
		case kPatrickStar: 
			if (m_PrevModelNumber != m_ModelNumber)
			{
				m_GameObject->ChangeTexture(L"../res/patrick.png");
				m_TextureNumber = kPatrickStarT;
			}
			m_GameObject->LoadModel(L"../res/patrick.obj");
			break;
		case kSpongeBob: 
			if (m_PrevModelNumber != m_ModelNumber)
			{
				m_GameObject->ChangeTexture(L"../res/spongebob.png");
				m_TextureNumber = kSpongeBobT;
			}
			m_GameObject->LoadModel(L"../res/spongebob.obj");
			break;
		case kGary:
			if (m_PrevModelNumber != m_ModelNumber)
			{
				m_GameObject->ChangeTexture(L"../res/gary.png");
				m_TextureNumber = kGaryT;
			}
			m_GameObject->LoadModel(L"../res/gary.obj");
			break;
		default:
			break;
		}

		m_PrevModelNumber = m_ModelNumber;
		m_PrevTextureNumber = m_TextureNumber;

	}
}

bool App::Render()
{
	//Create the GUI window with all the fancy gidgets
	CreateGUIWindow();

	//Reload the shape if certain changes are made
	ReloadShape();
	

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

