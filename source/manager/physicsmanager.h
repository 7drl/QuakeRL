#ifndef PHYSICSMANAGER_H
#define PHYSICSMANAGER_H

#include "../defines.h"
#include <Box2D/Box2D.h>

class Entity;
class CollisionEvent;

enum class BodyType
{
	Normal,
	Sensor
};

enum class CollisionEventType
{
	OnEnter,
	OnLeave
};

typedef std::map<b2Body *, int> CollisionCounterMap;
typedef std::map<b2Body *, CollisionCounterMap>  CollisionCacheMap;
typedef std::list<CollisionEvent> CollisionEventList;
typedef std::list<b2Body*> BodiesScheduledForRemoveList;

class CollisionEvent
{
	public:
		CollisionEvent(CollisionEventType type, b2Body &targetBody, Entity &target, b2Body &otherBody, Entity *otherEntity);

		CollisionEventType GetType() const;
		b2Body &GetTargetBody() const;
		Entity &GetTarget() const;
		b2Body &GetOtherBody() const;
		Entity *GetOtherEntity() const;

	private:
		CollisionEventType eType;
		b2Body &rOtherBody;
		b2Body &rTargetBody;
		Entity *pOtherEntity;
		Entity &rTarget;
};

inline CollisionEventType CollisionEvent::GetType() const
{
	return eType;
}

inline b2Body &CollisionEvent::GetTargetBody() const
{
	return rTargetBody;
}

inline Entity &CollisionEvent::GetTarget() const
{
	return rTarget;
}

inline b2Body &CollisionEvent::GetOtherBody() const
{
	return rOtherBody;
}

inline Entity *CollisionEvent::GetOtherEntity() const
{
	return pOtherEntity;
}

class PhysicsManager: public b2ContactListener
{
	public:
		PhysicsManager();
		virtual ~PhysicsManager();

		void Update(f32 dt);

		b2Body *CreateBody(ISceneObject *obj, b2Vec2 *customSize = NULL);
		b2Body *CreateKinematicBody(ISceneObject *obj, b2Vec2 *customSize = NULL);
		b2Body *CreateStaticBody(ISceneObject *obj, BodyType type = BodyType::Normal, bool track = false, b2Vec2 *customSize = NULL);

		void DestroyBody(b2Body *body);
		bool RayCast(b2Body *startingBody, b2Vec2 relativeDest);

		void AddBodyToRemove(b2Body *body);
		void RemoveBodies();

	protected:
		//Contact listener
		virtual void BeginContact(b2Contact *contact);
		virtual void EndContact(b2Contact *contact);

	private:
		void ClearWorld();
		void AddContact(b2Fixture *fixture, b2Body *body, b2Fixture *otherFixture, b2Body *other);
		void RemoveContact(b2Fixture *fixture, b2Body *body, b2Fixture *otherFixture, b2Body *other);
		void ClearContacts(b2Body *body);

	private:
		b2World *pWorld;
		CollisionCacheMap mapCollisions;
		BodiesScheduledForRemoveList lstBodiesForRemove;
		CollisionEventList lstEvents;
		f32 fTimeLeft;
};

#endif // PHYSICSMANAGER_H
