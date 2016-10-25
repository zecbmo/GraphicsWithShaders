#pragma once
// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "../DXFramework/imgui.h"
#include "../DXFramework/imgui_impl_dx11.h"
#include "../DXFramework/baseapplication.h"
#include "D3D.h"
#include "../DXFramework/SphereMesh.h"
#include "../DXFramework/Light.h"
#include "LightShader.h"
#include "../DXFramework/OrthoMesh.h"
#include "../DXFramework/RenderTexture.h"
#include "TextureShader.h"
#include "BlurShader.h"



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
	void RenderBlurToTexture(RenderTexture* target, RenderTexture* from, BlurShader* b_shader);
	void SampleToDifferentTarget(RenderTexture* target, RenderTexture* from);
	//void UpSample(RenderTexture* target, RenderTexture* from);


	void RenderScene(RenderTexture* rend);

private:
	Light* light;
	SphereMesh* m_Mesh;
	LightShader* shader;

	RenderTexture* m_render_texture;
	OrthoMesh* m_ortho_mesh;
	
	TextureShader* text_shader;

	BlurShader* box_blur_shader;
	BlurShader* vert_blur_shader;
	BlurShader* hor_blur_shader;

	RenderTexture* m_box_render_texture;
	RenderTexture* m_vert_render_texture;
	RenderTexture* m_hor_render_texture;
	RenderTexture* m_downsample_render_texture;
	RenderTexture* m_upsample_render_texture;


	float width, height;

};

#endif