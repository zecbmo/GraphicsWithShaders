#pragma once
// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "../DXFramework/baseapplication.h"
#include "D3D.h"
#include "../DXFramework/CubeMesh.h"
#include "../DXFramework/PlaneMesh.h"
#include "../DXFramework/Light.h"
#include "LightShader5.h"


class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input*);

	bool Frame();
	void HandleInput(float);

protected:
	bool Render();

private:
	Light* light;
	CubeMesh* m_Mesh;
	LightShader* shader;
	float time;
	float height;


};

#endif