#include "move.h"

command_t	targeted_explore(agent_info_t info)
{
	static int	switch_after = 100;
	coords_t targets[NUM_BEES][2] = {
		{{3, 3}, {3, 10}},
		{{3, 16}, {3, 22}},
		{{9, 16}, {16, 16}},
		{{22, 16}, {22, 22}},
		{{22, 3}, {22, 10}}
	};
	
	return (try_going(info, go_to_coords(info, targets[info.bee][(info.turn / switch_after) % 2])));
}

dir_t	go_to_coords(agent_info_t info, coords_t target)
{
	int	dx = target.col - info.col;
	int dy = target.row - info.row;

	if (dx == 0 || dy == 0)
	{
		if (dx == 0)
			return (dy < 0 ? N : S);
		else
			return (dx < 0 ? W : E);
	}
	if (abs(dx) * 2 < abs(dy))
		return (dy < 0 ? N : S);
	else if (abs(dy) * 2 < abs(dx))
		return (dx < 0 ? W : E);
	else
	{
		if (dx < 0)
			return (dy < 0 ? NW : SW);
		else
			return (dy < 0 ? NE : SE);
	}
}	

dir_t	go_home(agent_info_t info)
{
	dir_t goal = W - 4*info.player;
	int home[2] = {12, 1};
	if (info.player)
	{
		home[1] = 28;
		if (info.row - home[0] > 0)
			goal = NE;
		else if (info.row - home[0] < 0)
			goal = SE;
		if (info.col >= 27)
			goal = N + (info.row - home[0] < 0)*4;
	}
	else
	{
		if (info.row - home[0] > 0)
			goal = NW;
		else if (info.row - home[0] < 0)
			goal = SW;
		if (info.col <= 2)
			goal = N + (info.row - home[0] < 0)*4;
	}
	return goal;
}

cell_t	get_cell_type(agent_info_t info, dir_t d)
{
	int	x = 0;
	int y = 0;

	if (d == N || d == NW || d == NE)
		y = -1;
	if (d == S || d == SW || d == SE)
		y = 1;
	if (d == E || d == NE || d == SE)
		x = 1;
	if (d == W || d == NW || d == SW)
		x = -1;
	return (info.cells[VIEW_DISTANCE+y][VIEW_DISTANCE+x]);
}

command_t try_going(agent_info_t info, dir_t direction)
{
//	printf("Bee at [%d,%d] going towards %d\n", info.row, info.col, direction);
	cell_t target = get_cell_type(info, direction);
	//if direction is empty, move
	if (target == EMPTY)
	{
		return (command_t) {
                .action = MOVE,
                .direction = direction
            };
	}
	//if direction is wall, punch it
	else if (target == WALL)
	{
		return (command_t) {
                .action = GUARD,
                .direction = direction
            };
	}
	//if all else fails, panic
	else 
	{
		return (command_t) {
        	.action = MOVE,
	        .direction = rand() % 8
    	};
	}
}

