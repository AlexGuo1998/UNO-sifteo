#ifdef __VSFIX
#include "vsfix.h"
#endif

#include "scroller.h"

using namespace Sifteo;

extern TimeStep g_frameclock;

void Scroller::update(int8_t x) {
	flag = 0;
	//x = x_in;
	//check state
	switch (status) {
	case 0://stopped
		if (locked || abs(x) < kAccelThresholdOn || isTiltingAtEdge(x)) {
			return;
		}
		//else: start
		status = 1;
		flag = 2;
		//fall through
	case 1://tilting
	c1:		
		if (!locked && abs(x) >= kAccelThresholdOff) {
			velocity += (-(float)x / 4 * g_frameclock.delta() * kTimeDilator) * velocityMultiplier(x);

			// clamp maximum velocity based on cube angle
			if (abs(velocity) > maxVelocity(x)) {
				velocity = (velocity < 0 ? 0 - maxVelocity(x) : maxVelocity(x));
			}

			position += velocity * g_frameclock.delta() * kTimeDilator;

			// don't go past the backstop
			if (position >= max || position <= 0) {
				position = (position >= max) ? max : 0;
				velocity = 0;
			} else {
				return;
			}
		}
		status = 2;

		getStopPos();
		//fall through
	case 2://inertia
		// do not pull to item unless tilting has stopped.
		if (locked || abs(x) < kAccelThresholdOff) {
			velocity += stop_pos - position;
			velocity *= stiffness;
			position += velocity * g_frameclock.delta() * kTimeDilator;
			position = lerp(position, stop_pos, 0.15f);

			if (abs(velocity) < 1.0f && abs(stop_pos - position) < 0.5f) {
				// prevent being off by one pixel when we stop
				position = stop_pos;
				status = 0;
				flag = 1;
			}

		} else {
			if (isTiltingAtEdge(x)) {
				return;
			} else {
				status = 1;
				goto c1;
				//goto mode 1
			}
		}
	}
}

