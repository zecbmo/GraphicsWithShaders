#pragma once
// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "../DXFramework/baseapplication.h"
#include "D3D.h"
#include "../DXFramework/SphereMesh.h"
#include "../DXFramework/QuadMesh.h"
#include "../DXFramework/Light.h"
#include "LightShader_04.h"


class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input*);

	bool Frame();

protected:
	bool Render();

private:
	Light* light;
	Light* light2;
	SphereMesh* m_Mesh;
	QuadMesh* m_quad;
	LightShader* shader;


};

#endif