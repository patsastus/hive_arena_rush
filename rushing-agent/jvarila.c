#include "jvarila.h"
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>


void	initialize_hivemind(void)
{
	for (int i = 0; i < NUM_ROWS; ++i) {
		for (int j = 0; j < NUM_COLS; ++j) {
			g_hivemind.map[i][j] = UNKNOWN;
			g_hivemind.last_observed[i][j] = 0;
		}
	}
	for (int i = 0; i < NUM_BEES; ++i) {
		g_hivemind.initial_explore[i] = true;
	}
}

void	update_hivemind(agent_info_t info)
{
	for (int r = 0; r < 2 * VIEW_DISTANCE + 1; ++r) {
		for (int c = 0; c < 2 * VIEW_DISTANCE + 1; ++c) {
			int	global_row = r + info.row - VIEW_DISTANCE;
			int	global_col = c + info.col - VIEW_DISTANCE;
			if (global_row < 0 || global_row >= NUM_ROWS
				|| global_col < 0 || global_col >= NUM_COLS)
				continue;
			g_hivemind.map[global_row][global_col] = info.cells[r][c];
			g_hivemind.last_observed[global_row][global_col] = info.turn;
		}
	}
	for (int r = 0; r < NUM_ROWS; ++r) {
		for (int c = 0; c < NUM_COLS; ++c) {
			if (info.turn - g_hivemind.last_observed[r][c] > 200)
				g_hivemind.map[r][c] = UNKNOWN;
		}
	}
}

void	print_hivemind(void)
{
	for (int r = 0; r < NUM_ROWS; ++r) {
		for (int c = 0; c < NUM_COLS; ++c) {
			cell_t	cell = g_hivemind.map[r][c];
			switch(cell) {
				case EMPTY:
					write(STDOUT_FILENO, " . ", 3);
					break;
				case BEE_0:
					write(STDOUT_FILENO, " B0", 3);
					break;
				case BEE_1:
					write(STDOUT_FILENO, " B1", 3);
					break;
				case BEE_0_WITH_FLOWER:
					write(STDOUT_FILENO, " f0", 3);
					break;
				case BEE_1_WITH_FLOWER:
					write(STDOUT_FILENO, " f1", 3);
					break;
				case HIVE_0:
					write(STDOUT_FILENO, " H0", 3);
					break;
				case HIVE_1:
					write(STDOUT_FILENO, " H1", 3);
					break;
				case FLOWER:
					write(STDOUT_FILENO, " F ", 3);
					break;
				default:
					write(STDOUT_FILENO, " ? ", 3);
			}
		}
		write(STDOUT_FILENO, "\n", 1);
	}
	write(STDOUT_FILENO, "\n", 1);
}

inline int distance_squared(coords_t from, coords_t to)
{
	return (	(to.col - from.col) * (to.col - from.col)
				+ (to.row - from.col) * (to.row - from.col));
}

inline bool blocked(agent_info_t info, int dir)
{
	coords_t	bee		= {VIEW_DISTANCE, VIEW_DISTANCE};
	coords_t	target	= direction_to_coords(bee, dir);

	return (info.cells[target.row][target.col] != EMPTY);
}

int	direction_towards(coords_t from, coords_t to)
{
	if (to.col == from.col && to.row < from.row)
		return N;
	if (to.col > from.col && to.row < from.row)
		return NE;
	if (to.col > from.col && to.row == from.row)
		return E;
	if (to.col > from.col && to.row > from.row)
		return SE;
	if (to.col == from.col && to.row > from.row)
		return S;
	if (to.col < from.col && to.row > from.row)
		return SW;
	if (to.col < from.col && to.row == from.row)
		return W;
	else
		return NW;
}

int closest_flower_direction(agent_info_t info)
{
	coords_t	bee = (coords_t) {
		.row = VIEW_DISTANCE,
		.col = VIEW_DISTANCE
	};
	coords_t	closest_flower = (coords_t) {	// Place default invalid flower at an impossible distance
		.row = 2 * VIEW_DISTANCE + 1,
		.col = 2 * VIEW_DISTANCE + 1
	};

	for (int r = 0; r < 2 * VIEW_DISTANCE + 1; ++r) {
		for (int c = 0; c < 2 * VIEW_DISTANCE + 1; ++c) {
			if (info.cells[r][c] != FLOWER)
				continue;

			coords_t	new_flower = {.row = r, .col = c};
			int			new_dist = distance_squared(bee, new_flower);
			int			old_dist = distance_squared(bee, closest_flower);

			if (new_dist < old_dist)
				closest_flower = new_flower;
		}
	}
	if (distance_squared(bee, closest_flower) > 2 * VIEW_DISTANCE * VIEW_DISTANCE)
		return -1;
	return direction_towards(bee, closest_flower);
}

coords_t	closest_x_to_target(cell_t type, coords_t target)
{
	coords_t	closest = {.row = 3 * NUM_ROWS, .col = 3 * NUM_COLS};

	for (int r = 0; r < NUM_ROWS; ++r) {
		for (int c = 0; c < NUM_COLS; ++c) {
			if (g_hivemind.map[r][c] != type)
				continue;
			coords_t	candidate = {.row = r, .col = c};
			if (distance_squared(candidate, target) < distance_squared(candidate, closest))
				closest = candidate;
		}
	}
	return closest;
}

coords_t	closest_flower_in_hivemind(agent_info_t info)
{
	coords_t	bee = {.row = info.row, .col = info.col};

	return closest_x_to_target(FLOWER, bee);
}


coords_t	closest_bee_to_target(agent_info_t info, coords_t target)
{
	cell_t		bee_team	= (info.player == 0 ? BEE_0 : BEE_1);

	return closest_x_to_target(bee_team, target);
}

inline command_t	move_in_random_unblocked_direction(agent_info_t info)
{
	int	dir		= rand() % 8;
	int	count	= 0;

	while (blocked(info, dir) && count++ < 8)
		dir = (dir + 1) % 8;

	return (command_t) {
		.action = MOVE,
		.direction = dir
	};
}
