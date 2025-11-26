#pragma once

#include "Csp.h"
#include "Queen.h"

namespace inference
{
	bool canSatisfy(Csp& csp, Position& p1, Queen& q2);
	bool revise(Csp& csp, Queen& q1, Queen& q2);
	bool ac3(Csp& csp);
}