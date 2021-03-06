#pragma once
#include "../../Plugins/OpenGLRendering/OGLRenderer.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"

#include "RenderObject.h"
#include "../../Common/Vector2.h"

namespace NCL {
	namespace CSC3223 {

		class Renderer : public OGLRenderer
		{
		public:
			Renderer(Window& w);
			Vector3		vector_speed = Vector3(0,0,0); // Added for shader
			vector<Vector3> speed_store;
			~Renderer();

			void AddRenderObject(RenderObject* ro) {
				renderObjects.emplace_back(ro);
			}

			void AddToFrontRenderObject(RenderObject* ro) {
				renderObjects.insert(renderObjects.begin(), ro);
			}

			void DeleteAllRenderObjects() {
				for (const RenderObject* object : renderObjects) {
					delete object;
				}
				renderObjects.clear();
			}

			void PopRenderObject() {
				renderObjects.pop_back();
			}

			Matrix4 GetProjectionMatrix() {
				return projMatrix;
			}

			Matrix4 GetViewMatrix() {
				return viewMatrix;
			}

			void SetProjectionMatrix(const Matrix4&m) {
				projMatrix = m;
			}

			void SetViewMatrix(const Matrix4&m) {
				viewMatrix = m;
			}

		protected:
			void RenderNode(RenderObject* root);
	
			void OnWindowResize(int w, int h)	override;

			void RenderFrame()	override;
			OGLShader*		defaultShader;

			Matrix4		projMatrix;
			Matrix4		viewMatrix;
			

			vector<RenderObject*> renderObjects;
		};
	}
}

