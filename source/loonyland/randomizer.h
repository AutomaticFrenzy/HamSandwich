#ifndef Randomizer_H
#define Randomizer_H

#include "jamultypes.h"
#include "mgldraw.h"
#include "highscore.h"
#include <string>
#include <set>

TASK(void) RandomizerMenu(MGLDraw *mgl);

#define CURSOR_RANDOSEED	0
#define CURSOR_SEEDENTRY	1
#define CURSOR_GENERATE		2
#define CURSOR_PLAY			3
#define CURSOR_EXIT			4
#define CURSOR_DIFFICULTY	5
#define CURSOR_COMPLETION	6


#define CURSOR_START	0
#define CURSOR_END		6

#define MAX_SEED_LENGTH 11
#define R_NUM_LOCATIONS 106

struct rItem
{
	/* data */
	//int randId;
	int itemId = 0;
	int playerVarId = 0;
	std::string itemName = "";
};

struct location
{
	/* data */
	//int randId;
	bool isQuest = false;
	std::string mapName = 0;
	int mapId, xcoord, ycoord = 0;
	int s1, s2 = 0;
	std::string description = "";
	std::function<bool(std::set<int> inv)> requirements;
	rItem item;
};


void RandomizeSeed();

void PlaceItems(std::vector<location>& loc);

bool HaveLightSource(const std::set<int>& inv);

bool HaveAnyBigGem(const std::set<int>& inv);

bool HaveAllOrbs(const std::set<int>& inv);

bool HaveAllBats(const std::set<int>& inv);

bool HaveAllVamps(const std::set<int>& inv);

bool HaveSpecialWeaponDamage(const std::set<int>& inv);

bool HaveAllMushrooms(const std::set<int>& inv);

bool CanCleanseCrypts(const std::set<int>& inv);

bool CanEnterRockyCliffs(const std::set<int>& inv);

bool CanEnterVampy(const std::set<int>& inv);

bool CanEnterVampyII(const std::set<int>& inv);

bool CanEnterVampyIII(const std::set<int>& inv);

bool CanEnterVampyIV(const std::set<int>& inv);


int RandomFill(std::vector<location>& locs);

bool CheckBeatable(std::vector<location>& locs);

std::string GetSeed();

#endif
