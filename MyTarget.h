#pragma once

#include <vgl.h>
#include <vec.h>
#include <mat.h>
#include "MyCube.h"

class MyTarget
{
public:
	MyTarget(MyCube * in);
	MyCube * pCube;

	float radius;
	vec3 offset;
	vec3 currentPos;

	bool bRandom;

	void Draw(GLuint program, mat4 uMat, float time);
	vec3 GetPosition(float time);
	
	void toggleRandom() {bRandom = !bRandom;};
	
};

MyTarget::MyTarget(MyCube * in)
{
	pCube = in;
	radius = 0.6;
	offset = vec3(0.0,0.2,0.0);
	currentPos = vec3(0,0,0);
	bRandom = false;
}

vec3 MyTarget::GetPosition(float time)
{
	static float theta = 0;

	if(bRandom == false)
		theta = time*3.141592;
	else
	{
		static float lastTime = 0;
		if(time-lastTime>2.0f)
		{
			lastTime = time;
			theta = (rand()%1000)/1000.0f*3.141592*2;
		}
	}

	float x = radius*cos(theta);
	float y = radius*sin(theta);
	currentPos = vec3(x,y,0)+offset;

	return currentPos;
}

void MyTarget::Draw(GLuint program, mat4 CTM, float time)
{
	vec3 p = GetPosition(time);
	mat4 T = Translate(p.x, p.y, 0);
	mat4 M = Scale(0.08,0.08,0.08);
	
	GLuint uMat = glGetUniformLocation(program, "uMat");
	glUniformMatrix4fv(uMat, 1, true, CTM*T*M);
	pCube->Draw(program);			
}