#pragma once

#include <d3d11.h>
#include <directxmath.h>
#include "texture.h"
#include "Light.h"
#include <list>

/** The Shader Argument Struct
*
*	This Struct contains all the infocrmation that a shader class may need.
*	It is passed to the SetShaderParameter Function within all Shader classes and they can 
*	pick and use what information is needed.
*	This will information will be stored in the game object class.
*
*	Most of the shader properties here will be controllable by the user during runtime
*
*	Updated Changed So all values are public to give shaders access to the device context etc
*/

struct ShaderArgs
{
public:
	//Allows the GameObject to set private members (to set these values automatically without anyone else being able to tamper)
	friend class GameObject;
	ShaderArgs() : 
		m_DeviceContext(nullptr), m_DissolveMap(nullptr),
		m_Texture(nullptr), m_DepthMap(nullptr),  
		m_WorldMatrix(XMMATRIX()), m_ViewMatrix(XMMATRIX()), 
		m_ProjectionMatrix(XMMATRIX()), m_IsPointLight(true), m_TextureOn(true), m_IsDisplacementMap(0),	
		m_TessFactor(1.0f), m_ExplosionAmount(0){}


	ID3D11ShaderResourceView* m_DepthMap;

	//Light Shader
	std::list<Light*> m_Lights;
	bool m_IsPointLight; // true by default
	bool m_TextureOn; //texture controls
	int m_NumberOfLights;
	XMFLOAT3 m_CameraPos;
	


	//Dissolve Shader
	Texture* m_DissolveMap;
	float m_DissolveAmount;
	float m_DissolveFringeSize;

	//Vertex Manipulation Shader
	float m_Speed;
	float m_Height;
	float m_Time;
	float m_IsDisplacementMap;
	Texture* m_DisplacementMap;
	

	//Defaults for all Shaders
	ID3D11DeviceContext* m_DeviceContext;
	XMMATRIX m_WorldMatrix;
	XMMATRIX m_ViewMatrix;
	XMMATRIX m_ProjectionMatrix;
	ID3D11ShaderResourceView* m_Texture;

	//Blur Shaders
	int m_ScreenWidth;
	int m_ScreenHeight;

	//Shadows
	float m_ShadowBias;
	Light* m_ShadowLight;

	//Tessilation
	float m_TessFactor;

	//GeoShader
	XMFLOAT3 m_CameraUpVec;
	float m_ExplosionAmount;

	//Two Textures for passing to the double texture shader
	ID3D11ShaderResourceView* m_LeftTexture;
	ID3D11ShaderResourceView* m_RightTexture;

};
