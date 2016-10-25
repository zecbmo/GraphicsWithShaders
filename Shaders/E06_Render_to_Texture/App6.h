#pragma once
// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "../DXFramework/baseapplication.h"
#include "D3D.h"
#include "../DXFramework/SphereMesh.h"
#include "../DXFramework/Light.h"
#include "LightShader.h"
#include "../DXFramework/OrthoMesh.h"
#include "../DXFramework/RenderTexture.h"
#include "TextureShader.h"


class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input*);

	bool Frame();

protected:
	bool Render();
	void RenderToTexture();
	void RenderScene();

private:
	Light* light;
	SphereMesh* m_Mesh;
	LightShader* shader;

	RenderTexture* m_render_texture;
	OrthoMesh* m_ortho_mesh;
	
	TextureShader* text_shader;

};

#endif