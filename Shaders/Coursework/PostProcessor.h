#pragma once

#include "../DXFramework/baseapplication.h"
#include "../DXFramework/GameObject.h"
#include "../DXFramework/RenderTexture.h"
#include "../DXFramework/OrthoMesh.h"
#include "D3D.h"
#include <list>
#include "TextureShader.h"

/*	The Post Processor
*	
*	This class will have extended functions that allow for easy creation of render targets
*	It will contain 3 main functions/with extended functions that encompase the same effect
*
*	1. Render a List of Game Objects to A texture based on a shader - (based on non-postprocessingshaders)
*	2. Take a RenderTexture and apply post processing to it
*	3. Render a rendertexture to screen
*
*/

enum RenderTargetFlag { kScreenSize, kHalfScreenSize, kDoubleScreenSize };

class PostProcessor
{
public:
	PostProcessor(D3D* device, int screenWidth, int screenHeight, float screenNear, float screenDepth);
	~PostProcessor();

	//First Pass Function - Renders Scene to texture - has flags for setting custom width and height- if not set will default to the screen width and height 
	RenderTexture* RenderGameObjectsToTexture(std::list<GameObject*> gameObjects, BaseShader * shader, ShaderArgs &shaderArgs, Camera* camera, XMFLOAT3 clearColour, RenderTargetFlag flag = kScreenSize);
	RenderTexture* RenderDepthToTexture(std::list<GameObject*> gameObjects, BaseShader * shader, ShaderArgs &shaderArgs, Camera* camera, XMFLOAT3 clearColour, Light* light = nullptr, RenderTargetFlag flag = kScreenSize);
	RenderTexture* PostProcessTexture(RenderTexture* RendTexture, BaseShader* postProcessShader, ShaderArgs &shaderArgs, Camera* camera, XMFLOAT3 clearColour, RenderTargetFlag flag = kScreenSize);
	void RenderTextureToScene(RenderTexture* renderTexture, Camera* camera, BaseShader* textureShader, ShaderArgs &shaderArgs);
	void Reset();
private:
	D3D* m_Direct3D;
	int m_ScreenWidth;
	int m_ScreenHeight;
	float m_ScreenNear;
	float m_ScreenDepth;
	OrthoMesh* m_OrthoMesh;


	RenderTexture* GetScreenSizeRenderTarget();
	RenderTexture*  GetHalfScreenSizeRenderTarget();
	RenderTexture* GetDoubleScreenSizeRenderTarget();


	//base render targets
	std::list<RenderTexture*> ScreenSizeRenderTargets;
	std::list<RenderTexture*> HalfScreenSizeRenderTargets;
	std::list<RenderTexture*> DoubleSizeRenderTargets;



};

