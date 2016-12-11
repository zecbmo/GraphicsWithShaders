#pragma once

#include "BaseMesh.h"
#include "BaseShader.h"
#include "ShaderArgs.h"
#include "camera.h"
#include "D3D.h"

/** The GameObject Class
*
*	This Class will Represent an object within the scene.
*	It will be basic in that it only inlcudes a mesh and a Transform.
*	There are a choice of initalisation functions to add a mesh type to the GameObject. eg a cube/sphere
*	Shader Arguments will be used to control the shader - based on shader being used they can be setup however needed
*	Safety checks for shader arguments will be set in each individual shader and throw an error if the needed args haven't been set
*	On Render Will render the shape to the given transforms with a selected shader passed as a pointer
*/

class GameObject
{
public:
	GameObject(ID3D11Device* device, ID3D11DeviceContext* deviceContext, WCHAR* textureFilename);
	~GameObject();

	//Initialisation Options for different Mesh Types
	void CreateSphereObject(int resolution = 20);
	void CreateCubeObject(int resolution = 20);
	void CreatePlaneObject(int resolution = 100);
	void LoadModel(WCHAR* filename);
	void CreateTesselationMesh();
	void CreateOrthoMesh(int width, int height, int xPosition = 0, int yPosition = 0);
	void CreateQuadMesh();
	void CreatePointMesh();

	//In future it may be better to overload the Render Function for certain Shaders (e.g. Tesselation/Geomotry)
	virtual void Render(D3D* Direct3D, Camera * Camera, BaseShader* Shader, ShaderArgs& shaderArgs);
	virtual void RenderFromLightSource(D3D* Direct3D, Light * light, BaseShader* Shader, ShaderArgs& shaderArgs);
	void RenderSpinningObject(D3D* Direct3D, Camera * Camera, BaseShader* Shader, ShaderArgs& shaderArgs, float dt, float RotSpeed, float ScaleSpeed);
	void GameObject::RenderTargetFunction(D3D* Direct3D, BaseShader* Shader, ShaderArgs& shaderArgs);
	
	//Able to edit the public member data when called e.g GameObject->ModifyShaderArgs()->Light = m_Light;
	//The items that a shader needs must be set with this befoe the render function is called

	inline XMFLOAT3 GetPosition() { return m_Position; };
	inline void SetPosition(XMFLOAT3 Position) { m_Position = Position; };
	inline void SetRotation(XMFLOAT3 Rotation) { m_Rotation = Rotation; };
	inline void SetScale(XMFLOAT3 Scale) { m_Scale = Scale; };

	//To be used to allow ImGUI controll over positions 
	inline XMFLOAT3* GetPositionPointer() { return &m_Position; };
	inline XMFLOAT3* GetRotationPointer() { return &m_Rotation; };
	inline XMFLOAT3* GetScalePointer() { return &m_Scale; };

	//Simple Command to allow texture changes in run time
	inline void ChangeTexture(WCHAR* textureFilename) { m_TextureFilename = textureFilename; };

private:
	//The base mesh is a pointer to hold the specific (child)mesh created 
	BaseMesh* m_Mesh;
	
	//The Transforms for the Shape
	XMFLOAT3 m_Position;
	XMFLOAT3 m_Rotation;	
	XMFLOAT3 m_Scale;
	
	//DirectX devie/context
	ID3D11Device* m_Device;
	ID3D11DeviceContext* m_DeviceContext;
	WCHAR* m_TextureFilename;

	float m_ScaleHelper;
};

