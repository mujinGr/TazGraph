#pragma once

#include "Components.h"

class LightComponent : public Component {
public:
    Color color;
    float radius = 0.0f;

    SDL_Rect destRect;
    TransformComponent* transform = nullptr;

    LightComponent()
    {
    }

    ~LightComponent() {

    }

    void init() override {
        transform = &entity->GetComponent<TransformComponent>();

        destRect.w = transform->width * transform->scale;
        destRect.h = transform->height * transform->scale;
    }

    void update(float deltaTime) override {
        destRect.x = static_cast<int>(transform->getPosition().x); //make player move with the camera, being stable in centre, except on edges
        destRect.y = static_cast<int>(transform->getPosition().y);
    }

    void draw(SpriteBatch&  batch, TazGraphEngine::Window& window) override
    {
        float tempScreenScale = window.getScale();
        glm::vec4 pos((float)destRect.x * tempScreenScale, (float)destRect.y * tempScreenScale,
            (float)destRect.w * tempScreenScale, (float)destRect.h * tempScreenScale);
        batch.draw(pos, glm::vec4(-1.0f, -1.0f, 2.0f, 2.0f), NULL, transform->getZIndex(), color, transform->getRotation());
	}
};