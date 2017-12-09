
#ifndef _TREEDEFS_H_
#define _TREEDEFS_H_

enum TreeType
{
	TREE_PINNORMAL=0,
	TREE_PINBURN,
	TREE_PINEYELLOW,
	TREE_PINLIGHT,
	TREE_PINSNOW,
	TREE_PIN2,
	TREE_PIN3,
	TREE_PIN4,
	TREE_PIN2SNOW,
	TREE_PIN3SNOW,
	TREE_PIN4SNOW,

	TREE_PALMNORMAL,
	TREE_PALMBURN,
	TREE_PALM2,
	TREE_PALM3,
	TREE_PALM4,
	TREE_PALMB,
	TREE_PALMB2,
	TREE_PALMB3,
	TREE_PALMB4,
	TREE_PALMB5,
	TREE_PALMB6,
	TREE_PALMB7,

	TREE_OAK,
	TREE_OAK2,
	TREE_OAK3,
	TREE_OAK4,

	TREE_COUNT,

	TREE_USER
};

const char * const szTreeNames[TREE_COUNT]=
{
	"pine_normal",
	"pine_burn",
	"pine_yellow",
	"pine_light",
	"pine_snow",
	"pine2",
	"pine3",
	"pine4",
	"pine2_snow",
	"pine3_snow",
	"pine4_snow",

	"palm_normal",
	"palm_burn",
	"palm2",
	"palm3",
	"palm4",
	"palmB",
	"palmB2",
	"palmB3",
	"palmB4",
	"palmB5",
	"palmB6",
	"palmB7",

	"oak",
	"oak2",
	"oak3",
	"oak4"

};

#define TREE_RADIUS				2
#define TREE_REALRADIUS			1.0f

#endif