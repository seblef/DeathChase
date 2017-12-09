
#ifndef _SHADERSDEFS_H_
#define _SHADERSDEFS_H_


//			Defs communes

#define MAT_VIEWPROJ			register(c0)
#define MAT_WORLD				register(c4)
#define MAT_INVWORLD			register(c8)
#define MAT_ROTWORLD			register(c12)

#define COLOR_AMBIENT			register(c16)
#define COLOR_EMISSIVE			register(c17)
#define COLOR_SPECULAR			register(c18)

#define SHIELD_CAMPOS			register(c19)		// x,y,z et dwVisDist
#define HEIGHTMAP_INFO			register(c20)		// size, 1.0f / size et scale

#define COMMON_TIME				register(c88)

#define LIGHT0_POSITION			register(c89)
#define LIGHT0_COLOR			register(c90)
#define LIGHT1_POSITION			register(c91)
#define LIGHT1_COLOR			register(c92)
#define LIGHT_AMBIENT_COLOR		register(c93)
#define LIGHT_SUN_DIRECTION		register(c94)
#define LIGHT_SUN_COLOR			register(c95)



#define MAT_VIEWPROJ_REG			0
#define MAT_WORLD_REG				4
#define MAT_INVWORLD_REG			8
#define MAT_ROTWORLD_REG			12

#define COLOR_AMBIENT_REG			16
#define COLOR_EMISSIVE_REG			17
#define COLOR_SPECULAR_REG			18

#define SHIELD_CAMPOS_REG			19
#define HEIGHTMAP_INFO_REG			20

#define COMMON_TIME_REG				88

#define LIGHT0_POSITION_REG			89
#define LIGHT0_COLOR_REG			90
#define LIGHT1_POSITION_REG			91
#define LIGHT1_COLOR_REG			92
#define LIGHT_AMBIENT_COLOR_REG		93
#define LIGHT_SUN_DIRECTION_REG		94
#define LIGHT_SUN_COLOR_REG			95



#endif