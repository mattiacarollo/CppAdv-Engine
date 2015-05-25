#pragma once

#include "Vector3.h"

class RigidBody;
class Collision
{

public:
	Collision();
	~Collision();
	Collision(RigidBody* rigidbodyfirst, RigidBody* rigidbodysecond, const Vector3& pointOfApplication, const Vector3& force, const Vector3& normal);

	void ApplyCollision();

private:
	RigidBody* m_firstObj;
	RigidBody* m_secondObj;
	Vector3 m_vpointOfApplication;
	Vector3 m_vforce;
	Vector3 m_vnormal;
};