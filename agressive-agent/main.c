#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "agent.h"
#include "jvarila.h"
#include "move.h"

t_hivemind	g_hivemind;

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
	update_hivemind(info);
//	print_hivemind();

    cell_t bee = info.cells[VIEW_DISTANCE][VIEW_DISTANCE];

    if (is_bee_with_flower(bee))
    {
		// Bee next to its corresponding hive
        int hive_dir = find_neighbour(info, hive_cell(info.player));
        if (hive_dir >= 0) {
            return (command_t) {
                .action = FORAGE,
                .direction = hive_dir
            };
        } else {
			return (try_going(info, go_home(info)));
		}
    }
	
	// If next to a flower
	int flower_dir = find_neighbour(info, FLOWER);
	if (flower_dir >= 0)
	{
		return (command_t) {
			.action = FORAGE,
			.direction = flower_dir
		};
	}

	// If next to an enemy bee with a flower -> punch enemy
	int	enemy_type	= (info.player == 0 ? BEE_1_WITH_FLOWER : BEE_0_WITH_FLOWER);
	int	enemy_dir	= find_neighbour(info, enemy_type);
	if (enemy_dir >= 0) {
		return (command_t) {
			.action = GUARD,
			.direction = enemy_dir
		};
	}

	// If next to enemy hive -> build wall?

	// Try to find a flower in the view distance
	flower_dir = closest_flower_direction(info);
	if (flower_dir >= 0) {
		int	count = 0;
		while (blocked(info, flower_dir) && count++ < 8)
			flower_dir = (flower_dir + 1) % 8;
		if (flower_dir >= 0) {
			return (command_t) {
				.action = MOVE,
				.direction = flower_dir
			};
		}
	}

	// Try to look for a flower in the hivemind
	coords_t	closest_flower = closest_flower_in_hivemind(info);
	if (closest_flower.row >= NUM_ROWS || closest_flower.col >= NUM_COLS)
	{
		// printf("No closest hivemind flower\n");
		// fflush(stdout);
		return targeted_explore(info);
	}
	// printf("The hivemind knows about a flower at row: %d col: %d\n", closest_flower.row, closest_flower.col);
	// fflush(stdout);
	coords_t	closest_bee = closest_bee_to_target(info, closest_flower);
	if (closest_bee.row != info.row || closest_bee.col != info.col)
		return targeted_explore(info);
	// printf("Bee at row: %d col: %d pursuing\n", closest_bee.row, closest_bee.col);
	// fflush(stdout);
	return (command_t) {
		.action = MOVE,
		.direction = direction_towards(closest_bee, closest_flower)
	};
}

int main(int argc, char **argv)
{
    if (argc < 3)
        panic("Usage: ./agent arena_host arena_ip");

    srand(time(NULL) + getpid());

    char *host = argv[1];
    int port = atoi(argv[2]);
    char *team_name = "agressive-agent";
	
	for (int i = 0; i < NUM_ROWS; ++i) {
		for (int j = 0; j < NUM_COLS; ++j)
			g_hivemind.map[i][j] = UNKNOWN;
	}

	initialize_hivemind();
    agent_main(host, port, team_name, think);
}
