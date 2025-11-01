#include "move.h"
#include <stdio.h>

void	update_bee_target(agent_info_t info)
{
	g_hivemind.bee_targets_reached[info.bee] += 1;
}

int closest_param_direction(agent_info_t info, cell_t param)
{
	coords_t	bee = (coords_t) {
		.row = VIEW_DISTANCE,
		.col = VIEW_DISTANCE
	};
	coords_t	closest_param = (coords_t) {	// Place default invalid flower at an impossible distance
		.row = 2 * VIEW_DISTANCE + 1,
		.col = 2 * VIEW_DISTANCE + 1
	};

	for (int r = 0; r < 2 * VIEW_DISTANCE + 1; ++r) {
		for (int c = 0; c < 2 * VIEW_DISTANCE + 1; ++c) {
			if (info.cells[r][c] != param)
				continue;

			coords_t	new_param = {.row = r, .col = c};
			int			new_dist = distance_squared(bee, new_param);
			int			old_dist = distance_squared(bee, closest_param);

			if (new_dist < old_dist)
				closest_param = new_param;
		}
	}
	if (distance_squared(bee, closest_param) > 2 * VIEW_DISTANCE * VIEW_DISTANCE)
		return -1;
	return direction_towards(bee, closest_param);
}


command_t	targeted_explore(agent_info_t info)
{
	int			num_locations = 3;
	int			target;
	cell_t		opponent = ((info.player == 1) ? BEE_0 : BEE_1);
	coords_t    targets[2][NUM_BEES][3] = {
		{                                  
			{{3, 3}, {3, 10}, {3, 27}},    
			{{3, 16}, {3, 22}, {7, 25}},   
			{{9, 16}, {16, 16}, {12, 25}}, 
			{{22, 16}, {22, 22}, {17, 25}},
			{{22, 3}, {22, 10}, {22, 27}}  
		},                                 
		{                                  
			{{3, 27}, {3, 20}, {3, 3}},    
			{{3, 14}, {3, 8}, {7, 5}},     
			{{9, 14}, {16, 14}, {12, 5}},  
			{{22, 14}, {22, 8}, {17, 5}},  
			{{22, 27}, {22, 20}, {22, 3}}  
		}                                 
	};
	//if doing and initial rush and you see an opponent bee (with or without flower), set initial rush to false
	if (g_hivemind.initial_explore[info.bee] && (closest_param_direction(info, opponent) > -1 || closest_param_direction(info, opponent + 2) > -1))
		g_hivemind.initial_explore[info.bee] = false;
	target = g_hivemind.bee_targets_reached[info.bee];
	if (target < num_locations && targets[info.player][info.bee][target].row == info.row &&  
		targets[info.player][info.bee][target].col == info.col)
	{
//		printf("Bee %d reached a target\n", info.bee);
		update_bee_target(info);
	}
	if (target == num_locations)
	{
//		printf("Bee %d reached final destination\n", info.bee);
		return(move_in_random_unblocked_direction(info));
	}
	return (try_going(info, go_to_coords(info, targets[info.player][info.bee][target])));
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

