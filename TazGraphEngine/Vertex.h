#pragma once

#include "GL/glew.h"

struct Position {
	float x;
	float y;
	float z;
};

struct Color {
	Color() : r(0), g(0), b(0), a(0) {}
	Color(GLubyte R, GLubyte G, GLubyte B, GLubyte A) : r(R), g(G), b(B), a(A) {}
	
	glm::vec4 toVec4() const {
		return glm::vec4(r, g, b, a) / 255.0f; // Normalize 0-255 to 0-1
	}

	static Color fromVec4(const glm::vec4& v) {
		return Color(v.r * 255, v.g * 255, v.b * 255, v.a * 255);
	}

	GLubyte r;
	GLubyte g;
	GLubyte b;
	GLubyte a;

	Color operator*(float scalar) const {
		return Color(static_cast<GLubyte>(r * scalar),
			static_cast<GLubyte>(g * scalar),
			static_cast<GLubyte>(b * scalar),
			static_cast<GLubyte>(a * scalar));
	}

	// Operator overload for color addition
	Color operator+(const Color& other) const {
		return Color(static_cast<GLubyte>(r + other.r),
			static_cast<GLubyte>(g + other.g),
			static_cast<GLubyte>(b + other.b),
			static_cast<GLubyte>(a + other.a));
	}

	bool operator==(const Color& other) const {
        return r == other.r && g == other.g && b == other.b && a == other.a;
    }
};

struct UV {
	float u;
	float v;
};

struct ColorVertex { //instead of using the general Vertex that has also info about texture
	// we use this where we want just color
	Position position;
	Color color;

	void setPosition(float x, float y, float z = -5.0f) {
		position.x = x;
		position.y = y;
		position.z = z;
	}

	void setColor(GLubyte r, GLubyte g, GLubyte b, GLubyte a) {
		color.r = r;
		color.g = g;
		color.b = b;
		color.a = a;
	}
};

struct Vertex  : ColorVertex{
	// UV texture coordinates
	UV uv;

	void setUV(float u, float v) {
		uv.u = u;
		uv.v = v;
	}
};