#pragma once

#include "OpenGLEntity.h"

class Particle {
public:
	Particle() { _is_dead = true; _obj_ref = NULL; }
	Particle(Vector3 pos);
	~Particle();
	int _update(Vector3 velocity,float time_elapsed);
	float get_global_time();
	Vector3 _acc, _vel, _pos;
	float _inv_mass = 50;
	bool _is_dead = false;
	unsigned int index;
	float global_total_time;
	const float TTD = 8;
	RenderObject* _obj_ref;
};