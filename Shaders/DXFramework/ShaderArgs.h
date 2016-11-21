#pragma once

#include <d3d11.h>
#include <directxmath.h>
#include "texture.h"
#include "Light.h"

/** The Shader Argument Struct
*
*	This Struct contains all the infocrmation that a shader class may need.
*	It is passed to the SetShaderParameter Function within all Shader classes and they can 
*	pick and use what information is needed.
*	This will information will be stored in the game object class.
*	The Member Data within the Public Section can be edited. (These may or may not be used by a shader)
*	The Member Data within the Private Section will be set automatically within the game object.
*/

struct ShaderArgs
{
public:
	//Allows the GameObject to set private members (to set these values automatically without anyone else being able to tamper)
	friend class GameObject;
	ShaderArgs() : m_DeviceContext(nullptr), m_Texture(nullptr), m_DepthMap(nullptr), m_Light(nullptr), m_WorldMatrix(XMMATRIX()), m_ViewMatrix(XMMATRIX()), m_ProjectionMatrix(XMMATRIX()) {}
	ID3D11ShaderResourceView* m_DepthMap;
	Light* m_Light;

private:
	ID3D11DeviceContext* m_DeviceContext;
	XMMATRIX &m_WorldMatrix;
	XMMATRIX &m_ViewMatrix;
	XMMATRIX &m_ProjectionMatrix;
	ID3D11ShaderResourceView* m_Texture;
};
