#ifndef MOVE_H
# define MOVE_H
#include "agent.h"
#include <stdlib.h>

command_t	targeted_explore(agent_info_t info);
dir_t 		go_to_coords(agent_info_t info, coords_t target);
dir_t		go_home(agent_info_t info);
cell_t		get_cell_type(agent_info_t info, dir_t d);
command_t	try_going(agent_info_t info, dir_t direction);

#endif
