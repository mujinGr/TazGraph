#pragma once

#include "../../../Components.h"
#include "../../../UtilComponents.h"
#include <functional>  // For std::function

class ButtonComponent : public Component {
public:
    enum class ButtonState {
        NORMAL,
        HOVERED,
        PRESSED
    };

private:
    ButtonState _state = ButtonState::NORMAL;
    std::function<void()> _onClick; // Callback function for click action

    std::string buttonLabel = "";
    glm::vec2 bDimensions{0,0};
    Color bBackground{255,255,255,255};

    Entity* uiLabel = nullptr;
    Entity* buttonBackground = nullptr;
public:
    ButtonComponent()
        : _state(ButtonState::NORMAL),
        _onClick([]() {}) // Default empty function
    {
    }
    ButtonComponent(std::function<void()> onClick)
        : _state(ButtonState::NORMAL), _onClick(onClick) {}

    ButtonComponent(std::function<void()> onClick, std::string button_label, glm::vec2 b_dimensions, Color b_background)
        : _state(ButtonState::NORMAL),
        _onClick(onClick),
        buttonLabel(button_label),
        bDimensions(b_dimensions),
        bBackground(b_background){
           // create UI label with the string given
           // set the button background 
    }

    void init() override {
        if (buttonLabel.length() > 0) {
            // need different shader to render text so it has to be different 
            uiLabel = &entity->getManager()->addEntity<Node>();
            
            uiLabel->addComponent<TransformComponent>(glm::vec2(0, 0), Layer::action, glm::ivec2(32, 32), 1);
            uiLabel->addComponent<UILabel>(uiLabel->getManager(), buttonLabel, "arial");

            uiLabel->setParentEntity(entity);

            uiLabel->addGroup(Manager::buttonLabels);
        }
        if (bDimensions != glm::vec2(0.0f, 0.0f)) {
            buttonBackground = &entity->getManager()->addEntity<Node>();

            buttonBackground->addComponent<TransformComponent>(glm::vec2(0, 0), Layer::action, bDimensions, 1);
            buttonBackground->addComponent<Rectangle_w_Color>();

            buttonBackground->GetComponent<Rectangle_w_Color>().color = bBackground; // Grey color
            buttonBackground->setParentEntity(entity);

            buttonBackground->addGroup(Manager::panelBackground);
        }
    }

    void setOnClick(std::function<void()> newOnClick) {
        _onClick = newOnClick;
    }

    void setState(ButtonState state) {
        _state = state;
    }

    void update(float deltaTime) override {
        // Update the button state based on user input
        // Change the texture or appearance based on the state
        // ...

        if (_state == ButtonState::PRESSED && _onClick) {
            _onClick(); // Call the click action callback
        }
        setState(ButtonState::NORMAL); // Reset the state
    }

    // Other methods for setting textures, handling mouse events, etc.

    std::string GetComponentName() override {
        return "ButtonComponent";
    }
};