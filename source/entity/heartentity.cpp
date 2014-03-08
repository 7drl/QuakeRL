#include "heartentity.h"
#include "entityfactory.h"

ENTITY_CREATOR("Heart", HeartEntity)

HeartEntity::HeartEntity()
	: ItemEntity("Heart", "Heart")
{
}

