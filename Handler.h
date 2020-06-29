#pragma once
#include "Spawner.h"
#include "Flowfield.h"
#include "FastNoise.h"

class Handler {
public:
	Handler(Renderer* renderer);
	int _update(float time, Window* window);


private:
	Spawner* _spawner;
	Flowfield _flowfields[GRID_LENGTH / 100][GRID_HEIGHT / 100][GRID_DEPTH / 100];
	RenderObject* _vector_points;
	RenderObject* _boundary_points; 
	RenderObject* _flowfield_obj;
	Renderer* _renderer;
	FastNoise noise;
	Vector3 _view_position = Vector3(500, -500, -500);
	float _noise_val = 0;
	float _elapsed_time = 0;
	float _camera_angle = 0;
};