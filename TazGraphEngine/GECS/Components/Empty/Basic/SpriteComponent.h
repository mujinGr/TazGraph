#pragma once

#include "../../../Components.h"
#include <SDL2/SDL.h>
#include "GL/glew.h"
#include "../../../../TextureManager/TextureManager.h"
#include "../../../Animators/Animation.h"

#include "../../../Animators/FlashAnimation.h"
#include <map>
#include "../../../../Vertex.h"
#include <cstddef>


// TODO: (extra): can add states for different states (0 for full solid tile or 1 for no solid
class SpriteComponent : public Component //sprite -> transform
{
private:
	const GLTexture* gl_texture = nullptr;

public:
	std::string texture_name = "";
	TazColor default_color = { 255, 255, 255, 255 };
	TazColor color = { 255, 255, 255, 255 };

	TransformComponent* transform = nullptr;
	SDL_FRect srcRect = { 0,0,0,0 };

	Animation animation;
	FlashAnimation flash_animation;

	SDL_RendererFlip spriteFlip = SDL_FLIP_NONE;


	int currentItem = 0;

	SpriteComponent() = default;
	SpriteComponent(std::string id)
	{
		setTex(id);
	}
	SpriteComponent(TazColor clr)
	{
		default_color = clr;
		color = clr;
	}

	~SpriteComponent()
	{
	}

	void setTex(std::string id) //this function is used to change texture of a sprite
	{
		texture_name = id;
		gl_texture = TextureManager::getInstance().Get_GLTexture(id);
		srcRect.w = (float)gl_texture->width;
		srcRect.h = (float)gl_texture->height;
	}

	void init() override
	{
		if (!entity->hasComponent<TransformComponent>())
		{
			entity->addComponent<TransformComponent>();
		}
		transform = &entity->GetComponent<TransformComponent>();

		srcRect.x = srcRect.y = 0;
		srcRect.w = transform->size.x;
		srcRect.h = transform->size.y;

	}

	void update(float deltaTime) override
	{
	}

	void draw(size_t v_index, PlaneModelRenderer& batch, TazGraphEngine::Window& window)
	{
		if (gl_texture == NULL)
		{
			return;
		}

		float screenScale = window.getScale();

		glm::vec3 pos(
			transform->getPosition().x * screenScale,
			transform->getPosition().y * screenScale,
			transform->getPosition().z);

		glm::vec2 size(
			transform->size.x * transform->scale * screenScale,
			transform->size.y * transform->scale * screenScale);

		float srcUVposX = spriteFlip == SDL_FLIP_HORIZONTAL ?
			(srcRect.x + srcRect.w) / gl_texture->width :
			srcRect.x / gl_texture->width;
		float srcUVposY = (srcRect.y) / gl_texture->height;

		float srcUVw = spriteFlip == SDL_FLIP_HORIZONTAL ?
			-srcRect.w / gl_texture->width :
			srcRect.w / gl_texture->width;
		float srcUVh = srcRect.h / gl_texture->height;

		glm::vec4 uv(srcUVposX, srcUVposY, srcUVw, srcUVh);

		batch.draw(v_index, size, transform->getPosition(), transform->rotation, uv, gl_texture->id);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		/*glBindTexture(GL_TEXTURE_2D, 0);*/
	}

	void SetAnimation(int idX, int idY, size_t fr, float sp, const Animation::animType type, int reps = 0)
	{
		animation = Animation(idX, idY, fr, sp, type, reps);
	}

	void SetFlashAnimation(size_t fr, float sp, const Animation::animType type, const std::vector<float>& flashTimes, TazColor flashC, int reps = 0)
	{
		flash_animation = FlashAnimation(fr, sp, type, flashTimes, flashC, reps);
	}

	void setCurrFrame() {
		this->srcRect.x = (this->animation.indexX * this->transform->size.x) /* init */ + (this->srcRect.w * animation.cur_frame_index/* curframe from total frams */);
		this->srcRect.y = this->animation.indexY * this->transform->size.y;
	}

	void setFlashFrame() {
		this->color = this->flash_animation.flashColor * this->flash_animation.interpolation_a
			+ default_color * (1 - this->flash_animation.interpolation_a);
	}

	void DestroyTex()
	{
		//TextureManager::DestroyTexture(texture);
		gl_texture = nullptr;
	}
	void DestroyGlTex()
	{
		gl_texture = NULL;
	}

	std::string GetComponentName() override {
		return "SpriteComponent";
	}

	void showGUI(std::vector<BaseComponent*> otherComponents = {}) override {
		ImGui::Separator();

		// Get the list of texture names
		std::vector<std::string> textureNames = TextureManager::getInstance().Get_GLTextureNames();

		if (!textureNames.empty()) {
			std::vector<const char*> items;
			for (const std::string& name : textureNames)
				items.push_back(name.c_str());

			if (ImGui::Combo("Textures", &currentItem, items.data(), (int)items.size())) {
				if (!textureNames[currentItem].empty()) {
					setTex(textureNames[currentItem]);
				}
			}
		}
	};
};