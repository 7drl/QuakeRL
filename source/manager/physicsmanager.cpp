#include "physicsmanager.h"
#include "../defines.h"
#include "../entity/entity.h"

#define DEG2RAD		0.0174532925199432957f
#define RAD2DEG		57.295779513082320876f

#define PIX2M		0.01f
#define M2PIX		(1.0f / PIX2M)

PhysicsManager::PhysicsManager()
	: pWorld(NULL)
	, fTimeLeft(0.0f)
{
	pWorld = sdNew(b2World(b2Vec2(0.0f, 0.0f)));
	pWorld->SetContactListener(this);
}

PhysicsManager::~PhysicsManager()
{
	this->RemoveBodies();
	this->ClearWorld();
	sdDelete(pWorld);
}

void PhysicsManager::Update(f32 dt)
{
	const f32 timeStep = 1/60.0f;
	fTimeLeft += dt;

	while (fTimeLeft >= timeStep)
	{
		fTimeLeft -= timeStep;
		pWorld->Step(timeStep, 8, 3);
		pWorld->ClearForces();
	}

	for (b2Body *b = pWorld->GetBodyList(); b; b = b->GetNext())
	{
		ISceneObject *obj = (ISceneObject *)b->GetUserData();
		if (obj != NULL)
		{
			b2Vec2 p = b->GetPosition();
			f32 a = b->GetAngle() * RAD2DEG;
			obj->SetPosition(p.x * M2PIX, p.y * M2PIX);
			obj->SetRotation(a);
		}
	}

	while (!lstEvents.empty())
	{
		CollisionEvent event = *lstEvents.begin();
		lstEvents.pop_front();
		event.GetTarget().OnCollision(event);
	}
}

void PhysicsManager::ClearWorld()
{
	for (b2Body *b = pWorld->GetBodyList(); b; b = b->GetNext())
	{
		ISceneObject *obj = (ISceneObject *)b->GetUserData();
		if (obj != NULL)
		{
			pWorld->DestroyBody(b);
		}
	}
}

void PhysicsManager::AddBodyToRemove(b2Body *body)
{
	lstBodiesForRemove.push_back(body);
}

void PhysicsManager::RemoveBodies()
{
	// Remove all the bodies colected to be destroyed
	for (BodiesScheduledForRemoveList::iterator it = lstBodiesForRemove.begin(), end = lstBodiesForRemove.end(); it != end; ++it)
	{
		//this->DestroyBody(*it);
		pWorld->DestroyBody(*it);
	}
	lstBodiesForRemove.clear();
}


b2Body *PhysicsManager::CreateBody(ISceneObject *obj, b2Vec2 *customSize)
{
	if (!obj)
		return NULL;

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(obj->GetX() * PIX2M, obj->GetY() * PIX2M);
	bodyDef.angle = 0;
	bodyDef.userData = obj;
	b2Body *b = pWorld->CreateBody(&bodyDef);

	b2PolygonShape boxShape;
	if (customSize)
	{
		boxShape.SetAsBox(customSize->x * 0.5f * PIX2M, customSize->y * 0.5f * PIX2M);
	}
	else
	{
		boxShape.SetAsBox(obj->GetWidth() * 0.5f * PIX2M, obj->GetHeight() * 0.5f * PIX2M);
	}

	b2FixtureDef fixDef;
	fixDef.shape = &boxShape;
	fixDef.density = 1.0f;
	fixDef.restitution = 0.10f;
	fixDef.friction = 0.0f;
	b->CreateFixture(&fixDef);

	return b;
}

b2Body *PhysicsManager::CreateKinematicBody(ISceneObject *obj, b2Vec2 *customSize)
{
	if (!obj)
		return NULL;

	b2BodyDef bodyDef;
	bodyDef.type = b2_kinematicBody;
	bodyDef.position.Set(obj->GetX() * PIX2M, obj->GetY() * PIX2M);
	bodyDef.angle = 0;
	bodyDef.userData = obj;
	b2Body *b = pWorld->CreateBody(&bodyDef);

	b2PolygonShape boxShape;
	if (customSize)
	{
		boxShape.SetAsBox(customSize->x * 0.5f * PIX2M, customSize->y * 0.5f * PIX2M);
	}
	else
	{
		boxShape.SetAsBox(obj->GetWidth() * 0.5f * PIX2M, obj->GetHeight() * 0.5f * PIX2M);
	}

	b2FixtureDef fixDef;
	fixDef.shape = &boxShape;
	fixDef.density = 1.0f;
	fixDef.restitution = 0.10f;
	fixDef.friction = 0.0f;
	b->CreateFixture(&fixDef);

	return b;
}

void PhysicsManager::DestroyBody(b2Body *body)
{
	if (!body)
		return;

	this->ClearContacts(body);
	pWorld->DestroyBody(body);
}

b2Body *PhysicsManager::CreateStaticBody(ISceneObject *obj, BodyType type, bool track, b2Vec2 *customSize)
{
	if (!obj)
		return NULL;

	b2BodyDef bodyDef;
	bodyDef.type = b2_staticBody;

	// FIXME: Metadata objects nao tem o pivot no centro do objeto - corrigir na Seed durante o load dos objects.
	// Aqui estou compensando o x e y com metade da largura e altura.
	bodyDef.position.Set((obj->GetX() + (obj->GetWidth() * 0.5f)) * PIX2M, (obj->GetY() + (obj->GetHeight() * 0.5f)) * PIX2M);
	bodyDef.angle = 0.0f;
	bodyDef.userData = track ? obj : NULL;
	b2Body *b = pWorld->CreateBody(&bodyDef);

	b2PolygonShape boxShape;
	if (customSize)
	{
		boxShape.SetAsBox(customSize->x * 0.5f * PIX2M, customSize->y * 0.5f * PIX2M);
	}
	else
	{
		boxShape.SetAsBox(obj->GetWidth() * 0.5f * PIX2M, obj->GetHeight() * 0.5f * PIX2M);
	}

	b2FixtureDef fixDef;
	fixDef.shape = &boxShape;
	fixDef.density = 1.0f;
	fixDef.isSensor = type == BodyType::Sensor;
	b->CreateFixture(&fixDef);

	return b;
}

void PhysicsManager::AddContact(b2Fixture *fixture, b2Body *body, b2Fixture *otherFixture, b2Body *other)
{
	CollisionCacheMap::iterator it = mapCollisions.lower_bound(body);
	if ((it != mapCollisions.end()) && !(mapCollisions.key_comp()(body, it->first)))
	{
		//found
	}
	else
	{
		//not found, so insert new body in the cache
		it = mapCollisions.insert(it, std::make_pair(body, CollisionCounterMap()));
	}

	CollisionCounterMap &counter = it->second;

	//Now check if the body in the cache is colliding with the other body
	CollisionCounterMap::iterator counterIt = counter.lower_bound(other);
	if ((counterIt != counter.end()) && !(counter.key_comp()(body, counterIt->first)))
	{
		//Already exists, increment counter
		counterIt->second++;
	}
	else
	{
		//Does not exists, insert
		counter.insert(counterIt, std::make_pair(other, 1));

		lstEvents.push_back(CollisionEvent(
			CollisionEventType::OnEnter,
			*body, *reinterpret_cast<Entity *>(fixture->GetUserData()),
			*other, reinterpret_cast<Entity *>(otherFixture->GetUserData())
		));
	}
}

void PhysicsManager::RemoveContact(b2Fixture *fixture, b2Body *body, b2Fixture *otherFixture, b2Body *other)
{
	CollisionCacheMap::iterator it = mapCollisions.find(body);
	if (it == mapCollisions.end())
	{
		Log("[PhysicsManager::RemoveContact] Warning: Body already removed");
		return;
	}

	CollisionCounterMap::iterator counterIt = it->second.find(other);
	if (counterIt == it->second.end())
	{
		Log("[PhysicsManager::RemoveContact] Warning: Other already removed");
		return;
	}

	counterIt->second--;
	if (counterIt->second == 0)
	{
		lstEvents.push_back(CollisionEvent(
			CollisionEventType::OnLeave,
			*body, *reinterpret_cast<Entity *>(fixture->GetUserData()),
			*other, reinterpret_cast<Entity *>(otherFixture->GetUserData())
		));

		it->second.erase(counterIt);
		if (it->second.empty())
		{
			mapCollisions.erase(it);
		}
	}
}

void PhysicsManager::ClearContacts(b2Body *body)
{
	mapCollisions.erase(body);

	for (CollisionCacheMap::iterator it = mapCollisions.begin(), end = mapCollisions.end(); it != end; )
	{
		CollisionCacheMap::iterator original = it++;

		original->second.erase(body);
		if (original->second.empty())
			mapCollisions.erase(original);
	}

	//
	//Clear events
	CollisionEventList::iterator next;
	for (CollisionEventList::iterator it = lstEvents.begin(), end = lstEvents.end();it != end;it = next)
	{
		next = it++;

		if (&it->GetTargetBody() == body || &it->GetOtherBody() == body)
		{
			lstEvents.erase(it);
		}
	}
}

void PhysicsManager::BeginContact(b2Contact *contact)
{
	b2Fixture *fixtureA = contact->GetFixtureA();
	b2Fixture *fixtureB = contact->GetFixtureB();

	b2Body *a = fixtureA->GetBody();
	b2Body *b = fixtureB->GetBody();

	if (fixtureA->GetUserData())
	{
		this->AddContact(fixtureA, a, fixtureB, b);
	}

	if (fixtureB->GetUserData())
	{
		this->AddContact(fixtureB, b, fixtureA, a);
	}
}

void PhysicsManager::EndContact(b2Contact *contact)
{
	b2Fixture *fixtureA = contact->GetFixtureA();
	b2Fixture *fixtureB = contact->GetFixtureB();

	b2Body *a = fixtureA->GetBody();
	b2Body *b = fixtureB->GetBody();

	if (fixtureA->GetUserData())
	{
		this->RemoveContact(fixtureA, a, fixtureB, b);
	}

	if (fixtureB->GetUserData())
	{
		this->RemoveContact(fixtureB, b, fixtureA, a);
	}
}

class MyRayCastCallback: public b2RayCastCallback
{
	public:
		MyRayCastCallback(b2Body *bodyToIgnore)
			: pIgnoreBody(bodyToIgnore)
			, fFound(false)
		{
		}

		virtual float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction)
		{
			UNUSED(point)
			UNUSED(normal)
			UNUSED(fraction)

			if (fixture->GetBody() == pIgnoreBody)
				return -1;

			fFound = true;
			return 0;
		}

		bool Found() const
		{
			return fFound;
		}

	private:
		b2Body *pIgnoreBody;
		bool fFound;
};

bool PhysicsManager::RayCast(b2Body *startingBody, b2Vec2 relativeDest)
{
	MyRayCastCallback callback(startingBody);

	relativeDest += startingBody->GetPosition();
	pWorld->RayCast(&callback, startingBody->GetPosition(), relativeDest);

	return callback.Found();
}

CollisionEvent::CollisionEvent(CollisionEventType type, b2Body &targetBody, Entity &target, b2Body &otherBody, Entity *otherEntity)
	: eType(type)
	, rOtherBody(otherBody)
	, rTargetBody(targetBody)
	, pOtherEntity(otherEntity)
	, rTarget(target)
{
}


