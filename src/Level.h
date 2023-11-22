#ifndef LEVEL_H
#define LEVEL_H

#include "types.h"

inline s32
compute_points(s32 level, s32 line_count)
{
	switch (line_count)
	{
	case 1: return 40 * (level + 1);
		break;
	case 2: return 100 * (level + 1);
		break;
	case 3: return 300 * (level + 1);
		break;
	case 4: return 1200 * (level + 1);
		break;
	}

	return 0;
}

inline s32
get_lines_for_next_level(s32 start_level, s32 level)
{
	s32 first_level_up_limit = min((start_level * 10 + 10), max(100, (start_level * 10 - 50)));

	if (level == start_level)
	{
		return first_level_up_limit;
	}

	s32 diff = level - start_level;
	return first_level_up_limit + diff * 10;
}

#endif // LEVEL_H