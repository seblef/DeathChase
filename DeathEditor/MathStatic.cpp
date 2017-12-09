
#include "../DeathEngine/Core/Vector.h"

namespace Core
{

float Matrix4::TCos[360];
float Matrix4::TSin[360];

Vector2 Vector2::NullVector(0.0f,0.0f);
Vector2 Vector2::XAxisVector(1.0f,0.0f);
Vector2 Vector2::YAxisVector(0.0f,1.0f);

Vector2i Vector2i::NullVector(0,0);
Vector2i Vector2i::XAxisVector(1,0);
Vector2i Vector2i::YAxisVector(0,1);

Vector3 Vector3::NullVector(0.0f,0.0f,0.0f);
Vector3 Vector3::XAxisVector(1.0f,0.0f,0.0f);
Vector3 Vector3::YAxisVector(0.0f,1.0f,0.0f);
Vector3 Vector3::ZAxisVector(0.0f,0.0f,1.0f);

Matrix4 Matrix4::Identity;
Matrix4 Matrix4::SphereMap;

void Matrix4::InitTrigo()
{
	int i;
	for(i=0;i<360;i++)
	{
		TCos[i]=cosf(i*3.141592f/180.0f);
		TSin[i]=sinf(i*3.141592f/180.0f);
	}

	Identity.m[0][0]=1.0f; Identity.m[0][1]=0.0f; Identity.m[0][2]=0.0f; Identity.m[0][3]=0.0f;
	Identity.m[1][0]=0.0f; Identity.m[1][1]=1.0f; Identity.m[1][2]=0.0f; Identity.m[1][3]=0.0f;
	Identity.m[2][0]=0.0f; Identity.m[2][1]=0.0f; Identity.m[2][2]=1.0f; Identity.m[2][3]=0.0f;
	Identity.m[3][0]=0.0f; Identity.m[3][1]=0.0f; Identity.m[3][2]=0.0f; Identity.m[3][3]=1.0f;

	SphereMap.m[0][0]=0.5f; SphereMap.m[0][1]= 0.0f; SphereMap.m[0][2]=0.0f; SphereMap.m[0][3]=0.0f;
	SphereMap.m[1][0]=0.0f; SphereMap.m[1][1]=-0.5f; SphereMap.m[1][2]=0.0f; SphereMap.m[1][3]=0.0f;
	SphereMap.m[2][0]=0.0f; SphereMap.m[2][1]= 0.0f; SphereMap.m[2][2]=1.0f; SphereMap.m[2][3]=0.0f;
	SphereMap.m[3][0]=0.5f; SphereMap.m[3][1]= 0.5f; SphereMap.m[3][2]=0.0f; SphereMap.m[3][3]=1.0f;

}

}