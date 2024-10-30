#include "e_collision.hpp"



int nthp::entity::checkRectCollision(nthp::entity::cRect a, nthp::entity::cRect b) {
	if (a.x + a.w >= b.x &&
		a.x <= b.x + b.w &&
		a.y + a.h >= b.y &&
		a.y <= b.y + b.h) 
	{ return 1; }
			
	return 0;
}
