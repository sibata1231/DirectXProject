#include "Collision.h"

CollisionParam::CollisionParam(std::string *name, std::string *tag, float radius, XMFLOAT3 *pos, XMFLOAT3 *size) {
	m_name = name;
	m_TagName = tag;
	m_fRadius = radius;
	m_pos = pos;
	m_size = size;
}

Collision::Collision(CollisionParam *collisionParam) : m_collisionParam(collisionParam) {}

Collision::~Collision(){
	SAFE_DELETE(m_collisionParam);
}

bool Collision::OnCollision(Collision *Collision ,int dir,CollisionTypes mode) {
	switch (mode)
	{
	case CollisionTypes::RECT:
		switch (dir)
		{
			case 0: return Rect2D(XMFLOAT2(m_collisionParam->m_pos->x, m_collisionParam->m_pos->y), XMFLOAT2(Collision->m_collisionParam->m_pos->x, Collision->m_collisionParam->m_pos->y), XMFLOAT2(m_collisionParam->m_size->x, m_collisionParam->m_size->y), XMFLOAT2(Collision->m_collisionParam->m_size->x, Collision->m_collisionParam->m_size->y));break;
			case 1: return Rect2D(XMFLOAT2(m_collisionParam->m_pos->y, m_collisionParam->m_pos->z), XMFLOAT2(Collision->m_collisionParam->m_pos->y, Collision->m_collisionParam->m_pos->z), XMFLOAT2(m_collisionParam->m_size->y, m_collisionParam->m_size->z), XMFLOAT2(Collision->m_collisionParam->m_size->y, Collision->m_collisionParam->m_size->z));break;
			case 2:	return Rect2D(XMFLOAT2(m_collisionParam->m_pos->z, m_collisionParam->m_pos->x), XMFLOAT2(Collision->m_collisionParam->m_pos->z, Collision->m_collisionParam->m_pos->x), XMFLOAT2(m_collisionParam->m_size->z, m_collisionParam->m_size->x), XMFLOAT2(Collision->m_collisionParam->m_size->z, Collision->m_collisionParam->m_size->x));break;
		}
	case CollisionTypes::CIRCLE:
		switch (dir){
			case 0:	return Circle2D(XMFLOAT2(m_collisionParam->m_pos->x, m_collisionParam->m_pos->y), XMFLOAT2(Collision->m_collisionParam->m_pos->x, Collision->m_collisionParam->m_pos->y), m_collisionParam->m_fRadius, Collision->m_collisionParam->m_fRadius);break;
			case 1:	return Circle2D(XMFLOAT2(m_collisionParam->m_pos->y, m_collisionParam->m_pos->z), XMFLOAT2(Collision->m_collisionParam->m_pos->y, Collision->m_collisionParam->m_pos->z), m_collisionParam->m_fRadius, Collision->m_collisionParam->m_fRadius);break;
			case 2: return Circle2D(XMFLOAT2(m_collisionParam->m_pos->z, m_collisionParam->m_pos->x), XMFLOAT2(Collision->m_collisionParam->m_pos->z, Collision->m_collisionParam->m_pos->x), m_collisionParam->m_fRadius, Collision->m_collisionParam->m_fRadius);break;
		}
	case CollisionTypes::BOX:
		return Box3D(*m_collisionParam->m_pos, *Collision->m_collisionParam->m_pos, *m_collisionParam->m_size, *Collision->m_collisionParam->m_size);
	case CollisionTypes::SPHERE:
		return Sphere3D(*m_collisionParam->m_pos, *Collision->m_collisionParam->m_pos, m_collisionParam->m_fRadius, Collision->m_collisionParam->m_fRadius);
	}
	return false;

}

bool Collision::Circle2D(XMFLOAT2 posA, XMFLOAT2 posB, float fRadiusA, float fRadiusB) {
	float fX = posA.x - posB.x;
	float fY = posA.y - posB.y;
	float fR = fRadiusA + fRadiusB;
	return fX * fX + fY * fY <= fR * fR;
}

bool Collision::Rect2D(XMFLOAT2 posA, XMFLOAT2 posB, XMFLOAT2 sizeA, XMFLOAT2 sizeB) {
	return	posA.x - sizeA.x <= posB.x + sizeB.x &&
			posB.x - sizeB.x <= posA.x + sizeA.x &&
			posA.y - sizeA.y <= posB.y + sizeB.y &&
			posB.y - sizeB.y <= posA.y + sizeA.y;
}

bool Collision::Sphere3D(XMFLOAT3 posA, XMFLOAT3 posB, float fRadiusA, float fRadiusB) {
	float fX = posA.x - posB.x;
	float fY = posA.y - posB.y;
	float fZ = posA.z - posB.z;
	float fR = fRadiusA + fRadiusB;
	return fX * fX + fY * fY + fZ * fZ <= fR * fR * fR;
}

bool Collision::Box3D(XMFLOAT3 posA, XMFLOAT3 posB, XMFLOAT3 sizeA, XMFLOAT3 sizeB) {
	return	posA.x - sizeA.x <= posB.x + sizeB.x &&
			posB.x - sizeB.x <= posA.x + sizeA.x &&
			posA.y - sizeA.y <= posB.y + sizeB.y &&
			posB.y - sizeB.y <= posA.y + sizeA.y &&
			posA.z - sizeA.z <= posB.z + sizeB.z &&
			posB.z - sizeB.z <= posA.z + sizeA.z;
}