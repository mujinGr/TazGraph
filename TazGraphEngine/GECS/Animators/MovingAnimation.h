#pragma once

#include "Animation.h"
#include <vector>
#include <glm/glm.hpp>
#include <stdexcept>

class MovingAnimation : public Animation //todo moving animation can be moving sprite with of without transform
{
public:
	// have a vector for the positions
	// when given only dx and dy then add that to the same vector as first element
	// sprite will check bool if it is about continious 

	glm::vec3 startingPosition = glm::vec3(0);
	glm::vec3 distance = glm::vec3(0);  // Stores the positions for the animation
	glm::vec3 dest_rotation = glm::vec3(0);

	MovingAnimation() : Animation()
	{

	}
	// ix,iy is initial position (destX, destY), f is total frames to move, s is the speed to move frames, type as in animation, dx,dy distance to move
	MovingAnimation(glm::vec3 m_startPos, int f, float s, const std::string _type, glm::vec3 m_distance, int _reps = 0)
		: Animation(0, 0, f, s, _type) // Animation frames look the next number of frames from the index
	{
		startingPosition = m_startPos;
		distance = m_distance;

		reps = _reps;
	}

	MovingAnimation(glm::vec3 m_startPos, int f, float s, const animType _type, glm::vec3 m_distance, int _reps = 0)
		: Animation(0, 0, f, s, _type) // Animation frames look the next number of frames from the index
	{
		startingPosition = m_startPos;

		distance = m_distance;

		reps = _reps;
	}

	MovingAnimation(
		glm::vec3 m_startPos, size_t f, float s, const animType _type, glm::vec3 m_distance, glm::vec3 m_dest_rotation, int _reps = 0)
		: Animation(0, 0, f, s, _type)// Animation frames look the next number of frames from the index
	{
		startingPosition = m_startPos;

		distance = m_distance;
		dest_rotation = m_dest_rotation;

		reps = _reps;
	}
};