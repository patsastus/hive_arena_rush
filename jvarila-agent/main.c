#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "agent.h"
#include <stdbool.h>

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

	printf("Searching for unblocked direction in random move function\n");

	while (blocked(info, dir) && count++ < 8)
	{
		printf("Was blocked\n");
		dir = (dir + 1) % 8;
	}

	printf("Moving into dir %d\n", dir);
	return (command_t) {
		.action = MOVE,
		.direction = dir
	};
}

command_t think(agent_info_t info)
{
    cell_t bee = info.cells[VIEW_DISTANCE][VIEW_DISTANCE];

    if (is_bee_with_flower(bee))
    {
		// Bee happens to be next to the corresponding hive
        int hive_dir = find_neighbour(info, hive_cell(info.player));
        if (hive_dir >= 0)
        {
            return (command_t) {
                .action = FORAGE,
                .direction = hive_dir
            };
        }
		// If it has a flower randomly try to find way back to hive
		return move_in_random_unblocked_direction(info);
    }

	// If next to a flower
	int flower_dir = find_neighbour(info, FLOWER);
	if (flower_dir >= 0) {
		return (command_t) {
			.action = FORAGE,
			.direction = flower_dir
		};
	}

	// Try to find a flower in the view distance
	flower_dir = closest_flower_direction(info);

	// No flower in view distance -> random move
	if (flower_dir < 0)
		return move_in_random_unblocked_direction(info);

	int	count = 0;
	while (blocked(info, flower_dir) && count++ < 8)
		flower_dir = (flower_dir + 1) % 8;
	if (flower_dir >= 0) {
		return (command_t) {
			.action = MOVE,
			.direction = flower_dir
		};
	}

	return move_in_random_unblocked_direction(info);
}

int main(int argc, char **argv)
{
    if (argc < 3)
        panic("Usage: ./agent arena_host arena_ip");

    srand(time(NULL) + getpid());

    char *host = argv[1];
    int port = atoi(argv[2]);
    char *team_name = "jvarila_agent";

    agent_main(host, port, team_name, think);
}
