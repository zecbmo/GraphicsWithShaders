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
	Light* m_Light;

	//GameObjects in Scene
	GameObject* m_GameObject;

	//Shaders 
	TextureShader* m_TextureShader;
	DissolveShader* m_DissolveShader;

	//ImGUI
	int m_ModelNumber;
	int m_PrevModelNumber;
	int m_ShaderNumber;

	//Reload the Shape
	void ReloadShape();
};

#endif