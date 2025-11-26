#pragma once

#include "Queen.h"
namespace relation
{
	typedef bool (*relationFunc)(Position p1, Position p2);
	bool conflict(Position p1, Position p2);
}
