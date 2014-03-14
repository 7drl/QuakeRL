#ifndef DEFINES_H
#define DEFINES_H

#include <Seed.h>
using namespace Seed;

const Vector3f VECTOR_UP		{0, -1, 0};	// ↑
const Vector3f VECTOR_RIGHT		{1, 0, 0};	// →
const Vector3f VECTOR_DOWN		{0, 1, 0};	// ↓
const Vector3f VECTOR_LEFT		{-1, 0, 0};	// ←

const Vector3f VECTOR_UP_LEFT		= VECTOR_UP + VECTOR_LEFT;		// ↖
const Vector3f VECTOR_UP_RIGHT		= VECTOR_UP + VECTOR_RIGHT;		// ↗
const Vector3f VECTOR_DOWN_RIGHT	= VECTOR_DOWN + VECTOR_RIGHT;	// ↘
const Vector3f VECTOR_DOWN_LEFT		= VECTOR_DOWN + VECTOR_LEFT;	// ↙

const Vector3f VECTOR_ZERO {0, 0, 0};

#define DISABLE_AUDIO	1

#endif // DEFINES_H
