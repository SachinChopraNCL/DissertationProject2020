#include "Handler.h"

Handler::Handler(Renderer* renderer) {
	noise.SetSeed(rand());
	_renderer = renderer;
	_spawner = new Spawner(renderer);
	noise.SetNoiseType(FastNoise::Simplex);
	/* Produces Centre points for vector */
	OGLMesh* vector_grid = new OGLMesh();
	vector<Vector3> vector_positions;
	vector<Vector4> vector_colours; 
	for (int i = 1; i < GRID_LENGTH / 100; i++) {
		for (int j = 1; j < GRID_HEIGHT / 100; j++) {
			for (int k = 1; k < GRID_DEPTH / 100; k++) {
				vector_positions.push_back(Vector3(i * 100, j * 100, k*100));
				vector_colours.push_back(Vector4(1.f, 1.f, 1.f, 1.f));
			}
		}
	}
	vector_grid->SetVertexPositions(vector_positions);
	vector_grid->SetVertexColours(vector_colours);
	vector_grid->SetPrimitiveType(GeometryPrimitive::Points);
	vector_grid->UploadToGPU();
	_vector_points = new RenderObject(vector_grid);
	_renderer->AddRenderObject(_vector_points);
	/* Produces bounding box for vector*/
	OGLMesh* bounding_grid = new OGLMesh();
	vector<Vector3> bounding_positions;
	vector<Vector4> bounding_colours;
	for (int i = 0; i < GRID_LENGTH / 100; i++) {
		for (int j = 0; j < GRID_HEIGHT / 100; j++) {
			for (int k = 0; k < GRID_DEPTH / 100; k++) {
				bounding_positions.push_back(Vector3(i * 100 + 50, j * 100 + 50, k * 100 + 50));
				bounding_colours.push_back(Vector4(1.f, 1.f, 1.f, 1.f));
			}
		}
	}
	bounding_grid->SetVertexPositions(bounding_positions);
	bounding_grid->SetVertexColours(bounding_colours);
	bounding_grid->SetPrimitiveType(GeometryPrimitive::Points);
	bounding_grid->UploadToGPU();
	_boundary_points = new RenderObject(bounding_grid);
	_renderer->AddRenderObject(_boundary_points);
	/*Initialise Flowfields*/
	OGLMesh* flowfield_lines = new OGLMesh();
	vector<Vector3> flowfield_position;
	vector<Vector4> flowfield_colour;
	for (int i = 0; i < (GRID_LENGTH / 100) - 1; i++) {
		for (int j = 0; j < (GRID_LENGTH / 100) - 1; j++){
			for (int k = 0; k < (GRID_LENGTH / 100) - 1; k++) {

				_flowfields[i][j][k] = Flowfield(0, Vector3(0,0,0), Vector3((i + 1) * 100, (j + 1) * 100, (k + 1) * 100));
				flowfield_position.push_back(_flowfields[i][j][k]._centre);
				flowfield_position.push_back(_flowfields[i][j][k]._centre);
				flowfield_colour.push_back(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
				flowfield_colour.push_back(Vector4(1.0f, 1.0f, 1.0f, 1.0f));

			}
		}
	}
	flowfield_lines->SetVertexPositions(flowfield_position);
	flowfield_lines->SetVertexColours(flowfield_colour);
	flowfield_lines->SetPrimitiveType(GeometryPrimitive::Lines);
	flowfield_lines->UploadToGPU();
	_flowfield_obj = new RenderObject(flowfield_lines);
	_renderer->AddRenderObject(_flowfield_obj);
}



int Handler::_update(float time, Window* window) {
	//Update camera position 
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_A)) {
		_view_position.x += 1.0f;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_D)) {
		_view_position.x -= 1.0f;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_W)) {
		_view_position.y -= 1.0f;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_S)) {
		_view_position.y += 1.0f;
	}
	
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_UP)) {
		_view_position.z += 1.0f;
	}

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_DOWN)) {
		_view_position.z -= 1.0f;
	}

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_Q)) {
		_camera_angle += 0.1;
	}
	
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_E)) {
		_camera_angle -= 0.1;
	}
	Matrix4 _rotate = Matrix4::Rotation(_camera_angle, Vector3(0, 1, 0));
	_renderer->SetViewMatrix(  Matrix4::Translation(_view_position) * _rotate);
	//Randomise new flowfields.
	_elapsed_time += time;
	_spawner->_update(_flowfields, time);
	if (_elapsed_time > 0.1) {
		for (int i = 0; i < GRID_LENGTH / 100; i++) {
			for (int j = 0; j < GRID_HEIGHT / 100; j++) {
				for (int k = 0; k < GRID_DEPTH / 100; k++) {
					float x = noise.GetSimplex(i * 100, j * 100, k * 100, _noise_val);
					float y = noise.GetSimplex(i + j + 4000, i + j + 4000, k * 5, _noise_val);
					float z =  noise.GetSimplex(i + j + 10000, i + j + 10000, k * 5, _noise_val);

					Vector3 current_centre = _flowfields[i][j][k]._centre;
					Vector3 position = Vector3(current_centre.x + (x * 50), current_centre.y + (y * 50), current_centre.z + (z * 50));
					_flowfields[i][j][k]._force_point = position;
					_flowfields[i][j][k].calculate_velocity();
					
				}
			}
		}
		//reset timer & delete flowfields for redrawing
		_elapsed_time = 0;
		_renderer->PopRenderObject();
		delete _flowfield_obj;
		OGLMesh* flowfield_lines = new OGLMesh();
		vector<Vector3> flowfield_position;
		vector<Vector4> flowfield_colour;
		for (int i = 0; i < (GRID_LENGTH / 100) - 1; i++) {
			for (int j = 0; j < (GRID_LENGTH / 100) - 1; j++) {
				for (int k = 0; k < (GRID_DEPTH / 100) - 1; k++) {
					flowfield_position.push_back(_flowfields[i][j][k]._centre);
					flowfield_position.push_back(_flowfields[i][j][k]._force_point);
					flowfield_colour.push_back(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
					flowfield_colour.push_back(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
				}
			}
		}
		flowfield_lines->SetVertexPositions(flowfield_position);
		flowfield_lines->SetVertexColours(flowfield_colour);
		flowfield_lines->SetPrimitiveType(GeometryPrimitive::Lines);
		flowfield_lines->UploadToGPU();
		_flowfield_obj = new RenderObject(flowfield_lines);
		_renderer->AddRenderObject(_flowfield_obj);
		_noise_val += 0.5;
		
	}

	return 1;
}