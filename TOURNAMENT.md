Here are some stats from sets of 1000 fights between the different agents, each cell has percentages for (team_on_row : draw : team_on_column):

| **             | example              | combined           | rushing             | aggressive            |
| -------------- | -------------------- | ------------------ | ------------------- | --------------------- |
| **example**    | **                   | 0.00:0.00:100.00   | 0.00:0.00:100.00    | 0.00:0.00:100.00      |
| **combined**   | 100.00 : 0.00 : 0.00 | **                 | 98.20 : 0.70: 1.10  | 41.70 : 14.10 : 44.20 |
| **rushing**    | 100.00 : 0.00 : 0.00 | 1.10 : 0.70: 98.20 | **                  | 0.50 : 0.50 : 99.00   |
| **aggressive** | 100.00 : 0.00 : 0.00 | 44.20:14.10:41.70  | 99.00 : 0.50 : 0.50 | **                    |

We also tested **aggressive** and **combined** against some agents we found in public repos from the 2022 challenge, again the results presented in table form (to run test, we needed to copy the libagent.a into the cloned repo, and some projects still wouldn't compile):

**DeepBlueC:** https://github.com/SeanTroy/HiveArena

**tribe13-agent7:** https://github.com/kona3b/42_Hive_Arena

**team-aleph:** https://github.com/teemu-hakala/hive-arena-team-aleph (takes ~15 minutes to simulate 1000 matches against this staller)

**Framboisier:** https://github.com/B9R9/42-Hive-Arena


| **                 | combined              | aggressive            |
| ------------------ | --------------------- | --------------------- |
| **DeepBlueC**      | 12.80 : 58.70 : 28.50 | 14.00 : 53.60 : 32.40 |
| **tribe13-agent7** | 10.00 : 65.10 : 24:90 | 9.70 : 56.90 : 33.40  |
| **team-aleph**     | 1.80 : 95.90 : 2.30   | 1.60 : 95.90 : 2.50   |
| **Framboisier**    | 1.20 : 0.60 : 98.20   | 1.40 : 0.70 : 97.90   |
