#include <engine.hpp>

#include "mesh.hpp"
#include "model.hpp"
#include "camera.hpp"
#include "ackglm.hpp"
#include "../../scene/entity.hpp"
#include "../opengl/shader.hpp"

#include "../debug/debugdrawer.hpp"

#define LIGHT_LIMIT 16

extern Shader * defaultShader;

Shader * FB(Shader * sh)
{
	if(sh) return sh;
	return defaultShader;
}

struct LIGHTDATA
{
	__attribute__((aligned(4))) int type;
	__attribute__((aligned(4))) float intensity;
	__attribute__((aligned(4))) float arc;
	__attribute__((aligned(16))) VECTOR position;
	__attribute__((aligned(16))) VECTOR direction;
	__attribute__((aligned(16))) COLOR color;
};

static BUFFER * ubo = nullptr;
static BUFFER * bonesBuf = nullptr;

extern Shader * currentShader;

ACKNEXT_API_BLOCK
{
	CAMERA * camera;

	COLOR sky_color = { 0.3, 0.7, 1.0, 1.0 };

	void render_scene_with_camera(CAMERA * perspective)
	{
		if(perspective == nullptr) {
			return;
		}

		if(!ubo)
		{
			ubo = buffer_create(UNIFORMBUFFER);
			buffer_set(ubo, sizeof(LIGHTDATA) * LIGHT_LIMIT, nullptr);
		}

		if(!bonesBuf)
		{
			bonesBuf = buffer_create(UNIFORMBUFFER);
			buffer_set(bonesBuf, sizeof(MATRIX) * ACKNEXT_MAX_BONES, NULL);
		}

		// glEnable(GL_CULL_FACE);
		// glCullFace(GL_BACK);

		if(opengl_wireFrame) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		} else {
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		}

		glEnable(GL_DEPTH_TEST);
		glClearColor(sky_color.red, sky_color.green, sky_color.blue, sky_color.alpha);
		glClearDepth(1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// TODO: add current_view for rendering steps
		MATRIX matView, matProj;
		camera_to_matrix(perspective, &matView, &matProj, NULL);

		for(ENTITY * ent = ent_next(nullptr); ent != nullptr; ent = ent_next(ent))
		{
			// Entity * entity = promote<Entity>(ent);
			if(ent->model == nullptr) {
				continue;
			}
			// TODO: Filter entity by mask bits

			MATRIX matWorld;
			glm_to_ack(&matWorld,
				glm::translate(glm::mat4(), ack_to_glm(ent->position)) *
				glm::mat4_cast(ack_to_glm(ent->rotation)) *
				glm::scale(glm::mat4(), ack_to_glm(ent->scale)));

			// Update bones
			{
				MATRIX transforms[ACKNEXT_MAX_BONES];
				transforms[0] = ent->model->bones[0].transform;
				for(int i = 1; i < ent->model->boneCount; i++)
				{
					BONE * bone = &ent->model->bones[i];
					mat_mul(&transforms[i], &bone->transform, &transforms[bone->parent]);
				}

				MATRIX * boneTrafos = (MATRIX*)buffer_map(bonesBuf, READWRITE);
				for(int i = 0; i < ent->model->boneCount; i++)
				{
					BONE * bone = &ent->model->bones[i];
					mat_mul(&boneTrafos[i], &bone->bindToBoneTransform, &transforms[i]);
				}
				buffer_unmap(bonesBuf);
			}

			if(ent->material != nullptr) {
				// sets shader&material vars
				opengl_setMaterial(ent->material);
			}

			Model * model = promote<Model>(ent->model);
			for(int i = 0; i < model->api().meshCount; i++)
			{
				MESH const * mesh = model->api().meshes[i];
				MATERIAL const * mtl = model->api().materials[i];

				if(ent->material == nullptr) {
					// sets shader&material vars
					opengl_setMaterial(mtl);
				}

				opengl_setTransform(
					&matWorld,
					&matView,
					&matProj);

				// opengl_setLights() {

				GLint block_index = glGetUniformBlockIndex(
					currentShader->api().object,
					"LightBlock");
				if(block_index >= 0) // only when lights are required
				{
					int lcount = 0;
					LIGHTDATA * lights = (LIGHTDATA*)glMapNamedBuffer(
								ubo->object,
								GL_WRITE_ONLY);
					for(LIGHT * l = light_next(nullptr); l != nullptr; l = light_next(l))
					{
						lights[lcount].type = l->type;
						lights[lcount].intensity = l->intensity;
						lights[lcount].arc = cos(0.5 * DEG_TO_RAD * l->arc); // arc is full arc, but cos() is half-arc
						lights[lcount].position = l->position;
						lights[lcount].direction = l->direction;
						lights[lcount].color = l->color;

						vec_normalize(&lights[lcount].direction, 1.0);
						lcount += 1;
						if(lcount >= LIGHT_LIMIT) {
							break;
						}
					}
					glUnmapNamedBuffer(ubo->object);

					GLuint binding_point_index = 2;
					glBindBufferBase(
						GL_UNIFORM_BUFFER,
						binding_point_index,
						ubo->object);
					glUniformBlockBinding(
						currentShader->api().object,
						block_index,
						binding_point_index);
					currentShader->iLightCount = lcount;
				} else {
					currentShader->iLightCount = 0;
				}
				// }
				// void opengl_setLights();

				{ // opengl_setBones()
					GLint block_index = glGetUniformBlockIndex(
						currentShader->api().object,
						"BoneBlock");
					if(block_index >= 0)
					{
						GLuint binding_point_index = 4;
						glBindBufferBase(
							GL_UNIFORM_BUFFER,
							binding_point_index,
							bonesBuf->object);
						glUniformBlockBinding(
							currentShader->api().object,
							block_index,
							binding_point_index);
					}
				}

				currentShader->vecViewPos = perspective->position;

				// shader < mtl < model < mesh < ent
				shader_setUniforms(&currentShader->api(), demote(model));
				shader_setUniforms(&currentShader->api(), mesh);
				shader_setUniforms(&currentShader->api(), ent);

				opengl_drawMesh(mesh);
			}
		}
		DebugDrawer::render(matView, matProj);
	}
}
