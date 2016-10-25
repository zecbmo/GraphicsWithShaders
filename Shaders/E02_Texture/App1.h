// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "../DXFramework/baseapplication.h"
#include "D3D.h"
#include "../DXFramework/quadmesh.h"
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

private:
	TextureShader* m_TextureShader;
	QuadMesh* m_Mesh;
	QuadMesh* my_Mesh;

	float rot_num;
	
};

#endif