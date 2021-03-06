#pragma once
// Application.h
#ifndef _App_H
#define _App_H

// Includes
#include "../DXFramework/imgui.h"
#include "../DXFramework/imgui_impl_dx11.h"
#include "../DXFramework/baseapplication.h"
#include "../DXFramework/GameObject.h"
#include "D3D.h"
#include "LightShader.h"
#include "TextureShader.h"
#include "BlurShader.h"
#include "TessellationShader.h"
#include "DissolveShader.h"
#include "ShadowShader.h"
#include "DepthShader.h"
#include "GeometryShader.h"
#include "ManipulationShader.h"
#include "PostProcessor.h"
#include "DoubleTextureShader.h"

#define EYE_SEPARATION 0.3f

enum TextureType {kDefault, kCheckerBoard, kBunny, kDissolveMap, kGradientMap, kPatrickStarT, kSpongeBobT, kGaryT};
enum PostProcessType {kNone, kBoxBlur, kGausianBlur, kAnalglyph };

/* The Application class
*
*	This is the main scene within the program
*	It will update and render Objects/meshes/control within the program
*
*/


class App : public BaseApplication
{
public:

	App();
	~App();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input*);
	void InitShaders(HWND hwnd);
	bool Render();

	bool Frame();

protected:
	

private:
	//Light Settings
	void SetupLights();
	void ResetLights();
	
	//GameObjects in Scene
	GameObject* m_GameObject;
	GameObject* m_LightDebug[4];
	GameObject* m_ShadowPlane;
	GameObject* m_TessellatedCube;
	GameObject* m_Particle;
	void ResetGameObject();


	//Post Processing Scene
	void SetUpPostProcessingScene();
	void CreatePathHelper(XMFLOAT3 pos);
	void CreateWallHelper(XMFLOAT3 pos);
	void CreateBuildingHelper(XMFLOAT3 pos, XMFLOAT3 scale, WCHAR* texture);
	void RenderPostProcessingScene();
	std::list<GameObject*> m_SceneObjects;

	//Post processing
	PostProcessor* m_PostProcessor;
	RenderTexture* m_PostProcessTexture;
	RenderTexture* m_LeftRenderTexture;
	RenderTexture* m_RightRenderTexture;


	//Shaders 
	TextureShader* m_TextureShader;
	DissolveShader* m_DissolveShader;
	LightShader* m_LightShader;
	ManipulationShader* m_ManipulationShader;
	ManipulationShader* m_DisplacementMapShader;
	BlurShader* m_BoxBlurShader;
	BlurShader* m_HorizontalBlurShader;
	BlurShader* m_VerticalBlurShader;
	ShadowShader* m_ShadowShader;
	DepthShader* m_DepthShader;
	TessellationShader* m_TessellationShader;
	GeometryShader* m_GeoShader;
	GeometryShader* m_ExplosionShader;
	DoubleTextureShader* m_DoubleTextureShader;
	//Shader Arguments
	ShaderArgs m_ShaderArgs;
	void ReloadShaderSettings();

	//Shadow Game Object List for depth buffer
	std::list<GameObject*> m_ShadowObjects;
	void RenderShadowScene();

	//Tessellation Scene
	void RenderTessellationScene();

	//ImGUI will give user control over shapes and shaders in runtime
	void CreateGUIWindow();
	void ModelTransFormGUI();
	void ShowLightGUI();
	void ShowBasePostShadersGUI();
	int m_ModelNumber;
	int m_PrevModelNumber;
	int m_ShaderNumber;
	int m_PrevShaderNumber;
	int m_TextureNumber;
	int m_PrevTextureNumber;
	bool m_ShowDebugPositions;
	int m_PostProcessShader;
	int m_PostProcessBaseShader;
	bool m_ShowDepthTexture;
	bool m_ShowDepthFromLight;
	int m_ShadowQuality;


	//Reload the Shape
	void ReloadShape();

	//Camera Reset
	void ResetCamera();

	//Clear Colour
	XMFLOAT3 m_ClearColour;
};

#endif