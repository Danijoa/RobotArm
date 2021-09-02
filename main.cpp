#include <vgl.h>
#include <InitShader.h>
#include <mat.h>
#include "MyCube.h"
#include "MyPyramid.h"
#include "MyTarget.h"

MyCube cube;
MyPyramid pyramid;
MyTarget target(&cube);

GLuint program;
GLuint uMat;

mat4 CTM;

bool bPlay = false;
bool bChasingTarget= false;
bool bDrawTarget = false;

float ang1 = 360;
float ang2 = 360;
float ang3 = 360;

//Hand 위치
float x;
float y;
float z;

void myInit()
{
	cube.Init();
	pyramid.Init();	

	program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);
}

float time = 0;

void drawRobotArm(float ang1, float ang2, float ang3)
{
	mat4 temp = CTM;
	
	// BASE
	mat4 M(1.0);

	CTM = Translate(0, -0.4, 0) * RotateY(time*50);
	M = Translate(0, 0, 0.075)*Scale(0.3, 0.3, 0.05);
	glUniformMatrix4fv(uMat, 1, true, CTM*M);
	pyramid.Draw(program);

	M = Translate(0, 0, -0.075)*Scale(0.3, 0.3, 0.05);
	glUniformMatrix4fv(uMat, 1, true, CTM*M);
	pyramid.Draw(program);

	// 아래 나..사..
	M = Translate(0, 0, 0.12) * Scale(0.04, 0.04, 0.03);
	glUniformMatrix4fv(uMat, 1, true, CTM*M);
	cube.Draw(program);
	M = Translate(0, 0, -0.12) * Scale(0.04, 0.04, 0.03);
	glUniformMatrix4fv(uMat, 1, true, CTM*M);
	cube.Draw(program);

	// Upper Arm
	CTM *= RotateZ(ang1);
	M = Translate(0, 0.2, 0) *  Scale(0.1, 0.5, 0.1);
	glUniformMatrix4fv(uMat, 1, true, CTM*M);
	cube.Draw(program);

	// Lower Arm
	CTM *= Translate(0, 0.4, 0) * RotateZ(ang2);
	M = Translate(0, 0.2, 0.075) * Scale(0.1, 0.5, 0.05);
	glUniformMatrix4fv(uMat, 1, true, CTM*M);
	cube.Draw(program);

	M = Translate(0, 0.2, -0.075) * Scale(0.1, 0.5, 0.05);
	glUniformMatrix4fv(uMat, 1, true, CTM*M);
	cube.Draw(program);

	// 중간
	M = Translate(0, 0.02, 0.12) * Scale(0.04, 0.04, 0.03);
	glUniformMatrix4fv(uMat, 1, true, CTM*M);
	cube.Draw(program);
	M = Translate(0, 0.02, -0.12) * Scale(0.04, 0.04, 0.03);
	glUniformMatrix4fv(uMat, 1, true, CTM*M);
	cube.Draw(program);
	
	// Hand
	CTM *= Translate(0, 0.4, 0) * RotateZ(ang3);
	M = Translate(0, 0.025, 0) * Scale(0.4, 0.15, 0.1);
	glUniformMatrix4fv(uMat, 1, true, CTM*M);
	cube.Draw(program);

	// 위
	M = Translate(0, 0.02, 0.12) * Scale(0.04, 0.04, 0.03);
	glUniformMatrix4fv(uMat, 1, true, CTM*M);
	cube.Draw(program);
	M = Translate(0, 0.02, -0.12) * Scale(0.04, 0.04, 0.03);
	glUniformMatrix4fv(uMat, 1, true, CTM*M);
	cube.Draw(program);
	
	// Hand 끝에 Cube 달기
	M = Translate(-0.2, 0.025, 0) * Scale(0.04, 0.04, 0.03);
	glUniformMatrix4fv(uMat, 1, true, CTM*M);
	cube.Draw(program);

	// Hand 끝부분 위치 구하기
	M = Translate(-0.2, 0.025, 0);
	mat4 robo = CTM*M; 
	x = robo[0][3]; 
	y = robo[1][3];
	z = robo[2][3];

	CTM = temp;
}

float check = 0.5;
int num=0;
void computeAngle()
{
	// target 위치 받아오기
	vec3 tar = target.GetPosition(time);

	// Hand 끝부분 위치 구하기
	vec4 p = vec4(x, y, z, 1);

	// target과 x축 사이 각 구하기 
	float k = atan2(tar.y, tar.x)*180/3.141592;
	if( k >= 0 && k<90)
		k = 270 + k;
	else if( k >= 90 && k < 180)
		k = 270 + k;
	else if( k < 0 && k >= -90)
		k = -90 + k;
	else if( k < -90 && k >= -180)
		k = -90 + k;

	// ang1을 traget과 수평으로 놓기
	ang1 = k;

	// ang2을 음수만 나올수 있도록 조정하기
	float angle2 = 60 * sin((1/time)*3.141592-3.141592);
	float angle3 =  30 * sin(time*3.141592);
	mat4 MM1(1.0);
	mat4 MM2(1.0);
	mat4 mm = Translate(0, -0.4, 0)*RotateZ(ang1)*Translate(0, 0.4, 0)*RotateZ(angle2)*Translate(0, 0.4, 0) * RotateZ(angle3);
	MM2 = mm * MM1;
	// 각도에 따라 변화된 p
	vec4 pp = MM2 * p;

	// target과 hand 사이 거리
	float dis = sqrt((tar.x-pp.x)*(tar.x-pp.x)+(tar.y-pp.y)*(tar.y-pp.y));
	if(check <= dis && num==0)
	{
		num=1;
		check=dis;
		if(pp.x < tar.x && pp.x < 0)
			ang2 = angle2 - 60;
		else
			ang2 = angle2;

		if(pp.y >tar.y && pp.y>0)
			ang3 = 90 + ang2;
		else
			ang3 = -ang2;
	}
	else
	{
		check=dis;

		if(pp.x < tar.x && pp.x < 0)
			ang2 = angle2 - 60;
		else
			ang2 = angle2;

		if(pp.y >tar.y && pp.y>0)
			ang3 = 90 + ang2;
		else
			ang3 = -ang2;
	}
}

void myDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	GLuint uColor = glGetUniformLocation(program, "uColor");
	glUniform4f(uColor, -1,-1,-1,-1);

	uMat = glGetUniformLocation(program, "uMat");
	drawRobotArm(ang1, ang2, ang3);	

	glUniform4f(uColor, 1,0,0,1);
	CTM = Translate(0, -0.4, 0) * RotateY(time*50);
	if(bDrawTarget == true)
		target.Draw(program, CTM, time);
	
	glutSwapBuffers();
}

void myIdle()
{
	if(bPlay)
	{
		time += 1/60.0f;
		Sleep(1/60.0f*1000);

		if(bChasingTarget == false)
		{
			ang1 = 45 * sin(time*3.141592);
			ang2 = 60 * sin(time*2*3.141592);
			ang3 = 30 * sin(time*3.141592);
		}
		else
			computeAngle();

		glutPostRedisplay();
	}
}

void myKeyboard(unsigned char c, int x, int y)
{

	switch(c)
	{
	case '1':
		bChasingTarget = !bChasingTarget;
		break;
	case '2':
		bDrawTarget = !bDrawTarget;
		break;
	case '3':
		target.toggleRandom();
		break;
	case ' ':
		bPlay = !bPlay;
		break;
	default:
		break;
	}
}

int main(int argc, char ** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
	glutInitWindowSize(500,500);
	glutCreateWindow("Simple Robot Arm");

	glewExperimental = true;
	glewInit();

	myInit();
	glutDisplayFunc(myDisplay);
	glutKeyboardFunc(myKeyboard);
	glutIdleFunc(myIdle);

	glutMainLoop();
	return 0;
}