#pragma once
#include <sifteo.h>
#include <sifteo/math.h>
#include <sifteo/time.h>


struct Scroller {
public:
	inline void init(uint8_t maxcount, /*uint8_t step, uint8_t delta,*/ float newpos = 0) {
		this->maxcount = maxcount;
		this->max = (uint16_t)maxcount * step;
		//this->step = step;
		//this->delta = delta;
		if (newpos) position = newpos;
		velocity = 0;
		status = 1;//start interia
	}

	void update(int8_t x);

	float position = 0, velocity = 0;
	float stop_pos;//pos
	uint8_t stop_count;//count
	uint8_t status = 0;//0:stopped 1:tilting 2:inertia
	uint8_t flag = 0;//1->stopping 2->starting
	uint16_t max;
	uint8_t maxcount;
	bool locked = false;
private:
	
	const uint8_t step = 32;
	const uint8_t delta = 16;

	const float kTimeDilator = 13.1f;
	//const float kMaxSpeedMultiplier = 2.f;

	//const float kAccelThresholdOn = 4.15f * 4;
	//const float kAccelThresholdOff = 0.85f * 4;
	const int8_t kAccelThresholdOn = 16;
	const int8_t kAccelThresholdOff = 11;

	const int8_t kAccelThresholdStep = 40;
	const float kMaxSpeedMultiplier = 1.8;

	//const float stiffness = 0.333f;
	const float stiffness = 0.6;

	//const float kInertiaThreshold = 10.f;
	//const float kInertiaThreshold = 50.f;

	//const float kMaxNormalSpeed = 40.f;
	const float kMaxNormalSpeed = 35;
	const float kOneG = 64 * 0.25 * 10;

	inline bool isTiltingAtEdge(int8_t x) {
		return ((x < 0 && position >= max) || (x > 0 && position <= 0));
	}
	inline float velocityMultiplier(int8_t x) {
		return Sifteo::abs(x) > kAccelThresholdStep ? (1.f * kMaxSpeedMultiplier) : 1.f;
	}
	inline float maxVelocity(int8_t x) {
		return kMaxNormalSpeed / kOneG * Sifteo::abs(x) * velocityMultiplier(x);
	}
	inline float lerp(float min, float max, float u) {
		return min + u * (max - min);
	}
	inline void getStopPos() {
		stop_count = (int8_t)((position - delta) / step + 0.5);
		if (stop_count < 0) stop_count = 0;
		else if (stop_count >= maxcount) stop_count = maxcount - 1;
		stop_pos = delta + step * stop_count;
	}
};
