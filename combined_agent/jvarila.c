#include "jvarila.h"
#include <unistd.h>
#include <stdlib.h>

t_hivemind	g_hivemind;

void	update_hivemind(agent_info_t info)
{
	int	top		= info.row - VIEW_DISTANCE - 1;
	int	bottom	= info.row + VIEW_DISTANCE + 1;
	int	left	= info.col - VIEW_DISTANCE - 1;
	int	right	= info.col + VIEW_DISTANCE + 1;

	for (int i = 0; i < 2 * VIEW_DISTANCE + 3; ++i) {
		if (top > 0 && left + i > 0 && left + i < NUM_COLS)
			g_hivemind.map[top][left + i] = UNKNOWN;
		if (bottom < NUM_ROWS && left + i > 0 && left + i < NUM_COLS)
			g_hivemind.map[bottom][left + i] = UNKNOWN;
		if (left > 0 && top + i > 0 && top + i < NUM_ROWS)
			g_hivemind.map[top + i][left] = UNKNOWN;
		if (right < NUM_COLS && top + i > 0 && top + i < NUM_ROWS)
			g_hivemind.map[top + i][right] = UNKNOWN;
	}
	// printf("Updating hivemind for player %d bee %d\n", info.player, info.bee);
	// printf("%d %d\n", info.player, info.bee);
	for (int r = 0; r < 2 * VIEW_DISTANCE + 1; ++r) {
		for (int c = 0; c < 2 * VIEW_DISTANCE + 1; ++c) {
			int	global_row = r + info.row - VIEW_DISTANCE;
			int	global_col = c + info.col - VIEW_DISTANCE;
			if (global_row < 0 || global_row >= NUM_ROWS
				|| global_col < 0 || global_col >= NUM_COLS)
				continue;
			// printf("r: %d\tc: %d\tgr: %d\tgc: %d\n", r, c, global_row, global_col);
			g_hivemind.map[global_row][global_col] = info.cells[r][c];
		}
	}
}

void	print_hivemind()
{
	for (int r = 0; r < NUM_ROWS; ++r) {
		for (int c = 0; c < NUM_COLS; ++c) {
			cell_t	cell = g_hivemind.map[r][c];
			switch(cell) {
				case EMPTY:
					write(STDOUT_FILENO, " . ", 3);
					break;
				case BEE_0:
				case BEE_1:
					write(STDOUT_FILENO, " B ", 3);
					break;
				case BEE_0_WITH_FLOWER:
				case BEE_1_WITH_FLOWER:
					write(STDOUT_FILENO, " f ", 3);
					break;
				case HIVE_0:
				case HIVE_1:
					write(STDOUT_FILENO, " H ", 3);
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

	return (info.cells[target.row][target.col] != EMPTY ? true : false);
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
	if (closest_flower.col == bee.col && closest_flower.row < bee.row)
		return N;
	if (closest_flower.col > bee.col && closest_flower.row < bee.row)
		return NE;
	if (closest_flower.col > bee.col && closest_flower.row == bee.row)
		return E;
	if (closest_flower.col > bee.col && closest_flower.row > bee.row)
		return SE;
	if (closest_flower.col == bee.col && closest_flower.row > bee.row)
		return S;
	if (closest_flower.col < bee.col && closest_flower.row > bee.row)
		return SW;
	if (closest_flower.col < bee.col && closest_flower.row == bee.row)
		return W;
	else
		return NW;
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
