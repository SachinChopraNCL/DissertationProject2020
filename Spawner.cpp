#include "Spawner.h"


Spawner::Spawner(Renderer* renderer) {
	_renderer = renderer;
	srand(time(NULL));
	if(using_compute == true)
		_init_compute_shader();
	else {
		for (auto i = 0; i < _PARTICLE_LIMIT; i++) {
			_spawn();
		}
	}
}

int Spawner::_update(Flowfield(&_flowfields)[GRID_HEIGHT / 100][GRID_LENGTH / 100][GRID_DEPTH / 100], float time_elapsed) {
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	if (using_compute == false) {
		for (Particle* _particle : _particle_list) {
			if (_particle->get_global_time() > _particle->TTD) {
				float x = rand() % 100 + z_bound;
				float y = rand() % 100 + y_bound;
				float z = rand() % 100 + z_bound;
				_particle->_pos = Vector3(x, y, z);
				_particle->_vel = Vector3(0, 0, 0);
				_particle->global_total_time = 0;
			}
			double expected_x = _particle->_pos.x / 100;
			double expected_y = _particle->_pos.y / 100;
			double expected_z = _particle->_pos.z / 100;
			int int_x = (expected_x < 0.5) ? 0 : round(expected_x) - 1;
			int int_y = (expected_y < 0.5) ? 0 : round(expected_y) - 1;
			int int_z = (expected_z < 0.5) ? 0 : round(expected_z) - 1;
			_particle->_update(_flowfields[int_x][int_y][int_z]._velocity, time_elapsed);

		}
	}
	else {
		_update_compute_shader(_flowfields, time_elapsed);
		_draw_compute_shader();
	}
	return 1;
}

void Spawner::_spawn() {
	float x = rand() % 100 + x_bound;
	float y = rand() % 100 + y_bound;
	float z = rand() % 100 + z_bound;
	_particle_list.push_back(new Particle(Vector3(x, y, z)));
	_particle_list.back()->_obj_ref->is_particle = true;
	_renderer->AddToFrontRenderObject(_particle_list.back()->_obj_ref);
	_particle_list.back()->_obj_ref->SetShader(shader);
}


void Spawner::_init_compute_shader() {

	struct pos* positions = new pos[_PARTICLE_LIMIT];
	struct vel* vels = new vel[_PARTICLE_LIMIT];
	for (int i = 0; i < _PARTICLE_LIMIT; i++) {
		positions[i].x = rand() % 100 + x_bound;
		positions[i].y = rand() % 100 + y_bound;
		positions[i].z = rand() % 100 + z_bound;
		positions[i].w = 1;
		vels[i].vx = 0;
		vels[i].vy = 0;
		vels[i].vz = 0;
		vels[i].vw = 0;
	}

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &pos_sbo);
	glBindBuffer(GL_ARRAY_BUFFER, pos_sbo);
	glEnableVertexAttribArray(0);
	glVertexAttribFormat(0, 4, GL_FLOAT, false, 0);
	glVertexAttribBinding(0, 0);
	glBindVertexBuffer(0, pos_sbo, 0, sizeof(struct pos));
	glBindVertexArray(0);



	glBindBuffer(GL_SHADER_STORAGE_BUFFER, pos_sbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, _PARTICLE_LIMIT * sizeof(struct pos), positions, GL_STATIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glGenBuffers(1, &vel_sbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, vel_sbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, _PARTICLE_LIMIT * sizeof(struct vel), vels, GL_STATIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	

	float* _local_times = new float[_PARTICLE_LIMIT];

	for (auto i = 0; i < _PARTICLE_LIMIT; i++) {
		_local_times[i] = 0;
	}

	glGenBuffers(1, &time_sbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, time_sbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, _PARTICLE_LIMIT * sizeof(float), _local_times, GL_STATIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	
	glGenTextures(1, &field_sbo);
	glBindTexture(GL_TEXTURE_3D, field_sbo);
	glTextureStorage3D(field_sbo, 1, GL_RGBA32F, 10,10,10);

}

void Spawner::_update_compute_shader(Flowfield(&_flowfields)[GRID_HEIGHT / 100][GRID_LENGTH / 100][GRID_DEPTH / 100], float time_elapsed) {

	/*Load flowfields into a buffer to be passed into the compute shader*/
	int index = 0;
	for (auto x = 0; x < GRID_HEIGHT / 100; x++)
		for (auto y = 0; y < GRID_LENGTH / 100; y++)
			for (auto z = 0; z < GRID_DEPTH / 100; z++) {
				Vector3 current_velocity = _flowfields[x][y][z]._velocity;
				vector_data[index] = current_velocity.x;
				vector_data[index + 1] = current_velocity.y;
				vector_data[index + 2] = current_velocity.z;
				vector_data[index + 3] = 1;
				index += 4;
			}
	glTextureSubImage3D(field_sbo, 0, 0, 0, 0, 10, 10, 10, GL_RGBA, GL_FLOAT, vector_data);
	glBindImageTexture(0, field_sbo, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA32F);
	compute_shader->Bind();
	int dt_location = glGetUniformLocation(compute_shader->GetProgramID(), "dt");
	int field_location = glGetUniformLocation(compute_shader->GetProgramID(), "field");
	glUniform1i(field_location, 0);
	glUniform1f(dt_location, time_elapsed);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, time_sbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, pos_sbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, vel_sbo);
	compute_shader->Execute(_PARTICLE_LIMIT / WORK_GROUP_SIZE, 1, 1);
	glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
	compute_shader->Unbind();

}

void Spawner::_draw_compute_shader() {

	default_shader->Bind();

	Matrix4 projMatrix = _renderer->GetProjectionMatrix();
	int projLocation = glGetUniformLocation(default_shader->GetProgramID(), "projMatrix");
	glUniformMatrix4fv(projLocation, 1, false, (float*)&projMatrix);

	Matrix4 viewMatrix = _renderer->GetViewMatrix();
	int viewLocation = glGetUniformLocation(default_shader->GetProgramID(), "viewMatrix");
	glUniformMatrix4fv(viewLocation, 1, false, (float*)&viewMatrix);

	glBindVertexArray(vao);
	glDrawArrays(GL_POINTS, 0, _PARTICLE_LIMIT);
	glBindVertexArray(0);

	default_shader->Unbind();
}