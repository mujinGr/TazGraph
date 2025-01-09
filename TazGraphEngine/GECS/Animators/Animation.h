#pragma once

#include <string>

struct Animation //todo for now just add a bool hasFinished (useful for scripts) and much later may need onStop(when game paused)
{					// todo also we might need to have setReps/getReps and replace "play_once" with "play_n_times" where we pass 1 more value to call
	typedef enum {
		ANIMTYPE_NONE = 0, // just one animation
		ANIMTYPE_PLAY_N_TIMES = 1, // just iterates over the images one time. it holds the final image when finished.
		ANIMTYPE_LOOPED = 2, // going over the images again and again.
		ANIMTYPE_BACK_FORTH = 3  // iterate from index=0 to maxframe and back again. keeps holding the first image afterwards.
	} animType;

	int indexX = 0; // initial position
	int indexY = 0;
	int total_frames = 0;
	float speed = 1.0f;
	animType type = animType::ANIMTYPE_NONE;
	int reps = 0;

	int frame_times_played = 0;
	int cur_frame_index = 0;
	float cur_frame_index_f = 0;
	int times_played = 0;

	int flow_direction = 1;

	bool finished = false;

	Animation() 
	{
		
	}

	Animation(int ix, int iy , int f, float s, const std::string _type, int _reps = 0) // Animation frames look the next number of frames from the index
	{
		indexX = ix;
		indexY = iy;
		total_frames = f;
		speed = s;

		type = _type == "play_n_times" ? ANIMTYPE_PLAY_N_TIMES :
			_type == "back_forth" ? ANIMTYPE_BACK_FORTH :
			_type == "looped" ? ANIMTYPE_LOOPED :
			ANIMTYPE_NONE;
		reps = _reps;
	}

	Animation(int ix, int iy, int f, float s, const animType _type, int _reps = 0) // Animation frames look the next number of frames from the index
	{
		indexX = ix;
		indexY = iy;
		total_frames = f;
		speed = s;

		type = _type;
		reps = _reps;
	}

	void advanceFrame(float deltaTime) {
		unsigned short prev_frame_index = cur_frame_index;

		switch (type) {
		case Animation::animType::ANIMTYPE_LOOPED:
		case Animation::animType::ANIMTYPE_PLAY_N_TIMES:
			cur_frame_index_f += speed * deltaTime;
			cur_frame_index = static_cast<unsigned short>(cur_frame_index_f);

			// Check if the frame index has changed
			if (prev_frame_index != cur_frame_index) {
				frame_times_played = 1;
			}
			else {
				frame_times_played++;
			}

			if (cur_frame_index > total_frames - 1) //essentially when we see that now we reach a frame out of total frames we reset it
			{
				resetFrameIndex();
				times_played++;
				if (reps && times_played >= reps) {
					finished = true;
				}				
			}
			break;

		case Animation::animType::ANIMTYPE_BACK_FORTH:
			if (flow_direction == 1) {
				cur_frame_index_f += speed * deltaTime;

				if (cur_frame_index_f > total_frames) {
					cur_frame_index_f -= speed;
					flow_direction = -1;
				}
				cur_frame_index = static_cast<unsigned short>(cur_frame_index_f);
			}
			else if (flow_direction == -1) {
				if (cur_frame_index > 0) {
					cur_frame_index_f -= speed * deltaTime;
					cur_frame_index = static_cast<unsigned short>(cur_frame_index_f);
				}
				else {
					times_played++;
					flow_direction = 1;
					resetFrameIndex();
					if (reps && times_played >= reps) {
						finished = true;
					}
				}
			}
			// Check if the frame index has changed
			if (prev_frame_index != cur_frame_index) {
				frame_times_played = 1;
			}
			else {
				frame_times_played++;
			}
			break;

		case Animation::animType::ANIMTYPE_NONE:
			break;
		}
	}

	void resetFrameIndex() {
		cur_frame_index = 0;
		cur_frame_index_f = 0;
		frame_times_played = 0;
	}

	bool hasFinished() {
		return finished;
	}
};