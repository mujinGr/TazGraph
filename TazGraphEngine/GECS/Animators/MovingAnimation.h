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
	std::vector<glm::vec2> positions;  // Stores the positions for the animation
	std::vector<int> zIndices;         // zIndex for each position
	std::vector<int> rotations;

	MovingAnimation() : Animation()
	{

	}
	// ix,iy is initial position (destX, destY), f is total frames to move, s is the speed to move frames, type as in animation, dx,dy distance to move
	MovingAnimation(int ix, int iy, int f, float s, const std::string _type, int dx, int dy, int _reps = 0) : Animation(ix, iy, f, s, _type) // Animation frames look the next number of frames from the index
	{
		positions.clear();
		zIndices.clear();
		rotations.clear();

		glm::vec2 distanceXY(dx, dy);
		positions.push_back(distanceXY);
		zIndices.push_back(0);
		rotations.push_back(0);

		reps = _reps;
	}

	MovingAnimation(int ix, int iy, int f, float s, const animType _type, int dx, int dy, int _reps = 0) : Animation(ix, iy, f, s, _type) // Animation frames look the next number of frames from the index
	{
		positions.clear();
		zIndices.clear();
		rotations.clear();

		glm::vec2 distanceXY(dx, dy);
		positions.push_back(distanceXY);
		zIndices.push_back(0);
		rotations.push_back(0);
		
		reps = _reps;
	}

	MovingAnimation(
		int ix, int iy, int f, float s, const std::string _type, const std::vector<glm::vec2>& _positions, const std::vector<int>& _zIndices, const std::vector<int>& _rotations, int _reps = 0) // Animation frames look the next number of frames from the index
	{
		positions.clear();
		zIndices.clear();
		rotations.clear();

		positions = _positions;
		zIndices = _zIndices;
		rotations = _rotations;

		Animation(ix, iy, positions.size(), s, _type);

		reps = _reps;
		validateVectors();
	}

	MovingAnimation(
		int ix, int iy, int f, float s, const animType _type, const std::vector<glm::vec2>& _positions, const std::vector<int>& _zIndices, const std::vector<int>& _rotations, int _reps = 0)  // Animation frames look the next number of frames from the index
	{
		positions.clear();
		zIndices.clear();
		rotations.clear();

		positions = _positions;
		zIndices = _zIndices;
		rotations = _rotations;

		Animation(ix, iy, positions.size(), s, _type);

		reps = _reps;
		validateVectors();
	}

private:
	void validateVectors() const {
		// Ensure all vectors are of the same length to prevent indexing errors
		if (positions.size() != zIndices.size() || positions.size() != rotations.size()) {
			throw std::invalid_argument("All vectors (positions, zIndices, rotations) must have the same length.");
		}
	}
};