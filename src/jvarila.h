#ifndef JVARILA_H
# define JVARILA_H

# include "agent.h"
# include <stdbool.h>

typedef struct s_hivemind
{
	cell_t		map[NUM_ROWS][NUM_COLS];
	int			bee_targets_reached[NUM_BEES];
	int			last_observed[NUM_ROWS][NUM_COLS];
	bool		initial_explore[NUM_BEES];
}			t_hivemind;

extern t_hivemind g_hivemind;

void		initialize_hivemind(void);
void		update_hivemind(agent_info_t info);
void		print_hivemind(void);
int			distance_squared(coords_t from, coords_t to);
bool		blocked(agent_info_t info, int dir);
int			direction_towards(coords_t from, coords_t to);
int			closest_flower_direction(agent_info_t info);
coords_t	closest_flower_in_hivemind(agent_info_t info);
coords_t	closest_bee_to_target(agent_info_t info, coords_t target);
command_t	move_in_random_unblocked_direction(agent_info_t info);

#endif
