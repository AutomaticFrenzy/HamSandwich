/* DR. LUNATIC SUPREME WITH CHEESE

   A HamumuSoft Production.

   v 6.2

   Copyright 2003, Mike Hommel
*/

#include "mgldraw.h"
#include "jamulfont.h"
#include "jamulsound.h"

#include "moron.h"
#include "game.h"
#include "editor.h"
#include "tile.h"
#include "sound.h"
#include "monster.h"
#include "title.h"
#include "profmenu.h"
#include "worldselect.h"
#include "playlist.h"
#include "hiscore.h"
#include "arcade.h"
#include "moron.h"
#include "config.h"
#include "log.h"
#include "netmenu.h"
#include "internet.h"

#ifdef _WIN32
#include <shellapi.h>
#endif

MGLDraw *mainmgl;

int main(int argc, char* argv[])
{
	bool windowedGame=false;

	for (int i = 1; i < argc; ++i)
	{
		if (!strcmp(argv[i], "window"))
			windowedGame=true;
	}
	InitLog();

	LoadConfig();
	mainmgl=new MGLDraw("Supreme With Cheese",640,480,32,windowedGame);
	if(!mainmgl)
		return 0;

	LunaticInit(mainmgl);

	//CryptoTest();
#ifdef ARCADETOWN
	SplashScreen(mainmgl,"graphics/at_presents.bmp",32,0);
#endif
	SplashScreen(mainmgl,"graphics/hamumu.bmp",32,2);

	//NewComputerSpriteFix("graphics/items.jsp");
	//NewComputerSpriteFix("graphics/intface.jsp");
	shopping=0;
	while(1)
	{
		switch(MainMenu(mainmgl))
		{
			case 255:	// quit
				LunaticExit();
				delete mainmgl;
				ExitLog();
				FatalErrorQuit();
				return 0;
				break;
			case 0:	// new game
				shopping=0;
				WorldSelectMenu(mainmgl);
				break;
			case 1:	// continue
				shopping=0;
				ProfMenu(mainmgl);
				break;
			case 2:	// tutorial
				shopping=0;
				if(PlayWorld(mainmgl,"tutorial.dlw"))
					WorldSelectMenu(mainmgl);
				break;
			case 3:	// instructions
				HelpScreens(mainmgl);
				break;
			case 6:	// shopping
				shopping=1;
				ResetMoron();
				PlayWorld(mainmgl,"spismall.zlw");
				break;
			case 5:	// internet
				shopping=0;
				NetMenu(mainmgl);

				if(DoWebPage()==1)
				{
					LunaticExit();
					delete mainmgl;
					ExitLog();
					FatalErrorQuit();
#ifdef _WIN32
					ShellExecuteA(NULL,"open","http://hamumu.com/scores.php",NULL,NULL,SW_SHOWNORMAL);
#endif
					return 0;
				}
				else if(DoWebPage()==2)
				{
					LunaticExit();
					delete mainmgl;
					ExitLog();
					FatalErrorQuit();

#ifdef _WIN32
					ShellExecuteA(NULL,"open","http://hamumu.com/addon.php",NULL,NULL,SW_SHOWNORMAL);
#endif
					return 0;
				}
				break;
			case 7:	// editor
				shopping=0;
				LunaticEditor(mainmgl);
				break;
			case 8:	// insta-play the last world you were on
				shopping=0;
				if(profile.lastWorld[0]!='\0')
				{
					if(PlayWorld(mainmgl,profile.lastWorld))
						WorldSelectMenu(mainmgl);
				}
				break;
		}
	}

	StopSong();
	LunaticExit();
	delete mainmgl;
	ExitLog();
	FatalErrorQuit();
	JamulSoundExit();
}
