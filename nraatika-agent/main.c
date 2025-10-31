#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "agent.h"

/*
int hive_is_visible(agent_info_t info, int *x, int *y)
{
	int seen = 0;
	for (int i=0; i < VIEW_SIZE; ++i)
	{
		for (int j=0; j < VIEW_SIZE; ++j)
		{
			if (info.cells[i][j] == (HIVE_0 + info.player))
			{
				*y = i;
				*x = j;
				return 1;
			}
		}
	}
	return 0;
}*/

//go toward home
static dir_t go_home(agent_info_t info)
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

static cell_t	get_cell_type(agent_info_t info, dir_t d)
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

static command_t try_going(agent_info_t info, dir_t direction)
{
	printf("Bee at [%d,%d] going towards %d\n", info.row, info.col, direction);
	if (get_cell_type(info, direction) == EMPTY)
	{
		return (command_t) {
                .action = MOVE,
                .direction = direction
            };
	}
	else 
	{
		return (command_t) {
        	.action = MOVE,
	        .direction = rand() % 8
    	};
	}
}

int find_neighbour(agent_info_t info, cell_t type)
{
    coords_t center = {VIEW_DISTANCE, VIEW_DISTANCE};

    for(int dir = 0 ; dir < 8 ; dir++)
    {
        coords_t coords = direction_to_coords(center, dir);
        cell_t neighbour = info.cells[coords.row][coords.col];
        if (neighbour == type)
        {
            return dir;
        }
    }

    return -1;
}

command_t think(agent_info_t info)
{
    cell_t bee = info.cells[VIEW_DISTANCE][VIEW_DISTANCE];

    if (is_bee_with_flower(bee))
    {
        int hive_dir = find_neighbour(info, hive_cell(info.player));
        if (hive_dir >= 0)
        {
            return (command_t) {
                .action = FORAGE,
                .direction = hive_dir
            };
        }
		else
			return (try_going(info, go_home(info)));
    }
    else
    {
        int flower_dir = find_neighbour(info, FLOWER);
        if (flower_dir >= 0)
        {
            return (command_t) {
                .action = FORAGE,
                .direction = flower_dir
            };
        }
    }

    return (command_t) {
        .action = MOVE,
        .direction = rand() % 8
    };
}

int main(int argc, char **argv)
{
    if (argc < 3)
        panic("Usage: ./agent arena_host arena_ip");

    srand(time(NULL) + getpid());

    char *host = argv[1];
    int port = atoi(argv[2]);
    char *team_name = "nraatika_agent";

    agent_main(host, port, team_name, think);
}
