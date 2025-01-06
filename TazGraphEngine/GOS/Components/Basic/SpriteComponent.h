#pragma once

#include "../../Components.h"
#include <SDL2/SDL.h>
#include "GL/glew.h"
#include "../../../TextureManager/TextureManager.h"
#include "../../Animators/Animation.h"
#include "../../Animators/MovingAnimation.h"
#include "../../Animators/FlashAnimation.h"
#include <map>
#include "../../../Vertex.h"
#include <cstddef>


// TODO: (extra): can add states for different states (0 for full solid tile or 1 for no solid
class SpriteComponent : public Component //sprite -> transform
{
private:
	const GLTexture *gl_texture;
	GLuint _vboID = 0; //32 bits
	bool _isMainMenu = false;

public:
	Color default_color = { 255, 255, 255, 255 };
	Color color = { 255, 255, 255, 255 };

	TransformComponent* transform = nullptr;
	SDL_Rect srcRect, destRect;

	Animation animation;
	MovingAnimation moving_animation;
	FlashAnimation flash_animation;
	
	SDL_RendererFlip spriteFlip = SDL_FLIP_NONE;

	SpriteComponent() = default;
	SpriteComponent(std::string id)
	{
		setTex(id);
	}
	SpriteComponent(Color clr)
	{
		default_color = clr;
		color = clr;
	}

	SpriteComponent(std::string id, bool isMainMenu)
	{
		_isMainMenu = isMainMenu;
		setTex(id);
	}

	~SpriteComponent()
	{
		if (_vboID != 0) //buffer hasn't been generated
		{
			glDeleteBuffers(1, &_vboID); // create buffer and change vboID to point to that buffer
		}
		//SDL_DestroyTexture(texture); //no need for that anymore, because sprite points to a texture that could be used by multiple objects
	}

	void setTex(std::string id) //this function is used to change texture of a sprite
	{
		gl_texture = TextureManager::getInstance().Get_GLTexture(id);
	}

	void init() override
	{
		if (!entity->hasComponent<TransformComponent>())
		{
			entity->addComponent<TransformComponent>();
		}
		transform = &entity->GetComponent<TransformComponent>();
		
		srcRect.x = srcRect.y = 0;
		srcRect.w = transform->width;
		srcRect.h = transform->height;

		destRect.x = static_cast<int>(transform->getPosition().x); //make player move with the camera, being stable in centre, except on edges
		destRect.y = static_cast<int>(transform->getPosition().y);

		destRect.w = transform->width * transform->scale;
		destRect.h = transform->height * transform->scale;


	}

	void update(float deltaTime) override
	{
		destRect.x = static_cast<int>(transform->getPosition().x); //make player move with the camera, being stable in centre, except on edges
		destRect.y = static_cast<int>(transform->getPosition().y);
	}

	void draw(SpriteBatch&  batch, TazGraphEngine::Window& window) override
	{
		if (gl_texture == NULL)
		{
			return;
		}
		float tempScreenScale = window.getScale();
		glm::vec4 pos((float)destRect.x* tempScreenScale, (float)destRect.y * tempScreenScale,
			(float)destRect.w * tempScreenScale, (float)destRect.h * tempScreenScale);

		float srcUVposX = spriteFlip == SDL_FLIP_HORIZONTAL ?
			(float)(srcRect.x + srcRect.w) / (float)gl_texture->width :
			(float)srcRect.x / (float)gl_texture->width;
		float srcUVposY = ((float)srcRect.y) / (float)gl_texture->height;

		float srcUVw = spriteFlip == SDL_FLIP_HORIZONTAL ?
			-(float)srcRect.w / (float)gl_texture->width :
			(float)srcRect.w / (float)gl_texture->width;
		float srcUVh = (float)srcRect.h / (float)gl_texture->height;

		glm::vec4 uv(srcUVposX, srcUVposY, srcUVw, srcUVh);

		batch.draw(pos, uv, gl_texture->id, transform->getZIndex(), color, transform->getRotation());
		
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		/*glBindTexture(GL_TEXTURE_2D, 0);*/
	}

	void SetAnimation(int idX, int idY, int fr, float sp, const Animation::animType type, int reps = 0)
	{
		animation = Animation(idX, idY, fr, sp, type, reps);
	}

	void SetMovingAnimation(int idX, int idY, int fr, float sp, const Animation::animType type, const std::vector<glm::vec2>& _positions, const std::vector<int>& _zIndices, const std::vector<int>& _rotations, int reps = 0)
	{
		moving_animation = MovingAnimation(idX, idY, fr, sp, type, _positions, _zIndices, _rotations, reps); // dx,dy needs to be vector, if yes then dont need int dx dy
	}

	void SetFlashAnimation(int idX, int idY, int fr, float sp, const Animation::animType type, const std::vector<float>& flashTimes, Color flashC, int reps = 0)
	{
		flash_animation = FlashAnimation(idX, idY, fr, sp, type, flashTimes, flashC, reps);
	}

	void setCurrFrame() {
		this->srcRect.x = (this->animation.indexX * this->transform->width) /* init */ + ( this->srcRect.w * animation.cur_frame_index/* curframe from total frams */);
		this->srcRect.y = this->animation.indexY * this->transform->height;
	}

	void setMoveFrame() {

		this->destRect.x = (static_cast<int>(this->transform->getPosition().x) + this->moving_animation.indexX) /* init */ + (this->moving_animation.positions[0].x * moving_animation.cur_frame_index);
		this->destRect.y = (static_cast<int>(this->transform->getPosition().y) + this->moving_animation.indexY) + (this->moving_animation.positions[0].y * moving_animation.cur_frame_index);
	}

	void setSpecificMoveFrame() {

		this->destRect.x = (static_cast<int>(this->transform->getPosition().x) + this->moving_animation.indexX) /* init */ + (this->moving_animation.positions[moving_animation.cur_frame_index].x);
		this->destRect.y = (static_cast<int>(this->transform->getPosition().y) + this->moving_animation.indexY) + (this->moving_animation.positions[moving_animation.cur_frame_index].y);
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
};