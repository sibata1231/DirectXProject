#ifndef ___COLLISION_CONTROLLER_H___
#define ___COLLISION_CONTROLLER_H___

#include "Collision.h"
#include <list>

/**
 * @class CollisionController
 * @brief 当たり判定のアクセス権
 * @detail プレイヤーアクセス権の制御を行います。
 */
class CollisionController {
public:
	virtual void					Delete(std::string name) = 0;
	virtual							~CollisionController();
	virtual std::list<Collision *>	OnCollisionEnter(Collision *collision) = 0;
	virtual std::list<Collision *>	OnCollisionStay(Collision *collision) = 0;
	virtual std::list<Collision *>	OnCollisionExit(Collision *collision) = 0;
	virtual std::list<Collision *>	OnTriggerEnter(Collision *collision) = 0;
	virtual std::list<Collision *>	OnTriggerStay(Collision *collision) = 0;
	virtual std::list<Collision *>	OnTriggerExit(Collision *collision) = 0;
};

#endif