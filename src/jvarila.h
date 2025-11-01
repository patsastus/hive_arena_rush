#ifndef JVARILA_H
# define JVARILA_H

# include "agent.h"
# include <stdbool.h>

typedef struct s_hivemind
{
	cell_t		map[NUM_ROWS][NUM_COLS];
}			t_hivemind;

void		update_hivemind(agent_info_t info);
void		print_hivemind();
int			distance_squared(coords_t from, coords_t to);
bool		blocked(agent_info_t info, int dir);
int			closest_flower_direction(agent_info_t info);
command_t	move_in_random_unblocked_direction(agent_info_t info);

#endif
