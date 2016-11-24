// Lab1.cpp
// Lab 1 example, simple textured quad
#include "AppCoursework.h"

App::App()
{
	m_GameObject = nullptr;
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
	m_ShowDebugPositions = true;
	m_ShadowQuality = 0;
	m_ShowDepthFromLight = false;
		
	// Create Game object
	m_GameObject = new GameObject(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"../res/DefaultDiffuse.png");
	m_GameObject->CreateCubeObject();

	//plane for casting shadows
	m_ShadowPlane = new GameObject(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"../res/DefaultDiffuse.png");
	m_ShadowPlane->CreatePlaneObject();
	m_ShadowPlane->SetPosition(XMFLOAT3(-50, -5, -10));

	GameObject* ShadowSpere =  new GameObject(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"../res/DefaultDiffuse.png");
	ShadowSpere->CreateSphereObject();
	ShadowSpere->SetPosition(XMFLOAT3(-5,-2, 1));

	//Game object list for shadows
	m_ShadowObjects.push_back(m_GameObject);
	m_ShadowObjects.push_back(m_ShadowPlane);
	m_ShadowObjects.push_back(ShadowSpere);
	m_ShowDepthTexture = false;
	
	//SetUpLights
	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		m_LightDebug[i] = new GameObject(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"../res/DefaultDiffuse.png");
		m_LightDebug[i]->CreateSphereObject();
		m_LightDebug[i]->SetScale(XMFLOAT3(0.2f, 0.2f, 0.2f));
	}
	SetupLights();

	//Set up the Shaders
	m_TextureShader = new TextureShader(m_Direct3D->GetDevice(),hwnd);
	m_DissolveShader = new DissolveShader(m_Direct3D->GetDevice(), hwnd);
	m_LightShader = new LightShader(m_Direct3D->GetDevice(), hwnd);
	m_ManipulationShader = new ManipulationShader(m_Direct3D->GetDevice(), hwnd);
	m_DisplacementMapShader = new ManipulationShader(m_Direct3D->GetDevice(), hwnd);
	m_BoxBlurShader = new BlurShader(m_Direct3D->GetDevice(), hwnd, L"shaders/BoxBlur_vs.hlsl", L"shaders/BoxBlur_ps.hlsl");
	m_HorizontalBlurShader = new BlurShader(m_Direct3D->GetDevice(), hwnd, L"shaders/verticalBlur_vs.hlsl", L"shaders/verticalBlur_ps.hlsl");
	m_VerticalBlurShader = new BlurShader(m_Direct3D->GetDevice(), hwnd, L"shaders/horizontalBlur_vs.hlsl", L"shaders/horizontalBlur_ps.hlsl");
	m_ShadowShader = new ShadowShader(m_Direct3D->GetDevice(), hwnd);
	m_DepthShader = new DepthShader(m_Direct3D->GetDevice(), hwnd);

	//Some initial Shader Args
	m_ShaderArgs.m_DissolveMap = new Texture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"../res/dissolveMap2.jpg");
	m_ShaderArgs.m_DisplacementMap = new Texture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"../res/world.jpg");
	m_ShaderArgs.m_Height = 1;
	m_ShaderArgs.m_Time = 0;
	m_ShaderArgs.m_Speed = 1;
	m_ShaderArgs.m_ScreenWidth = screenWidth;
	m_ShaderArgs.m_ScreenHeight = screenHeight;
	m_ShaderArgs.m_ShadowBias = 0.0001f;

	//SetUp postprocessor
	m_PostProcessor = new PostProcessor(m_Direct3D, screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	m_PostProcessTexture = nullptr;
	SetUpPostProcessingScene();

	//Set Clear Colour
	m_ClearColour = XMFLOAT3(0.1f, 0.2f, 0.3f);
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


}


bool App::Frame()
{
	bool result;

	result = BaseApplication::Frame();
	if (!result)
	{
		return false;
	}

	//Update some shader Args
	m_ShaderArgs.m_CameraPos = m_Camera->GetPosition();
	m_ShaderArgs.m_Time += m_Timer->GetTime() * m_ShaderArgs.m_Speed;

	m_ShaderArgs.m_ShadowLight->GenerateViewMatrix();
	m_ShaderArgs.m_ShadowLight->GenerateProjectionMatrix(1, 1000);

	// Render the graphics.
	result = Render();
	if (!result)
	{
		return false;
	}

	
	return true;
}

void App::SetupLights()
{
	XMFLOAT4 colours[4] = { XMFLOAT4(1,1,1,1),
							XMFLOAT4(0,1,1,1),
							XMFLOAT4(1,0,1,1),
							XMFLOAT4(1,0,0,1) };

	XMFLOAT3 pos[4] = {		XMFLOAT3(-5,0,0),
							XMFLOAT3( 5,0,0),
							XMFLOAT3( 0,0,5),
							XMFLOAT3( 0,0,-5), };

	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		Light* Newlight = new Light;
		Newlight->SetDiffuseColour(colours[i]);
		Newlight->SetAmbientColour(0.2f, 0.2f, 0.2f, 1.0f);
		Newlight->SetLightPosition(pos[i]); //Give EachLight a dif position	
		Newlight->SetSpecularColour(1.0f, 1.0f, 1.0f, 1.0f);
		Newlight->SetSpecularPower(10);

		m_ShaderArgs.m_Lights.push_back(Newlight);
		Newlight = nullptr;
	}

	
	m_ShaderArgs.m_ShadowLight = new Light;
	m_ShaderArgs.m_ShadowLight->SetDiffuseColour(1,1,1,1);
	m_ShaderArgs.m_ShadowLight->SetAmbientColour(0.2f, 0.2f, 0.2f, 1.0f);
	m_ShaderArgs.m_ShadowLight->SetLightPosition(XMFLOAT3(10,10,10)); //Give EachLight a dif position	
	m_ShaderArgs.m_ShadowLight->SetDirection(0.5f, -0.5f, 0.0f);
	m_ShaderArgs.m_ShadowLight->SetSpecularColour(1.0f, 1.0f, 1.0f, 1.0f);
	m_ShaderArgs.m_ShadowLight->SetSpecularPower(10);
	m_ShaderArgs.m_ShadowLight->SetLookAt(0,0, 0);


	m_ShaderArgs.m_NumberOfLights = 1;

}

void App::SetUpPostProcessingScene()
{
	GameObject* car = new GameObject(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"../res/car.png");
	car->LoadModel(L"../res/car.obj");
	car->SetPosition(XMFLOAT3(-6, -5, 10));
	//car->SetScale(XMFLOAT3(0.02, 0.02, 0.02));

	GameObject* truck = new GameObject(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"../res/truck.png");
	truck->LoadModel(L"../res/truck.obj");
	truck->SetPosition(XMFLOAT3(6, -5, 13));
	//car->SetScale(XMFLOAT3(0.02, 0.02, 0.02));

	GameObject* dumpster = new GameObject(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"../res/dumpster.png");
	dumpster->LoadModel(L"../res/dumpster.obj");
	dumpster->SetPosition(XMFLOAT3(-7, -5, 20));

	GameObject* road = new GameObject(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"../res/road.jpg");
	road->CreatePlaneObject(200);
	road->SetPosition(XMFLOAT3(-100, -5, -50));

	GameObject* wall = new GameObject(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"../res/brick.jpg");
	wall->CreateCubeObject(20);
	wall->SetPosition(XMFLOAT3(0, -12, 40));
	wall->SetScale(XMFLOAT3(20, 10, 1));
	m_SceneObjects.push_back(wall);


	GameObject* rabbit = new GameObject(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"../res/rabbit.png");
	rabbit->LoadModel(L"../res/rabbit.obj");
	rabbit->SetPosition(XMFLOAT3(9, -5, 27));
	rabbit->SetRotation(XMFLOAT3(0, 45, 0));
	rabbit->SetScale(XMFLOAT3(0.5, 0.5, 0.5));
	m_SceneObjects.push_back(rabbit);

	GameObject* patrick = new GameObject(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"../res/patrick.png");
	patrick->LoadModel(L"../res/patrick.obj");
	patrick->SetPosition(XMFLOAT3(-11, -4.5, 7));
	patrick->SetRotation(XMFLOAT3(0, -45, 0));
	patrick->SetScale(XMFLOAT3(3, 3, 3));
	m_SceneObjects.push_back(patrick);


	CreatePathHelper(XMFLOAT3(-20, -5.5, 0));
	CreatePathHelper(XMFLOAT3(-20, -5.5, 20));
	CreatePathHelper(XMFLOAT3(20, -5.5, 0));
	CreatePathHelper(XMFLOAT3(20, -5.5, 20));

	CreatePathHelper(XMFLOAT3(-20, -5.5, -20));
	CreatePathHelper(XMFLOAT3(-20, -5.5, 40));
	CreatePathHelper(XMFLOAT3(20, -5.5, -20));
	CreatePathHelper(XMFLOAT3(20, -5.5, 40));
		
	CreateWallHelper(XMFLOAT3(-14, -11, -10));
	CreateWallHelper(XMFLOAT3(-14, -11, 30));
	CreateWallHelper(XMFLOAT3(14, -11, -10));
	CreateWallHelper(XMFLOAT3(14, -11, 30));
	
	CreateBuildingHelper(XMFLOAT3(-30, 0, 0), XMFLOAT3(10, 10, 10), L"../res/wall1.jpg");
	CreateBuildingHelper(XMFLOAT3(-30, 10, 20), XMFLOAT3(10, 20, 10), L"../res/wall2.jpg");
	CreateBuildingHelper(XMFLOAT3(-30, 5, 40), XMFLOAT3(10, 10, 10), L"../res/wall1.jpg");
	CreateBuildingHelper(XMFLOAT3(-30, -5, -20), XMFLOAT3(10, 20, 10), L"../res/wall3.jpg");
	CreateBuildingHelper(XMFLOAT3(0, 0, 70), XMFLOAT3(20, 20, 20), L"../res/wall4.jpg");
	CreateBuildingHelper(XMFLOAT3(30, -5, -20), XMFLOAT3(10, 10, 10), L"../res/wall1.jpg");
	CreateBuildingHelper(XMFLOAT3(30, 5, 40), XMFLOAT3(10, 20, 10), L"../res/wall2.jpg");
	CreateBuildingHelper(XMFLOAT3(30, 0, 20), XMFLOAT3(10, 10, 10), L"../res/wall1.jpg");
	CreateBuildingHelper(XMFLOAT3(30, 0, 0), XMFLOAT3(10, 20, 10), L"../res/wall3.jpg");





	m_SceneObjects.push_back(car);
	m_SceneObjects.push_back(truck);
	m_SceneObjects.push_back(road);
	m_SceneObjects.push_back(dumpster);
}

void App::CreatePathHelper(XMFLOAT3 pos)
{
	GameObject* path = new GameObject(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"../res/path.jpg");
	path->CreateCubeObject(20);
	path->SetPosition(pos);
	path->SetScale(XMFLOAT3(10, 1, 10));
	m_SceneObjects.push_back(path);
}

void App::CreateWallHelper(XMFLOAT3 pos)
{
	GameObject* wall = new GameObject(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"../res/brick.jpg");
	wall->CreateCubeObject(20);
	wall->SetPosition(pos);
	wall->SetScale(XMFLOAT3(1, 10, 20));
	m_SceneObjects.push_back(wall);
}

void App::CreateBuildingHelper(XMFLOAT3 pos, XMFLOAT3 scale, WCHAR* texture)
{
	GameObject* building = new GameObject(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), texture);
	building->CreateCubeObject(20);
	building->SetPosition(pos);
	building->SetScale(scale);
	m_SceneObjects.push_back(building);
}

void App::RenderPostProcessingScene()
{
	BaseShader* shaderToUse = nullptr;

	switch (m_PostProcessBaseShader)
	{
	case kTextureShader:
		shaderToUse = m_TextureShader;
		break;
	case kDissolveShader:
		shaderToUse = m_DissolveShader;
		break;
	case kLightShader:
		shaderToUse = m_LightShader;
		break;
	case kManipulationShader:
		shaderToUse = m_ManipulationShader;
		break;
	default:
		break;
	}



	switch (m_PostProcessShader)
	{
	case kNone:
	{
		//render Scene to texture
		m_PostProcessTexture = m_PostProcessor->RenderGameObjectsToTexture(m_SceneObjects, shaderToUse, m_ShaderArgs, m_Camera, m_ClearColour);
		m_PostProcessor->RenderTextureToScene(m_PostProcessTexture, m_Camera, m_TextureShader, m_ShaderArgs);

		break;
	}
	case kBoxBlur:
	{
		//render Scene to texture
		m_PostProcessTexture = m_PostProcessor->RenderGameObjectsToTexture(m_SceneObjects, shaderToUse, m_ShaderArgs, m_Camera, m_ClearColour);

		//Render Blur
		m_PostProcessTexture = m_PostProcessor->PostProcessTexture(m_PostProcessTexture, m_BoxBlurShader, m_ShaderArgs, m_Camera, m_ClearColour);

		//Render Final Image
		m_PostProcessor->RenderTextureToScene(m_PostProcessTexture, m_Camera, m_TextureShader, m_ShaderArgs);

		break;
	}
	case kGausianBlur:
	{
		//render Scene to texture
		m_PostProcessTexture = m_PostProcessor->RenderGameObjectsToTexture(m_SceneObjects, shaderToUse, m_ShaderArgs, m_Camera, m_ClearColour);

		//Down Scale
		m_PostProcessTexture = m_PostProcessor->PostProcessTexture(m_PostProcessTexture, m_TextureShader, m_ShaderArgs, m_Camera, m_ClearColour, kHalfScreenSize);

		//Hor Blur
		m_PostProcessTexture = m_PostProcessor->PostProcessTexture(m_PostProcessTexture, m_HorizontalBlurShader, m_ShaderArgs, m_Camera, m_ClearColour, kHalfScreenSize);

		//Vertical Blur
		m_PostProcessTexture = m_PostProcessor->PostProcessTexture(m_PostProcessTexture, m_VerticalBlurShader, m_ShaderArgs, m_Camera, m_ClearColour, kHalfScreenSize);

		//UpScale
		m_PostProcessTexture = m_PostProcessor->PostProcessTexture(m_PostProcessTexture, m_TextureShader, m_ShaderArgs, m_Camera, m_ClearColour);
		
		//Render Final Image
		m_PostProcessor->RenderTextureToScene(m_PostProcessTexture, m_Camera, m_TextureShader, m_ShaderArgs);


		break;
	}
	default:
		break;
	}
}

void App::CreateGUIWindow()
{

	ImGui_ImplDX11_NewFrame();
	{
		if (ImGui::BeginMainMenuBar())
		{
			ShaderArgs* Args = &m_ShaderArgs;

			if (ImGui::BeginMenu("Menu"))
			{
				if (ImGui::BeginMenu("Model Options"))
				{
					//DropDown to Pick Which Mesh
					ImGui::Combo("Mesh Select", &m_ModelNumber, "Cube\0Sphere\0Qaud\0Plane\0Patrick Star\0SpongeBob\0Gary\0");

					//DropDown to Pick Which Texture
					ImGui::Combo("Texture Select", &m_TextureNumber,
						"Default\0Checker Board\0Bunny Rabbit\0Dissolve Map\0Gradient Map\0Patrick Star\0Spongebob\0Gary\0");

					ModelTransFormGUI();			

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Shader Options"))
				{
					//DropDown to Select Shader
					ImGui::Combo("Shader Select", &m_ShaderNumber, "Texture Shader\0Dissolve Shader\0Light Shader\0Manipulation Shader\0Displacement Map Shader\0Post Processing Scene\0Shadow Shader");

					//Switch the GUI Based on what shader is selected (e.g. dissolve sliders will only show up when dissolve shader selected)
					switch (m_ShaderNumber)
					{
					case kTextureShader:
						break;
					case kDissolveShader:
						ImGui::SliderFloat("Dissolve amount", &Args->m_DissolveAmount,  0.0f, 1.0f);
						ImGui::SliderFloat("Fringe size", &Args->m_DissolveFringeSize,  0.0f, 1.0f);
						break;
					case kLightShader:
						ShowLightGUI();
						break;
					case kManipulationShader:
						ImGui::SliderFloat("Height", &Args->m_Height, 0.0f, 2.0f);
						ImGui::SliderFloat("Speed", &Args->m_Speed, 1.0f, 10.0f);
						break;
					case kDisplacementMapShader:
						ImGui::SliderFloat("Height", &Args->m_Height, 0.0f, 10.0f);
						break;
					case kPostProcessingScene:
						ImGui::Combo("Post Process Shader", &m_PostProcessShader, "Render To Texture\0Box Blur\0Gaussian Blur\0");
						ShowBasePostShadersGUI();						
						break;
					case kShadowShader:
					{
						ImGui::Combo("Shadow Quality", &m_ShadowQuality, "Normal\0Low\0High\0");
						ImGui::Checkbox("Show Depth Texture", &m_ShowDepthTexture);	
						
						if (!m_ShowDepthTexture)
						{
							ImGui::SameLine();
							ImGui::Checkbox("Show Depth From Light", &m_ShowDepthFromLight);
						}

						
						Light* light = m_ShaderArgs.m_ShadowLight;

						if (ImGui::CollapsingHeader("Light One Properties"))
						{
							//Get and Set Position with a slider in ImGUI
							XMFLOAT3 pos = light->GetPosition();
							float* light_pos[3] = { &pos.x, &pos.y, &pos.z };
							ImGui::DragFloat3("L1 Position", *light_pos, 0.2f, -10.0f, 10.0f);
							pos = XMFLOAT3(*light_pos[0], *light_pos[1], *light_pos[2]);
							light->SetLightPosition(pos);

							XMFLOAT4 diffuseCol = light->GetDiffuseColour();
							float* difcol[3]{ &diffuseCol.x,&diffuseCol.y, &diffuseCol.z };
							ImGui::ColorEdit3("L1 Diffuse Colour", *difcol);
							light->SetDiffuseColour(diffuseCol.x, diffuseCol.y, diffuseCol.z, 1.0f);							
						}

					}
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
	ImGui::DragFloat3("Position", *pos, 0.2f, -10.0f, 10.0f);

	static bool b = false;
	if (ImGui::Button("Rotation Reset")) { printf("Clicked\n"); b ^= 1; }
	if (b)
	{
		m_GameObject->SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));
		b = false;
	}

	XMFLOAT3& rot_pointer = *m_GameObject->GetRotationPointer();
	float* rot[3] = { &rot_pointer.x, &rot_pointer.y, &rot_pointer.z };
	ImGui::DragFloat3("Rotation", *rot, 0.2f, -10.0f, 10.0f);

	static bool c = false;
	if (ImGui::Button("Scale Reset")) { printf("Clicked\n"); c ^= 1; }
	if (c)
	{
		m_GameObject->SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
		c = false;
	}


	XMFLOAT3& scale_pointer = *m_GameObject->GetScalePointer();
	float* scale[3] = { &scale_pointer.x, &scale_pointer.y, &scale_pointer.z };
	ImGui::DragFloat3("Scale", *scale, 0.2f, -10.0f, 10.0f);
}

void App::ShowLightGUI()
{
	//Get Pointer to Args for easy access
	ShaderArgs* Args = &m_ShaderArgs;

	//Change number of lights with dropdown menu
	static int lightNo = Args->m_NumberOfLights - 1;
	ImGui::Combo("Number of Lights", &lightNo, "One\0Two\0Three\0Four\0");
	Args->m_NumberOfLights = lightNo + 1;

	//A button that when pressed will reset the lights positions
	static bool a = false;
	if (ImGui::Button("Reset Lights")) { printf("Clicked\n"); a ^= 1; }
	if (a)
	{
		XMFLOAT3 pos[4] = { XMFLOAT3(-5,0,0),
			XMFLOAT3(5,0,0),
			XMFLOAT3(0,0,5),
			XMFLOAT3(0,0,-5), };

		//loop through lights and reset
		int i = 0;
		for (auto iter : Args->m_Lights)
		{

			iter->SetLightPosition(pos[i]);
			i++;
		}
		a = false;
	}

	//A button that will show the debug positions 
	ImGui::SameLine(); ImGui::Checkbox("Show Debug Positions", &m_ShowDebugPositions);



	///////////////////The Intricies of ImGUI --- Have to set up all light params individually and cannot simply loop 

	//Get References to Each Light
	Light* lightID[MAX_LIGHTS];
	int j = 0;
	for (auto iter : Args->m_Lights)
	{

		lightID[j] = iter;
		j++;
	}
	
	XMFLOAT4 ambientCol = lightID[0]->GetAmbientColour();
	float* col[3]{ &ambientCol.x,&ambientCol.y, &ambientCol.z };
	ImGui::ColorEdit3("Ambient Colour", *col);
	lightID[0]->SetAmbientColour(ambientCol.x, ambientCol.y, ambientCol.z, 1.0f);
	
	int ID = 0;

	//If there is one light
	if (Args->m_NumberOfLights >= 1)
	{
		if (ImGui::CollapsingHeader("Light One Properties"))
		{		

			//Get and Set Position with a slider in ImGUI
			XMFLOAT3 pos = lightID[ID]->GetPosition();
			float* light_pos[3] = { &pos.x, &pos.y, &pos.z };
			ImGui::DragFloat3("L1 Position", *light_pos, 0.2f, -10.0f, 10.0f);
			pos = XMFLOAT3(*light_pos[0], *light_pos[1], *light_pos[2]);
			lightID[ID]->SetLightPosition(pos);

			XMFLOAT4 diffuseCol = lightID[ID]->GetDiffuseColour();
			float* difcol[3]{ &diffuseCol.x,&diffuseCol.y, &diffuseCol.z };
			ImGui::ColorEdit3("L1 Diffuse Colour", *difcol);
			lightID[ID]->SetDiffuseColour(diffuseCol.x, diffuseCol.y, diffuseCol.z, 1.0f);

			XMFLOAT4 specularCol = lightID[ID]->GetSpecularColour();
			float* speccol[3]{ &specularCol.x,&specularCol.y, &specularCol.z };
			ImGui::ColorEdit3("L1 Specular Colour", *speccol);
			lightID[ID]->SetSpecularColour(specularCol.x, specularCol.y, specularCol.z, 1.0f);

			float pow = lightID[ID]->GetSpecularPower();
			float* powp = &pow;
			ImGui::DragFloat("L1 Specular Power", powp, 1.f, 1.0f, 1000.0f);
			lightID[ID]->SetSpecularPower(pow);
		}

		

	}
	
	ID = 1;

	//If there is Two lights
	if (Args->m_NumberOfLights >= 2)
	{

		if (ImGui::CollapsingHeader("Light Two Properties"))
		{

			//Add Seperation and Header
			ImGui::Separator();
			ImGui::Text("Light Two : L2");

			//Get and Set Position with a slider in ImGUI
			XMFLOAT3 pos = lightID[1]->GetPosition();
			float* light_pos[3] = { &pos.x, &pos.y, &pos.z };
			ImGui::DragFloat3("L2 Position", *light_pos, 0.2f, -10.0f, 10.0f);
			pos = XMFLOAT3(*light_pos[0], *light_pos[1], *light_pos[2]);
			lightID[1]->SetLightPosition(pos);

			XMFLOAT4 diffuseCol = lightID[ID]->GetDiffuseColour();
			float* difcol[3]{ &diffuseCol.x,&diffuseCol.y, &diffuseCol.z };
			ImGui::ColorEdit3("L2 Diffuse Colour", *difcol);
			lightID[ID]->SetDiffuseColour(diffuseCol.x, diffuseCol.y, diffuseCol.z, 1.0f);

			XMFLOAT4 specularCol = lightID[ID]->GetSpecularColour();
			float* speccol[3]{ &specularCol.x,&specularCol.y, &specularCol.z };
			ImGui::ColorEdit3("L2 Specular Colour", *speccol);
			lightID[ID]->SetSpecularColour(specularCol.x, specularCol.y, specularCol.z, 1.0f);

			float pow = lightID[ID]->GetSpecularPower();
			float* powp = &pow;
			ImGui::DragFloat("L2 Specular Power", powp, 1.f, 1.0f, 1000.0f);
			lightID[ID]->SetSpecularPower(pow);
		}
	}
	
	ID = 2;

	//If there is Three lights
	if (Args->m_NumberOfLights >= 3)
	{
		if (ImGui::CollapsingHeader("Light Three Properties"))
		{
			//Add Seperation and Header
			ImGui::Separator();
			ImGui::Text("Light Three : L3");

			//Get and Set Position with a slider in ImGUI
			XMFLOAT3 pos = lightID[2]->GetPosition();
			float* light_pos[3] = { &pos.x, &pos.y, &pos.z };
			ImGui::DragFloat3("L3 Position", *light_pos, 0.2f, -10.0f, 10.0f);
			pos = XMFLOAT3(*light_pos[0], *light_pos[1], *light_pos[2]);
			lightID[2]->SetLightPosition(pos);

			XMFLOAT4 diffuseCol = lightID[ID]->GetDiffuseColour();
			float* difcol[3]{ &diffuseCol.x,&diffuseCol.y, &diffuseCol.z };
			ImGui::ColorEdit3("L3 Diffuse Colour", *difcol);
			lightID[ID]->SetDiffuseColour(diffuseCol.x, diffuseCol.y, diffuseCol.z, 1.0f);

			XMFLOAT4 specularCol = lightID[ID]->GetSpecularColour();
			float* speccol[3]{ &specularCol.x,&specularCol.y, &specularCol.z };
			ImGui::ColorEdit3("L3 Specular Colour", *speccol);
			lightID[ID]->SetSpecularColour(specularCol.x, specularCol.y, specularCol.z, 1.0f);

			float pow = lightID[ID]->GetSpecularPower();
			float* powp = &pow;
			ImGui::DragFloat("L3 Specular Power", powp, 1.f, 1.0f, 1000.0f);
			lightID[ID]->SetSpecularPower(pow);
		}
	}
	
	ID = 3;

	//If there is Four lights
	if (Args->m_NumberOfLights >= 4)
	{
		if (ImGui::CollapsingHeader("Light Four Properties"))
		{
			//Add Seperation and Header
			ImGui::Separator();
			ImGui::Text("Light Four : L4");

			//Get and Set Position with a slider in ImGUI
			XMFLOAT3 pos = lightID[3]->GetPosition();
			float* light_pos[3] = { &pos.x, &pos.y, &pos.z };
			ImGui::DragFloat3("L4 Position", *light_pos, 0.2f, -10.0f, 10.0f);
			pos = XMFLOAT3(*light_pos[0], *light_pos[1], *light_pos[2]);
			lightID[3]->SetLightPosition(pos);

			XMFLOAT4 diffuseCol = lightID[ID]->GetDiffuseColour();
			float* difcol[3]{ &diffuseCol.x,&diffuseCol.y, &diffuseCol.z };
			ImGui::ColorEdit3("L4 Diffuse Colour", *difcol);
			lightID[ID]->SetDiffuseColour(diffuseCol.x, diffuseCol.y, diffuseCol.z, 1.0f);

			XMFLOAT4 specularCol = lightID[ID]->GetSpecularColour();
			float* speccol[3]{ &specularCol.x,&specularCol.y, &specularCol.z };
			ImGui::ColorEdit3("L4 Specular Colour", *speccol);
			lightID[ID]->SetSpecularColour(specularCol.x, specularCol.y, specularCol.z, 1.0f);

			float pow = lightID[ID]->GetSpecularPower();
			float* powp = &pow;
			ImGui::DragFloat("L4 Specular Power", powp, 1.f, 1.0f, 1000.0f);
			lightID[ID]->SetSpecularPower(pow);
		}
	}
		
	



}

void App::ShowBasePostShadersGUI()
{
	ShaderArgs* Args = &m_ShaderArgs;
	//DropDown to Select Shader
	ImGui::Combo("Base Shader Select", &m_PostProcessBaseShader, "Texture Shader\0Dissolve Shader\0Light Shader\0Manipulation Shader\0");

	//Switch the GUI Based on what shader is selected (e.g. dissolve sliders will only show up when dissolve shader selected)
	switch (m_PostProcessBaseShader)
	{
	case kTextureShader:
		break;
	case kDissolveShader:
		ImGui::SliderFloat("Dissolve amount", &Args->m_DissolveAmount, 0.0f, 1.0f);
		ImGui::SliderFloat("Fringe size", &Args->m_DissolveFringeSize, 0.0f, 1.0f);
		break;
	case kLightShader:
		ShowLightGUI();
		break;
	case kManipulationShader:
		ImGui::SliderFloat("Height", &Args->m_Height, 0.0f, 2.0f);
		ImGui::SliderFloat("Speed", &Args->m_Speed, 1.0f, 10.0f);
		break;
	}
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
		case kPlane: 
		{
			m_GameObject->CreatePlaneObject(500);
			m_GameObject->SetPosition(XMFLOAT3(-250.f, -30.f, -20.f));
		}
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
	m_Direct3D->BeginScene(m_ClearColour.x, m_ClearColour.y, m_ClearColour.z, 1.0f);

	//// Generate the view matrix based on the camera's position.
	m_Camera->Update();

	////Reset Shader Flags
	m_ShaderArgs.m_IsDisplacementMap = 0; //flag in shader to load displacement map
	

	switch (m_ShaderNumber)
	{
	case kTextureShader:
		m_GameObject->Render(m_Direct3D, m_Camera, m_TextureShader, m_ShaderArgs);
		break;
	case kDissolveShader:
		m_GameObject->Render(m_Direct3D, m_Camera, m_DissolveShader, m_ShaderArgs);
		break;
	case kLightShader:
	{
		m_GameObject->Render(m_Direct3D, m_Camera, m_LightShader, m_ShaderArgs);

		if (m_ShowDebugPositions)
		{
			auto iter = m_ShaderArgs.m_Lights.begin();
			for (int i = 0; i < m_ShaderArgs.m_NumberOfLights; i++)
			{
				XMFLOAT3 lightPos = (*iter)->GetPosition();
				m_LightDebug[i]->SetPosition(lightPos);
				m_LightDebug[i]->Render(m_Direct3D, m_Camera, m_TextureShader, m_ShaderArgs);
				iter++;
			}
		}
		break;
	}
	case kManipulationShader:
	{
		m_GameObject->Render(m_Direct3D, m_Camera, m_ManipulationShader, m_ShaderArgs);
		break;
	}
	case kDisplacementMapShader:
	{
		m_ShaderArgs.m_IsDisplacementMap = 1; //flag in shader to load displacement map
		//m_GameObject->CreatePlaneObject();
		m_ModelNumber = kPlane;
		m_GameObject->Render(m_Direct3D, m_Camera, m_DisplacementMapShader, m_ShaderArgs);
		break;
	}
	case kPostProcessingScene:
	{
		RenderPostProcessingScene();
		break;
	}
	case kShadowShader:
	{
		RenderShadowScene();
	}
	default:
		break;
	}



	ImGui::Render();
	m_Direct3D->EndScene();

	m_PostProcessor->Reset();
	return true;
}

void App::RenderShadowScene()
{

	if (m_ShowDepthTexture)
	{
		m_PostProcessTexture = m_PostProcessor->RenderDepthToTexture(m_ShadowObjects, m_DepthShader, m_ShaderArgs, m_Camera, m_ClearColour);
		m_PostProcessor->RenderTextureToScene(m_PostProcessTexture, m_Camera, m_TextureShader, m_ShaderArgs);
	}
	else
	{
		Light* light = m_ShaderArgs.m_ShadowLight;

		m_PostProcessTexture = m_PostProcessor->RenderDepthToTexture(m_ShadowObjects, m_DepthShader, m_ShaderArgs, m_Camera, m_ClearColour, light, (RenderTargetFlag)m_ShadowQuality);
		m_ShaderArgs.m_DepthMap = m_PostProcessTexture->GetShaderResourceView();

		if (!m_ShowDepthFromLight) //Skip if we want to view the texture As Seen from the light
		{
			m_PostProcessTexture = m_PostProcessor->RenderDepthToTexture(m_ShadowObjects, m_ShadowShader, m_ShaderArgs, m_Camera, m_ClearColour);
		}
		m_PostProcessor->RenderTextureToScene(m_PostProcessTexture, m_Camera, m_TextureShader, m_ShaderArgs);

	}

}