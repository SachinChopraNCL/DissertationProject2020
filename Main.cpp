#include "../../Common/Window.h"
#include "../../Common/TextureLoader.h"
#include "../../Common/Vector3.h"
#include "../../Common/Vector4.h"
#include "../../Common/MeshGeometry.h"
#include "../../Common/Maths.h"

#include "../../Plugins/OpenGLRendering/OGLRenderer.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"

#include "Renderer.h"
#include "Handler.h"



using namespace NCL;
using namespace CSC3223;





int main() {
	srand(time(NULL));
	Window*w = Window::CreateGameWindow("CSC3223 Tutorials!");


	if (!w->HasInitialised()) {
		return -1;
	}
	Renderer*	renderer = new Renderer(*w);
	float aspect = w->GetScreenAspect();
	Matrix4 projMatrix = Matrix4::Perspective(1.0f, 6000.0f, aspect, 45.0f);

	renderer->SetProjectionMatrix(projMatrix);


	Handler h = Handler(renderer);




	while (w->UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {
		glGetString(GL_VERSION);
	

		float time = w->GetTimer()->GetTimeDelta();
		h._update(time, w);
		renderer->Update(time);

		
		renderer->Render();
		


		if (Window::GetKeyboard()->KeyPressed(KEYBOARD_PRIOR)) {
			w->ShowConsole(true);
		}
		if (Window::GetKeyboard()->KeyPressed(KEYBOARD_NEXT)) {
			w->ShowConsole(false);
		}

		if (Window::GetKeyboard()->KeyPressed(KEYBOARD_HOME)) {
			w->SetFullScreen(true);
		}
		if (Window::GetKeyboard()->KeyPressed(KEYBOARD_END)) {
			w->SetFullScreen(false);
		}

		w->SetTitle(std::to_string(time));
	}

	delete renderer;

	Window::DestroyGameWindow();
}