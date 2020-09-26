#ifndef __LEAF_OBJECT_H__
#define __LEAF_OBJECT_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image.h"

typedef double Coord_t;
typedef double Angle_t;

typedef struct _Leaf_Object {
	Leaf_Image * sprite;
	
	Coord_t x; /*X coord*/
	Coord_t y; /*Y coord*/
	Angle_t r; /*Rotation angle*/
}Leaf_Object;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
