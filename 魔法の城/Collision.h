#ifndef ___COLLISION_H___
#define ___COLLISION_H___

#include "main.h"

class CollisionParam {
public:
	std::string		*m_TagName;			// タグ名
	std::string		*m_name;			// オブジェクト名
	XMFLOAT3		*m_pos;
	XMFLOAT3		*m_size;
	float			 m_fRadius;
	CollisionParam(std::string *name, std::string *tag, float radius, XMFLOAT3 *pos, XMFLOAT3 *size);
};

enum class CollisionTypes{
	CIRCLE,
	RECT,
	SPHERE,
	BOX,
};

class Collision {
private:
	bool Circle2D(XMFLOAT2 posA, XMFLOAT2 posB, float fRadiusA, float fRadiusB);
	bool Rect2D(XMFLOAT2 posA, XMFLOAT2 posB, XMFLOAT2 sizeA, XMFLOAT2 sizeB);
	bool Sphere3D(XMFLOAT3 posA, XMFLOAT3 posB, float fRadiusA, float fRadiusB);
	bool Box3D(XMFLOAT3 posA, XMFLOAT3 posB, XMFLOAT3 sizeA, XMFLOAT3 sizeB);

public:
	CollisionParam *m_collisionParam;
	Collision(CollisionParam *collisionParam);
	~Collision();
	bool OnCollision(Collision* collision, int dir, CollisionTypes mode);
};

#endif