#pragma once

#include "Animation.h"

#define NUM_LOOP_STATES 3
#define NUM_BACK_FORTH_STATES 4


struct FlashAnimation : public Animation //todo moving animation can be moving sprite with of without transform
{
	enum class FlashState {
		FLASH_OUT,
		EASE_IN,
		FLASH_IN,
		EASE_OUT
	};

	float interpolation_a = 0.0f;
	std::map<FlashState, float> speeds;

	FlashState currentSpeedIndex = FlashState::FLASH_OUT;
	Color flashColor = Color(255,255,255,255);

	FlashAnimation() : Animation()
	{

	}
	// ix,iy is initial position (destX, destY), f is total frames to move, s is the speed to move frames, type as in animation, dx,dy distance to move
	FlashAnimation(int ix, int iy, size_t f, float s, const std::string _type,
		const std::vector<float>& flashTimes, Color flashC, int _reps = 0) : Animation(ix, iy, f, s, _type) // Animation frames look the next number of frames from the index
	{
		speeds[FlashState::FLASH_OUT] = flashTimes[0];
		speeds[FlashState::EASE_IN] = flashTimes[1];
		speeds[FlashState::FLASH_IN] = flashTimes[2];
		speeds[FlashState::EASE_OUT] = flashTimes[3];
		flashColor = flashC;
		reps = _reps;
	}

	FlashAnimation(int ix, int iy, size_t f, float s, const animType _type,
		const std::vector<float>& flashTimes, Color flashC, int _reps = 0) : Animation(ix, iy, f, s, _type) // Animation frames look the next number of frames from the index
	{
		speeds[FlashState::FLASH_OUT] = flashTimes[0];
		speeds[FlashState::EASE_IN] = flashTimes[1];
		speeds[FlashState::FLASH_IN] = flashTimes[2];
		speeds[FlashState::EASE_OUT] = flashTimes[3];
		flashColor = flashC;
		reps = _reps;
	}

	void advanceFrame(float deltaTime) {
		unsigned short prev_frame_index = cur_frame_index;

		speed = speeds[currentSpeedIndex];
		switch (currentSpeedIndex) {
		case FlashState::FLASH_OUT:
			interpolation_a = 0;
			break;
		case FlashState::EASE_IN:
			// Using simple subtraction to find the fractional part
			interpolation_a = cur_frame_index_f - (int)cur_frame_index_f;
			break;
		case FlashState::FLASH_IN:
			interpolation_a = 1;
			break;
		case FlashState::EASE_OUT:
			// Using simple subtraction and inversion for the fractional part
			interpolation_a = 1 - (cur_frame_index_f - (int)cur_frame_index_f);
			break;
		}
		switch (type) {
		case Animation::animType::ANIMTYPE_BACK_FORTH:
			
			cur_frame_index_f += speed * deltaTime;
			cur_frame_index = static_cast<unsigned short>(cur_frame_index_f);
				
			if (cur_frame_index >= NUM_BACK_FORTH_STATES) {
				times_played++;
				resetFrameIndex();
				if (reps && times_played >= reps) {
					finished = true;
				}
			}
			// Check if the frame index has changed
			if (prev_frame_index != cur_frame_index) {
				frame_times_played = 1;
				if ((static_cast<int>(currentSpeedIndex) + 1) % NUM_BACK_FORTH_STATES < NUM_BACK_FORTH_STATES)
					currentSpeedIndex = static_cast<FlashState>((static_cast<int>(currentSpeedIndex) + 1) % speeds.size());
			}
			else {
				frame_times_played++;
			}
			break;
		case Animation::animType::ANIMTYPE_LOOPED:
		case Animation::animType::ANIMTYPE_PLAY_N_TIMES:
			cur_frame_index_f += speed * deltaTime;
			cur_frame_index = static_cast<unsigned short>(cur_frame_index_f);

			// Check if the frame index has changed
			if (prev_frame_index != cur_frame_index) {
				frame_times_played = 1;
				if(	static_cast<int>(currentSpeedIndex) % NUM_LOOP_STATES < NUM_LOOP_STATES)
					currentSpeedIndex = static_cast<FlashState>(static_cast<int>(currentSpeedIndex) % NUM_LOOP_STATES);
			}
			else {
				frame_times_played++;
			}

			if (cur_frame_index >= NUM_LOOP_STATES) //essentially when we see that now we reach a frame out of total frames we reset it
			{
				resetFrameIndex();
				times_played++;
				if (reps && times_played >= reps) {
					finished = true;
				}
			}
			break;

		case Animation::animType::ANIMTYPE_NONE:
			break;
		}
	}

	std::vector<float> getSpeedsAsVector() const {
		std::vector<float> speedsVector(4);  // Create a vector of size 4

		// Ensure the vector fills according to the FlashState order
		speedsVector[0] = speeds.at(FlashState::FLASH_OUT);  // Use at() for direct access with bounds checking
		speedsVector[1] = speeds.at(FlashState::EASE_IN);
		speedsVector[2] = speeds.at(FlashState::FLASH_IN);
		speedsVector[3] = speeds.at(FlashState::EASE_OUT);

		return speedsVector;
	}

};