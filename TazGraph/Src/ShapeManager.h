#pragma once

#include "ECS/ECSManager.h"
#include <SDL2/SDL_ttf.h>
#include <glm/glm.hpp>

class ShapeManager {
public:
    static void DrawCircle(int32_t centreX, int32_t centreY, int32_t radius, SDL_Color color)
    {
        std::vector<GLfloat> vertices;

        // Define the circle in terms of vertices
        const int NUM_SEGMENTS = 100; // Increase for smoother circle
        for (int i = 0; i < NUM_SEGMENTS; i++) {
            float theta = 2.0f * 3.1415926f * float(i) / float(NUM_SEGMENTS); // Current angle

            float x = radius * cosf(theta); // Calculate the x coordinate
            float y = radius * sinf(theta); // Calculate the y coordinate

            vertices.push_back(x + centreX);
            vertices.push_back(y + centreY);
        }

        // Set the color
        glColor4ub(color.r, color.g, color.b, color.a);

        // Enable client states
        glEnableClientState(GL_VERTEX_ARRAY);

        // Point to the vertices to be used
        glVertexPointer(2, GL_FLOAT, 0, &vertices[0]);

        // Draw the circle
        glDrawArrays(GL_LINE_LOOP, 0, NUM_SEGMENTS);

        // Disable client states
        glDisableClientState(GL_VERTEX_ARRAY);
    }
};
