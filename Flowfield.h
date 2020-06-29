#pragma once
#include "OpenGLEntity.h"

class Flowfield {
public:
	Flowfield() {}
	Flowfield(float length, Vector3 force_point , Vector3 centre) {
		_length = length;
		_force_point = force_point;
		_centre = centre;
		_velocity = Vector3(0,0,0);
		calculate_velocity();
	}
	void calculate_velocity() {
		
		float scaled_x = (float)((_force_point.x - _centre.x) / 10000);
		float scaled_y = (float)((_force_point.y - _centre.y) / 10000);
		float scaled_z = (float)((_force_point.z - _centre.z) / 10000);

		_velocity = Vector3(scaled_x, scaled_y, scaled_z);
		
	}


	float _length;
	Vector3 _centre, _velocity,_force_point;
};