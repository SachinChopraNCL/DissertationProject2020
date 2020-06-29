
#include "Particle.h"

Particle::Particle(Vector3 pos) {
	_is_dead = false;
	_acc = Vector3(0, 0, 0);
	_vel = Vector3(0, 0, 0);
	_pos = pos;
	OGLMesh* particle_mesh = new OGLMesh();
	vector<Vector3> particle_verticies;
	vector<Vector4> particle_colours; 


	particle_verticies.push_back(Vector3(0,0,0));
	particle_colours.push_back(Vector4(0, 1, 0, 0));
	

	particle_mesh->SetVertexColours(particle_colours);
	particle_mesh->SetVertexPositions(particle_verticies);
	particle_mesh->SetPrimitiveType(GeometryPrimitive::Points);
	particle_mesh->UploadToGPU();

	_obj_ref = new RenderObject(particle_mesh);

}

float Particle::get_global_time() {
	return global_total_time;
}

int Particle::_update(Vector3 acc, float time_elapsed) {


	global_total_time += time_elapsed;

	this->_acc = acc * _inv_mass;
	this->_vel = this->_vel + (this->_acc * time_elapsed) ;
	if (this->_vel.x >= 0.5) { this->_vel.x = 0.5; }
	if (this->_vel.x <= -0.5) { this->_vel.x = -0.5; }
	if (this->_vel.y >= 0.5) { this->_vel.y = 0.5; }
	if (this->_vel.y <= -0.5) { this->_vel.y = -0.5; }
	if (this->_vel.z >= 0.5) { this->_vel.z = 0.5; }
	if (this->_vel.z <= -0.5) { this->_vel.z = -0.5; }
	this->_pos += this->_vel;
	
	if (this->_pos.x > GRID_LENGTH) { this->_pos.x = 0;}
	else if (this->_pos.x < 0) { this->_pos.x = GRID_LENGTH;}

	if (this->_pos.y > GRID_HEIGHT) {this->_pos.y = 0;}
	else if (this->_pos.y < 0) { this->_pos.y = GRID_HEIGHT;}
	
	if (this->_pos.z > GRID_DEPTH) { this->_pos.z = 0; }
	else if (this->_pos.z < 0) { this->_pos.z = GRID_DEPTH; }


	Matrix4 _translation = Matrix4::Translation(this->_pos);
	Matrix4 _scale = Matrix4::Scale(Vector3(1, 1, 1));
	this->_obj_ref->SetTransform(_translation * _scale);
	this->_obj_ref->time_alive = this->global_total_time;
	return 1;
}