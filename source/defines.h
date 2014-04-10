#ifndef DEFINES_H
#define DEFINES_H

#include <Seed.h>
using namespace Seed;

const vec3 VECTOR_UP			{0, -1, 0};	// ↑
const vec3 VECTOR_RIGHT		{1, 0, 0};	// →
const vec3 VECTOR_DOWN		{0, 1, 0};	// ↓
const vec3 VECTOR_LEFT		{-1, 0, 0};	// ←

const vec3 VECTOR_UP_LEFT		= VECTOR_UP + VECTOR_LEFT;		// ↖
const vec3 VECTOR_UP_RIGHT	= VECTOR_UP + VECTOR_RIGHT;		// ↗
const vec3 VECTOR_DOWN_RIGHT	= VECTOR_DOWN + VECTOR_RIGHT;	// ↘
const vec3 VECTOR_DOWN_LEFT	= VECTOR_DOWN + VECTOR_LEFT;	// ↙

const vec3 VECTOR_ZERO {0, 0, 0};

#define DISABLE_AUDIO	1

#endif // DEFINES_H
