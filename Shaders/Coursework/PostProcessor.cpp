#include "PostProcessor.h"



PostProcessor::PostProcessor(D3D* direct3d, int screenWidth, int screenHeight, float screenNear, float screenDepth)
{
	m_Direct3D = direct3d;
	m_ScreenWidth = screenWidth;
	m_ScreenHeight = screenHeight;
	m_ScreenNear = screenNear;
	m_ScreenDepth = screenDepth;

	m_OrthoMesh = new OrthoMesh(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, 0, 0);


	for (int i = 0; i < 10; i++)
	{
		RenderTexture* ScreenSize = new RenderTexture(m_Direct3D->GetDevice(), screenWidth, screenHeight, m_ScreenNear, m_ScreenDepth);
		RenderTexture* HalfScreenSize= new RenderTexture(m_Direct3D->GetDevice(), screenWidth / 2, screenHeight / 2, m_ScreenNear, m_ScreenDepth);
		RenderTexture* DoubleScreenSize = new RenderTexture(m_Direct3D->GetDevice(), screenWidth*2, screenHeight*2, m_ScreenNear, m_ScreenDepth);


		ScreenSizeRenderTargets.push_back(ScreenSize);
		HalfScreenSizeRenderTargets.push_back(HalfScreenSize);
		DoubleSizeRenderTargets.push_back(DoubleScreenSize);

	}

}


PostProcessor::~PostProcessor()
{
	if (m_Direct3D)
	{
		delete m_Direct3D;
		m_Direct3D = nullptr;
	}
}

RenderTexture* PostProcessor::RenderGameObjectsToTexture(std::list<GameObject*> gameObjects, BaseShader * shader, ShaderArgs &shaderArgs, Camera* camera, XMFLOAT3 clearColour, RenderTargetFlag flag)
{
	RenderTexture* RendTexture = nullptr;
	switch (flag)
	{
	case kScreenSize:
		RendTexture = GetScreenSizeRenderTarget();;
		break;
	case kHalfScreenSize:
		RendTexture = GetHalfScreenSizeRenderTarget();
		break;
	case kDoubleScreenSize:
		RendTexture = GetDoubleScreenSizeRenderTarget();
		break;
	default:
		break;
	}

	// Set the render target to be the render to texture.
	RendTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// Clear the render to texture.
	RendTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), clearColour.x, clearColour.y, clearColour.z, 1.0f);

	//loop for all game objects
	for (auto iter : gameObjects)
	{
		//Render The Game Object
		iter->Render(m_Direct3D, camera, shader, shaderArgs);
	}

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_Direct3D->SetBackBufferRenderTarget();


	return RendTexture;
}

RenderTexture * PostProcessor::RenderDepthToTexture(std::list<GameObject*> gameObjects, BaseShader * shader, ShaderArgs & shaderArgs, Camera * camera, XMFLOAT3 clearColour, Light * light, RenderTargetFlag flag)
{
	RenderTexture* RendTexture = nullptr;
	switch (flag)
	{
	case kScreenSize:
		RendTexture = GetScreenSizeRenderTarget();;
		break;
	case kHalfScreenSize:
		RendTexture = GetHalfScreenSizeRenderTarget();
		break;
	case kDoubleScreenSize:
		RendTexture = GetDoubleScreenSizeRenderTarget();
		break;
	default:
		break;
	}

	// Set the render target to be the render to texture.
	RendTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// Clear the render to texture.
	RendTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), clearColour.x, clearColour.y, clearColour.z, 1.0f);

	//loop for all game objects
	for (auto iter : gameObjects)
	{
		//Render The Game Object
		if (light)
		{
			iter->RenderFromLightSource(m_Direct3D, light, shader, shaderArgs);
		}
		else
		{
			iter->Render(m_Direct3D, camera, shader, shaderArgs);
		}

		
	}

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_Direct3D->SetBackBufferRenderTarget();


	return RendTexture;
}

RenderTexture * PostProcessor::PostProcessTexture(RenderTexture* Texture, BaseShader* postProcessShader, ShaderArgs &shaderArgs, Camera* camera, XMFLOAT3 clearColour, RenderTargetFlag flag)
{

	RenderTexture* RendTexture = nullptr;
	switch (flag)
	{
	case kScreenSize:
		RendTexture = GetScreenSizeRenderTarget();;
		break;
	case kHalfScreenSize:
		RendTexture = GetHalfScreenSizeRenderTarget();
		break;
	case kDoubleScreenSize:
		RendTexture = GetDoubleScreenSizeRenderTarget();
		break;
	default:
		break;
	}

	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	// Set the render target to be the render to texture.
	RendTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// Clear the render to texture.
	RendTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 1.0f, 0.0f, 1.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	camera->Update();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	camera->GetBaseViewMatrix(viewMatrix);
	m_Direct3D->GetOrthoMatrix(projectionMatrix);

	//turn off z depth and set ortho view
	m_Direct3D->TurnZBufferOff();

	//update the shader argumenets
	shaderArgs.m_WorldMatrix = worldMatrix;
	shaderArgs.m_ViewMatrix = viewMatrix;
	shaderArgs.m_ProjectionMatrix = projectionMatrix;
	shaderArgs.m_Texture = Texture->GetShaderResourceView();

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_OrthoMesh->SendData(m_Direct3D->GetDeviceContext());

	postProcessShader->SetShaderParameters(shaderArgs);
	// Render object (combination of mesh geometry and shader process
	postProcessShader->Render(m_Direct3D->GetDeviceContext(), m_OrthoMesh->GetIndexCount());

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_Direct3D->SetBackBufferRenderTarget();

	//turn off z depth and set ortho view
	m_Direct3D->TurnZBufferOn();


	return RendTexture;
}

void PostProcessor::RenderTextureToScene(RenderTexture* renderTexture, Camera* camera, BaseShader* textureShader, ShaderArgs &shaderArgs)
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	// Clear the scene. (default blue colour)
	m_Direct3D->BeginScene(0.39f, 0.58f, 0.92f, 1.0f);

	// Generate the view matrix based on the camera's position.
	camera->Update();

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);

	//turn off z depth and set ortho view
	m_Direct3D->TurnZBufferOff();

	m_Direct3D->GetOrthoMatrix(orthoMatrix);// ortho matrix for 2D rendering
	camera->GetBaseViewMatrix(baseViewMatrix);

	//update the shader argumenets
	shaderArgs.m_WorldMatrix = worldMatrix;
	shaderArgs.m_ViewMatrix = baseViewMatrix;
	shaderArgs.m_ProjectionMatrix = orthoMatrix;
	shaderArgs.m_Texture = renderTexture->GetShaderResourceView();

	//render final ortho mesh/texture

	m_OrthoMesh->SendData(m_Direct3D->GetDeviceContext());
	textureShader->SetShaderParameters(shaderArgs);
	textureShader->Render(m_Direct3D->GetDeviceContext(), m_OrthoMesh->GetIndexCount());

	m_Direct3D->TurnZBufferOn();

}

void PostProcessor::Reset()
{
	for (auto iter : ScreenSizeRenderTargets)
	{
		iter->Reset();
	}

	for (auto iter : HalfScreenSizeRenderTargets)
	{
		iter->Reset();
	}
	for (auto iter : DoubleSizeRenderTargets)
	{
		iter->Reset();
	}
}

RenderTexture * PostProcessor::GetScreenSizeRenderTarget()
{
	RenderTexture* returnText = nullptr;

	for (auto iter : ScreenSizeRenderTargets)
	{
		if (!iter->GetHasBeenUsed())
		{
			iter->SetHasBeenUsed();
			returnText = iter;
			break;
		}
	}

	if (!returnText)
	{
		error::ErrorMessage("Not Enought Screen Size Render Targets for post processing");
	}

	return returnText;
}

RenderTexture * PostProcessor::GetHalfScreenSizeRenderTarget()
{
	RenderTexture* returnText = nullptr;

	for (auto iter : HalfScreenSizeRenderTargets)
	{
		if (!iter->GetHasBeenUsed())
		{
			iter->SetHasBeenUsed();
			returnText = iter;
			break;
		}
	}

	if (!returnText)
	{
		error::ErrorMessage("Not Enought Half Screen Size Render Targets for post processing");
	}

	return returnText;
}

RenderTexture * PostProcessor::GetDoubleScreenSizeRenderTarget()
{
	RenderTexture* returnText = nullptr;

	for (auto iter : DoubleSizeRenderTargets)
	{
		if (!iter->GetHasBeenUsed())
		{
			iter->SetHasBeenUsed();
			returnText = iter;
			break;
		}
	}

	if (!returnText)
	{
		error::ErrorMessage("Not Enought Half Screen Size Render Targets for post processing");
	}

	return returnText;
}
