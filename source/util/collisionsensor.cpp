#include "collisionsensor.h"
#include "../scene/gamescene.h"

CollisionSensor::CollisionSensor()
	: pBody(NULL)
{
}

CollisionSensor::~CollisionSensor()
{
	gPhysics->DestroyBody(pBody);
	pBody = nullptr;
}

void CollisionSensor::Load(ISceneObject &metadata, void *userData)
{
	this->Load(metadata, false, NULL, userData);
}

void CollisionSensor::Load(ISceneObject &metadata, bool track, b2Vec2 *customSize, void *userData)
{
	pBody = gPhysics->CreateStaticBody(&metadata, BodyType::Sensor, track, customSize);
	pBody->GetFixtureList()->SetUserData(userData);
}

void CollisionSensor::Disable()
{
	pBody->GetFixtureList()->SetUserData(NULL);
}
