#!/usr/bin/env bash

set -e

if [ $# < 2 ]; then
    echo "Usage: ./match agent0_directory agent1_directory [amount of rounds]"
    exit 1
fi

make -C src
make -C $1
make -C $2

team1=$(basename $1)
team2=$(basename $2)
isodate=$(date -u +"%Y-%m-%dT%H:%M:%SZ")
logfile="${team1}-${team2}-${isodate}_tournament.log"

port=8000

team1_name=`grep team_name $team1/main.c | head -n 1 | awk '{print $4}' | tr -d '"' | tr -d ';'`
team2_name=`grep team_name $team2/main.c | head -n 1 | awk '{print $4}' | tr -d '"' | tr -d ';'`

rounds=10
if [ -n "$3" ]; then
	rounds=$3
fi
round=0
team1_wins=0
team1_side0_wins=0
team1_side1_wins=0
team2_wins=0
team2_side0_wins=0
team2_side1_wins=0
draws=0
sleep_time=0.0001

for i in $(seq 1 $rounds)
do
	(sleep $sleep_time ; cd $1 ; ./agent 127.0.0.1 $port | sed -e 's/^/[p0] /' ) &
	(sleep $sleep_time ; cd $2 ; ./agent 127.0.0.1 $port | sed -e 's/^/[p1] /' ) &

	bin/arena $port $logfile
	winner=`tail -n 1 $logfile | awk '{print $5}' | cut -c 2- | rev | cut -c 2- | rev | tr -d "[:space:]"`
	side=`tail -n 1 $logfile | awk '{print $4}'`
	if [ "$winner" = "$team1_name" ]; then
		team1_wins=$((team1_wins+1))
		if [ "$side" = "0" ]; then
			team1_side0_wins=$((team1_side0_wins+1))
		else
			team1_side1_wins=$((team1_side1_wins+1))
		fi
	elif [ "$winner" = "$team2_name" ]; then
		if [ "$side" = "0" ]; then
			team2_side0_wins=$((team2_side0_wins+1))
		else
			team2_side1_wins=$((team2_side1_wins+1))
		fi
		team2_wins=$((team2_wins+1))
	else
		draws=$((draws+1))
	fi
done

team1_win_percentage=`echo "scale = 4; $team1_wins / $rounds * 100" | bc`
team2_win_percentage=`echo "scale = 4; $team2_wins / $rounds * 100" | bc`
draw_percentage=`echo "scale = 4; $draws / $rounds * 100" | bc`

printf "%-40s%d\n" "Team $team1 wins:" "$team1_wins"
printf "%-40s%.2f\n" "Team $team1 win percentage:" "$team1_win_percentage"
printf "%-40s%d\n" "Team $team1 side 0 wins:" "$team1_side0_wins"
printf "%-40s%d\n" "Team $team1 side 1 wins:" "$team1_side1_wins"
printf "%-40s%d\n" "Team $team2 wins:" "$team2_wins"
printf "%-40s%.2f\n" "Team $team2 win percentage:" "$team2_win_percentage"
printf "%-40s%d\n" "Team $team2 side 0 wins:" "$team2_side0_wins"
printf "%-40s%d\n" "Team $team2 side 1 wins:" "$team2_side1_wins"
printf "%-40s%d\n" "Draws:" "$draws"
printf "%-40s%.2f\n" "Draw percentage:" "$draw_percentage"
