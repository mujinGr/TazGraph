#pragma once
#include "../../Components.h"

class PollingComponent : public Component {
public:
    void StartPolling(const std::string& file, float delayInSeconds) {
        pollingFile = file;
        timer = delayInSeconds;
        pollingActive = true;
    }

    void update(float deltaTime) override {
        if (!pollingActive) return;

        timer -= deltaTime / 10.0f;
        if (timer <= 0.0f) {
            SendMessageToOtherNodes(pollingFile);
            pollingActive = false; // Stop polling after sending the message
        }
    }

private:
    void SendMessageToOtherNodes(const std::string& file) {
        std::cout << "Polling complete. Sending message from file: " << file << std::endl;

       // get outLinks of Node, fine the node with the id, and send a message saying "Hello World"
       // keep log in the nodes where it shows the messages received
        if (!entity->getOutLinks().empty()) {
            for (auto& link : entity->getOutLinks()) {
                link->getToNode()->addMessage("Hello World");
                

                link->GetComponent<LineFlashAnimatorComponent>().Play("LineTransfer");
            }
        }
        // entity->makeAnimation(0.01f)

    }

    std::string pollingFile;
    float timer = 0.0f;
    bool pollingActive = false;
};