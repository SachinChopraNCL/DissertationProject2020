#pragma once
#include "OpenGLEntity.h"
#include "Particle.h"
#include "Flowfield.h"
#include <OGLComputeShader.h>
#define WORK_GROUP_SIZE 128

class Spawner {
public:
	Spawner(Renderer* renderer);
	int _update(Flowfield(&_flowfields)[GRID_HEIGHT / 100][GRID_LENGTH / 100][GRID_DEPTH / 100], float time_elapsed);
	void _spawn();
	void _init_compute_shader();
	void _update_compute_shader(Flowfield(&_flowfields)[GRID_HEIGHT / 100][GRID_LENGTH / 100][GRID_DEPTH / 100], float time_elapsed);
	void _draw_compute_shader();

	

private:

	struct pos {
		float x, y, z, w;
	};

	struct vel {
		float vx, vy, vz, vw;
	};

	bool using_compute = true;
	GLuint field_sbo, pos_sbo, vel_sbo, time_sbo, vao;
	float x_bound = 450, y_bound = 450, z_bound = 450;
	const int _PARTICLE_LIMIT = 1048576;
	vector<Particle*> _particle_list;
	Renderer* _renderer;
	OGLShader* shader = new OGLShader("rasterisationVert.glsl", "rasterisationFrag.glsl");
	OGLShader* default_shader = new OGLShader("ComputeVert.glsl", "ComputeFrag.glsl");
	OGLComputeShader* compute_shader = new OGLComputeShader("ComputeShader.glsl");
	float vector_data[10 * 10 * 10 * 4];


};