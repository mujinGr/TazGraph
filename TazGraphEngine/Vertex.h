#pragma once

#include "GL/glew.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

static glm::mat4 getRotationMatrix(glm::vec3 newRot) {
	float radX = glm::radians(newRot.x);
	float radY = glm::radians(newRot.y);
	float radZ = glm::radians(newRot.z);

	glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), radX, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), radY, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 rotationZ = glm::rotate(glm::mat4(1.0f), radZ, glm::vec3(0.0f, 0.0f, 1.0f));

	glm::mat4 rotationMatrix = rotationZ * rotationY * rotationX;
	return rotationMatrix;
}

static glm::vec2 rotatePoint(float x, float y, float centerX, float centerY, float radians) {
	float dx = x - centerX;
	float dy = y - centerY;
	return glm::vec2(
		centerX + dx * cos(radians) - dy * sin(radians),
		centerY + dx * sin(radians) + dy * cos(radians)
	);
};
static glm::vec2 rotatePoint(float x, float y, float radians) {
	return rotatePoint(x,y, 0.0f, 0.0f, radians);
};
/*	glm::vec3 arotatedTopLeft = rotatePoint(atopLeft.x, atopLeft.y, atopLeft.z, centerX, centerY, centerZ, 0, 0, 0);
		glm::vec3 arotatedBottomLeft = rotatePoint(abottomLeft.x, abottomLeft.y, abottomLeft.z, centerX, centerY, centerZ, 0, 0, 0);
		glm::vec3 arotatedBottomRight = rotatePoint(abottomRight.x, abottomRight.y, abottomRight.z, centerX, centerY, centerZ, 0, 0, 0);
		glm::vec3 arotatedTopRight = rotatePoint(atopRight.x, atopRight.y, atopRight.z, centerX, centerY, centerZ, 0, 0, 0);*/


using Position	= glm::vec3;
using Size		= glm::vec3;
using Rotation	= glm::vec3;

using UV		= glm::vec2;

struct Color {
	Color() : r(0), g(0), b(0), a(0) {}
	Color(GLubyte R, GLubyte G, GLubyte B, GLubyte A) : r(R), g(G), b(B), a(A) {}
	
	glm::vec4 toVec4() const {
		return glm::vec4(r, g, b, a) / 255.0f; // Normalize 0-255 to 0-1
	}

	static Color fromVec4(const glm::vec4& v) {
		return Color(
			static_cast<GLubyte>(v.r * 255.0f),
			static_cast<GLubyte>(v.g * 255.0f),
			static_cast<GLubyte>(v.b * 255.0f),
			static_cast<GLubyte>(v.a * 255.0f)
		);
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


struct ColorVertex { //instead of using the general Vertex that has also info about texture
	// we use this where we want just color
	Position position = Position(0);
	//todo different instanceVBO for the centers
	//Position centerMesh;
	Color color = Color();

	inline void setPosition(Position m_position) {
		position = m_position;
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
	UV uv = UV(0);

	inline void setUV(UV m_uv) {
		uv = m_uv;
	}
};