#pragma once
// Application.h
#ifndef _App_H
#define _App_H

// Includes
#include "../DXFramework/imgui.h"
#include "../DXFramework/imgui_impl_dx11.h"
#include "../DXFramework/baseapplication.h"
#include "D3D.h"
#include "../DXFramework/CubeMesh.h"
#include "../DXFramework/Light.h"
#include "LightShader.h"
#include "../DXFramework/OrthoMesh.h"
#include "../DXFramework/RenderTexture.h"
#include "../DXFramework/PlaneMesh.h"
#include "TextureShader.h"
#include "BlurShader.h"
#include "../DXFramework/MyTesselationMesh.h"
#include "TessellationShader.h"
#include "DissolveShader.h"
#include "../DXFramework/Texture.h"
#include "ShadowShader.h"
#include "DepthShader.h"
#include "../DXFramework/PointMesh.h"
#include "GeometryShader.h"

class App : public BaseApplication
{
public:

	App();
	~App();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input*);

	bool Frame();

protected:
	bool Render();
	void RenderToTexture();
	void RenderDissolve(float dissolveAmount,float fringeSize);
	void RenderBlurToTexture(RenderTexture* target, RenderTexture* from, BlurShader* b_shader);
	void SampleToDifferentTarget(RenderTexture* target, RenderTexture* from);
	//void UpSample(RenderTexture* target, RenderTexture* from);
	void RenderTesselation();

	void RenderDepthToTexture();
	void RenderShadowsFromDepthTexture(RenderTexture* rend);
	void RenderGeometryShaderScene();

	void RenderScene(RenderTexture* rend);

private:
	Light* light;
	CubeMesh* m_Mesh;
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
	RenderTexture* m_depth_texture;


	float width, height;

	//Tesselation
	MyTessellationMesh* tes_mesh;
	TessellationShader* tes_shader;
	float tess_value;

	//dissolve
	DissolveShader* dis_shader;
	Texture* dissolveMap;
	float dis_amount;
	float fringe;

	//depth and shadows
	DepthShader* depth_shader;
	ShadowShader* shadow_shader;
	PlaneMesh* m_plane_mesh;
	float time;


	//GeometryShader
	GeometryShader* geo_shader;
	PointMesh* point1;
	PointMesh* point2;
	PointMesh* point3;
	
};

#endif