#include "pause.h"
#include "player.h"
#include "options.h"
#include "quest.h"
#include "ch_summon.h"
#include "appdata.h"

#define SUBMODE_NONE	 0
#define SUBMODE_SLOTPICK 1

byte cursor=0;
static byte subcursor=0;
static char lastKey=0;
byte subMode;
float percent[5];	// the percentages in each save slot
char  area[5][32];		// which area the player was in in each
static byte darkness;
static int offX;
static byte oldc;
static byte noSaving=0;

void SetSubCursor(byte s)
{
	subcursor=s;
}

void RenderPowerUps(int x,int y)
{
	int i,j;
	char s[16];

	Print(x+4,y+2,"Power-Ups",0,2);
	DrawFillBox(x+1,y+45,x+220,y+270,3);
	DrawBox(x+1,y+45,x+220,y+270,31);

	// show powerups
	for(i=9;i>=player.firePower;i--)
		InstaRenderItem2(x+14+i*15,y+110,ITM_FIREPOWERUP,0,-32,GetDisplayMGL());
	for(i=0;i<player.firePower;i++)
		InstaRenderItem2(x+14+i*15,y+110,ITM_FIREPOWERUP,0,i*3,GetDisplayMGL());
	for(i=9;i>=player.fireRate;i--)
		InstaRenderItem2(x+15+i*15,y+155,ITM_FIRERATEUP,0,-32,GetDisplayMGL());
	for(i=0;i<player.fireRate;i++)
		InstaRenderItem2(x+15+i*15,y+155,ITM_FIRERATEUP,0,i*3,GetDisplayMGL());
	for(i=9;i>=player.fireRange;i--)
		InstaRenderItem2(x+14+i*15,y+220,ITM_RANGEUP,0,-32,GetDisplayMGL());
	for(i=0;i<player.fireRange;i++)
		InstaRenderItem2(x+14+i*15,y+220,ITM_RANGEUP,0,i*3,GetDisplayMGL());

	for(i=0;i<20;i++)
		InstaRenderItem2(x+186+(i&1)*20,y+80+(i/2)*18,ITM_BIGHEART,0,-32,GetDisplayMGL());
	if(player.monsType==MONS_PLYRSUMMON)
	{
		if(player.cheatsOn&PC_TERROR)
			for(i=0;i<(int)player.xtraByte-20;i++)
				InstaRenderItem2(x+186+(i&1)*20,y+80+(i/2)*18,ITM_BIGHEART,0,i,GetDisplayMGL());
		else
			for(i=0;i<(int)player.xtraByte;i++)
				InstaRenderItem2(x+186+(i&1)*20,y+80+(i/2)*18,ITM_BIGHEART,0,i,GetDisplayMGL());
	}
	else
		for(i=0;i<player.maxHearts-player.startHearts;i++)
			InstaRenderItem2(x+186+(i&1)*20,y+80+(i/2)*18,ITM_BIGHEART,0,i,GetDisplayMGL());

	j=(player.maxMoney-50)/25;
	for(i=5;i>=j;i--)
		InstaRenderItem2(x+21+i*25,y+250,ITM_BIGGEM,0,-32,GetDisplayMGL());
	for(i=0;i<j;i++)
		InstaRenderItem2(x+21+i*25,y+250,ITM_BIGGEM,0,i*5,GetDisplayMGL());

	sprintf(s,"MP: %06d",player.monsterPoints);
	Print(x+3,272,s,0,0);
	if(player.monsType==MONS_PLYRSWAMPDOG)
	{
		sprintf(s,"LVL: %02d",player.xtraByte);
		Print(x+130,272,s,0,0);
		sprintf(s,"NEXT: %06d",player.xtraVar-player.monsterPoints);
		Print(x+3,293,s,0,0);
	}
}

void RenderQuests(int x,int y)
{
	int i;

	// list quests
	Print(x+251,y+2,"Quests",0,2);
	DrawFillBox(x+250,y+45,x+630,y+45+264,3);
	DrawBox(x+249,y+44,x+631,y+46+264,31);
	x+=251;
	y+=43;
	for(i=0;i<NUM_QUESTS;i++)
	{
		if(player.var[VAR_QUESTDONE+i])
		{
			PrintColor(x,y,QuestName(i),0,0,0);
			DrawFillBox(x,y+15,x+180,y+15,31);
		}
		else if(player.var[VAR_QUESTASSIGN+i])
		{
			if(!player.var[VAR_QUESTDONE+i])
				PrintColor(x,y,QuestName(i),4,-3,0);
		}
		else	// not done or assigned
			PrintColor(x,y,"???????",0,-32,0);

		y+=24;
		if(i==9)
		{
			x+=194;
			y-=24*10;
		}
	}
}

void RenderInvItem(byte which,int x,int y,MGLDraw *mgl)
{
	char m[8];

	switch(which)
	{
		case 0: // slot #0, slingshot
			if(player.var[VAR_SILVERSLING]==0)
				RenderIntfaceSprite(x,y,9,0,mgl);
			else
				RenderIntfaceSprite(x,y,10,0,mgl);
			break;
		case 1: // slot #1, boots
			RenderIntfaceSprite(x,y,5,-32+32*(player.var[VAR_BOOTS]),mgl);
			break;
		case 2:	// slot #2, cat
			RenderIntfaceSprite(x,y,4,-32+32*(player.var[VAR_CAT]),mgl);
			if(player.var[VAR_QUESTDONE+QUEST_CAT])	// put checkmark if you've returned it
				RenderIntfaceSprite(x,y,3,0,mgl);
			break;
		case 3:	// slot #3, stick
			if(player.var[VAR_LANTERN])
			{
				RenderIntfaceSprite(x-5,y,12,0,mgl);
			}
			else if(player.var[VAR_TORCH]==0)
				RenderIntfaceSprite(x,y+5,7,-32+32*(player.var[VAR_STICK]),mgl);
			else	// render it lit if it is
				RenderIntfaceSprite(x,y+5,8,0,mgl);
			break;
		case 4:	// slot #4, shrooms
			InstaRenderItem(x,y+20,ITM_SHROOM,-32+32*(ShroomCount()>0),mgl);
			if(ShroomCount()>0 && player.var[VAR_QUESTDONE+QUEST_SHROOM]==0)
			{
				sprintf(m,"%d",ShroomCount());
				Print(x+9,y-1,m,-31,0);
				Print(x+11,y+1,m,-31,0);
				Print(x+10,y,m,0,0);
			}
			if(player.var[VAR_WITCHREWARD])
				RenderIntfaceSprite(x,y,3,0,mgl);
			break;
		case 5: // slot #5, doom daisy
			InstaRenderItem(x,y+25,ITM_DAISY,-32+32*(player.var[VAR_DAISY]),mgl);
			if(player.var[VAR_GAVEDAISY])
				RenderIntfaceSprite(x-3,y,3,0,mgl);
			break;
		case 6: // slot #6, ghost potion
			RenderIntfaceSprite(x,y+5,11,-32+32*(player.var[VAR_POTION]),mgl);
			break;
		case 7:	// slot #7, fertilizer
			RenderIntfaceSprite(x,y+5,6,-32+32*(player.var[VAR_FERTILIZER]),mgl);
			break;
		case 8:	// slot #8, mystery orbs
			InstaRenderItem(x,y+20,ITM_MYSORB,-32+32*(OrbCount()>0),mgl);
			if(OrbCount()>0 && OrbsUsed()<4)
			{
				sprintf(m,"%d",OrbCount()-OrbsUsed());
				Print(x+9,y-1,m,-31,0);
				Print(x+11,y+1,m,-31,0);
				Print(x+10,y,m,0,0);
			}
			if(player.var[VAR_QUESTDONE+QUEST_ORBS])
				RenderIntfaceSprite(x-5,y+5,3,0,mgl);
			break;
		case 9:	// slot #9, key 1
			InstaRenderItem2(x+5,y+20,ITM_KEY2,0,-32+32*player.var[VAR_KEY],mgl);
			break;
		case 10:	// slot #10, key 2
			InstaRenderItem2(x+5,y+20,ITM_KEY3,0,-32+32*player.var[VAR_KEY+1],mgl);
			break;
		case 11:	// slot #11, key 3
			InstaRenderItem2(x+5,y+20,ITM_KEY4,0,-32+32*player.var[VAR_KEY+2],mgl);
			break;
		case 27:	// slot #27, bats
			RenderIntfaceSprite(x,y,16,-32+32*(BatCount()>0),mgl);
			if(BatCount()>0 && BatsUsed()<4)
			{
				sprintf(m,"%d",BatCount()-BatsUsed());
				Print(x+9,y-1,m,-31,0);
				Print(x+11,y+1,m,-31,0);
				Print(x+10,y,m,0,0);
			}
			if(BatsUsed()==4)
				RenderIntfaceSprite(x,y,3,0,mgl);
			break;
		case 28:	// slot #28, vamp busts
			RenderIntfaceSprite(x,y,17,-32+32*(VampCount()>0),mgl);
			if(VampCount()>0 && VampsUsed()<8)
			{
				sprintf(m,"%d",VampCount()-VampsUsed());
				Print(x+9,y-1,m,-31,0);
				Print(x+11,y+1,m,-31,0);
				Print(x+10,y,m,0,0);
			}
			if(VampsUsed()==8)
				RenderIntfaceSprite(x,y,3,0,mgl);
			break;
		case 29:	// slot #29, triple fire
			RenderIntfaceSprite(x,y,13,-32+32*(player.var[VAR_TRIPLEFIRE]),mgl);
			break;
		case 30:	// slot #30, reflect
			RenderIntfaceSprite(x,y-5,14,-32+32*(player.var[VAR_REFLECT]),mgl);
			break;
		case 31:	// slot #31, happy stick
			RenderIntfaceSprite(x,y,15,-32+32*(player.var[VAR_TALISMAN]),mgl);
			if(player.var[VAR_QUESTDONE+QUEST_WITCH])
				RenderIntfaceSprite(x,y,3,0,mgl);
			break;
	}
	if(which>=12 && which<20)
	{
		// dolls
		InstaRenderItem(x,y+15,ITM_BATDOLL+which-12,-32+32*player.var[VAR_DOLL+which-12],mgl);
		if(player.var[VAR_DOLLGIVEN+which-12])
			RenderIntfaceSprite(x-5,y,3,0,mgl);
	}
	if(which>=20 && which<27)
	{
		// weapons
		InstaRenderItem2(x,y+20,ITM_WBOMB+which-20,0,-32+32*(player.var[VAR_WEAPON+which-20]>0),mgl);
	}
	if(which==32)
	{
		InstaRenderItem2(x,y+10,ITM_SILVER,0,-32+32*player.var[VAR_SILVER],mgl);
		if(player.var[VAR_QUESTDONE+QUEST_SILVER])
			RenderIntfaceSprite(x-5,y,3,0,mgl);
	}
}

void RenderInventory(int x,int y,MGLDraw *mgl)
{
	int c[]={260,360,
			 300,360,
			 340,360,
			 380,365,
			 420,363,
			 460,360,
			 525,360,
			 553,360,
			 600,360,

			 // keys
			 265,405,
			 305,405,
			 345,405,

			 // dolls
			 445,450,
			 470,450,
			 490,450,
			 510,450,
			 535,450,
			 560,450,
			 587,450,
			 610,450,

			 // weapons
			 390,400,
			 425,400,
			 460,400,
			 495,400,
			 530,400,
			 565,400,
			 600,400,

			 // bat statue
			 260,448,
			 // vamp bust
			 300,448,
			 // triple
			 337,450,
			 //reflect
			 372,450,
			 // happy stick
			 401,450,

			 // silver
			 500,365,
			 };
	int i;

	Print(x+251,y+312,"Items",0,2);
	DrawFillBox(x+250,y+355,x+630,y+475,3);
	DrawBox(x+249,y+354,x+631,y+476,31);

	for(i=0;i<33;i++)
	{
		RenderInvItem(i,x+c[i*2],y+c[i*2+1],mgl);
	}
}

void RenderPauseMenu(MGLDraw *mgl)
{
	char s[16];

	DarkenScreen(darkness);

	if(subMode!=SUBMODE_SLOTPICK || cursor!=2)
	{
		strcpy(s,"Wpn Lock: ");
		if(player.fireFlags&FF_WPNLOCK)
			strcat(s,"On");
		else
			strcat(s,"Off");

		if(opt.cheats[CH_SAVEANY] && (player.worldNum==WORLD_NORMAL || player.worldNum==WORLD_REMIX))
		{
			PrintColor(10,295,"Cancel",4-4*(cursor==0),-8+8*(cursor==0),2);
			PrintColor(10,330,s,4-4*(cursor==1),-8+8*(cursor==1),2);
			PrintColor(10,365,"Load Game",4-4*(cursor==2),-8+8*(cursor==2),2);
			if(noSaving)
				PrintColor(10,400,"Save Game",0,-8+8*(cursor==3),2);
			else
				PrintColor(10,400,"Save Game",4-4*(cursor==3),-8+8*(cursor==3),2);
			PrintColor(10,435,"Quit",4-4*(cursor==4),-8+8*(cursor==4),2);
		}
		else
		{
			PrintColor(10,295,"Cancel",4-4*(cursor==0),-8+8*(cursor==0),2);
			PrintColor(10,342,s,4-4*(cursor==1),-8+8*(cursor==1),2);
			PrintColor(10,388,"Load Game",4-4*(cursor==2),-8+8*(cursor==2),2);
			PrintColor(10,435,"Quit",4-4*(cursor==4),-8+8*(cursor==4),2);
		}

		if((player.worldNum==WORLD_NORMAL || player.worldNum==WORLD_REMIX))
		{
			RenderPowerUps(5-offX,-5);
			RenderQuests(0+offX,-offX-5);
			RenderInventory(0+offX,offX-5,mgl);
		}
		if(player.worldNum==WORLD_SURVIVAL || player.worldNum==WORLD_BOSSBASH)
		{
			RenderPowerUps(5-offX,-5);
		}
	}
	if(subMode==SUBMODE_SLOTPICK)
		RenderSlotPickMenu();
}

void RenderSlotPickMenu(void)
{
	char txt[48];
	int i;


	DrawFillBox(100,100,540,300,0);
	DrawBox(98,98,542,302,142);
	DrawBox(100,100,540,300,142);

	if(cursor==1)
		CenterPrint(320,105,"Load Game",32,0);
	else
		CenterPrint(320,105,"Save Game",32,0);

	for(i=0;i<5;i++)
	{
		sprintf(txt,"Slot %d - %s - %03.1f%%",i+1,area[i],percent[i]);
		PrintColor(110,135+i*30,txt,4-4*(subcursor==i),-8+16*(subcursor==i),0);
	}
}

void HandlePauseKeyPresses(MGLDraw *mgl)
{
	char k;

	k=mgl->LastKeyPressed();
	if(k)
		lastKey=k;
}

void GetSaves(void)
{
	FILE *f;
	int i;
	char txt[12];
	player_t p;

	for(i=0;i<5;i++)
	{
		sprintf(txt,"save%d.sav",i+1);
		f=AppdataOpen(txt,"rb");
		if(!f)
		{
			percent[i]=0;
			sprintf(area[i],"Unused");
		}
		else
		{
			fread(&p,sizeof(player_t),1,f);
			fclose(f);
			percent[i]=CalcPercent(&p);
			if(p.worldNum==WORLD_NORMAL)
				strcpy(area[i],p.areaName);
			else
				sprintf(area[i],"*%s",p.areaName);
		}
	}
}

void LoadGame(int i)
{
	FILE *f;
	char txt[12];

	sprintf(txt,"save%d.sav",i+1);
	f=AppdataOpen(txt,"rb");
	if(!f)
	{
		InitPlayer(INIT_GAME,0,0);
	}
	else
	{
		fread(&player,sizeof(player_t),1,f);
		if(player.worldNum==WORLD_REMIX)
		{
			FreeWorld(&curWorld);
			LoadWorld(&curWorld,"remix.llw");

			InitWorld(&curWorld,WORLD_REMIX);
		}
		else if(player.worldNum==WORLD_NORMAL)
		{
			FreeWorld(&curWorld);
			LoadWorld(&curWorld,"loony.llw");

			InitWorld(&curWorld,WORLD_NORMAL);
		}
		LoadGuys(f);
		if(!curMap)
			curMap=new Map(20,20,"hi");
		curMap->LoadFromProgress(f);
		fclose(f);
		player.lastSave=i;
		ResetInterface();
		MakeNormalSound(SND_LOADGAME);
		if(player.monsType==MONS_PLYRBONKULA)
			player.xtraVar=30*5;	// reset blood clock, to prevent fatal saves
		if(player.monsType==MONS_PLYRSUMMON)
		{
		//	InitSummons();
		//	FindSummons();
		}
		if(player.invinc<60)
			player.invinc=60;	// and make you invincible briefly
	}
	noSaving=0;
}

void SaveGame(int i)
{
	FILE *f;
	char txt[12];

	sprintf(txt,"save%d.sav",i+1);
	f=AppdataOpen(txt,"wb");
	if(!f)
	{
		return;
	}
	else
	{
		player.lastSave=i;
		player.destx=goodguy->mapx;
		player.desty=goodguy->mapy;
		if(!(player.cheatsOn&PC_HARDCORE))
			player.numSaves++;	// saves do not count in hardcore mode
		fwrite(&player,sizeof(player_t),1,f);
		player.destx=0;
		player.desty=0;
		SaveGuys(f);
		curMap->SaveProgress(f);
		fclose(f);
		AppdataSync();
		NewBigMessage("Game Saved",30);
		MakeNormalSound(SND_SAVEGAME);
	}
}

void DeleteSave(int i)
{
	char txt[12];

	sprintf(txt,"save%d.sav",i);
	remove(txt);
}


void BumpSaveGem(void)
{
	if(player.cheatsOn&PC_HARDCORE)
		return;	// no effect of save gems!!

	if(noSaving)
		return;	// can't save when the portal is opening

	EnterStatusScreen();
	InitPauseMenu();
	subMode=SUBMODE_SLOTPICK;
	cursor=3;
	player.saveClock=20;
}

void SetNoSaving(byte on)
{
	noSaving=on;
}

void InitPauseMenu(void)
{
	MakeNormalSound(SND_PAUSE);
	lastKey=0;
	subMode=0;
	cursor=0;
	darkness=0;
	offX=400;
	oldc=255;

	GetSaves();
}

byte UpdatePauseMenu(MGLDraw *mgl)
{
	byte c;
	static byte reptCounter=0;

	if(darkness<16)
		darkness++;
	if(offX>0)
	{
		offX-=32;
		if(offX<0)
			offX=0;
	}

	c=GetControls()|GetArrows();

	reptCounter++;
	if((!oldc) || (reptCounter>10))
		reptCounter=0;

	if(subMode==SUBMODE_NONE)	// not in any submenu
	{
		if((c&CONTROL_UP) && (!reptCounter))
		{
			cursor--;
			if(cursor==255)
				cursor=4;
			if((!opt.cheats[CH_SAVEANY] || (player.worldNum!=WORLD_NORMAL && player.worldNum!=WORLD_REMIX)) && cursor==3)
				cursor=2;

		}
		if((c&CONTROL_DN) && (!reptCounter))
		{
			cursor++;
			if(cursor==5)
				cursor=0;
			if((!opt.cheats[CH_SAVEANY] || (player.worldNum!=WORLD_NORMAL && player.worldNum!=WORLD_REMIX)) && cursor==3)
				cursor=4;
		}
		if(((c&CONTROL_B1) && (!(oldc&CONTROL_B1))) ||
		   ((c&CONTROL_B2) && (!(oldc&CONTROL_B2))))
		{
			switch(cursor)
			{
				case 0: // cancel
					return 0;
					break;
				case 1:	// weapon lock
					player.fireFlags^=FF_WPNLOCK;
					break;
				case 2:	// Load
					subMode=SUBMODE_SLOTPICK;
					break;
				case 3:	// Save
					if(!noSaving)
						subMode=SUBMODE_SLOTPICK;
					break;
				case 4: // quit game
					if(player.cheatsOn&PC_HARDCORE)
					{
						cursor=2;
						subMode=SUBMODE_SLOTPICK;
					}
					else
						return 4;
			}
		}
	}
	else if(subMode==SUBMODE_SLOTPICK)
	{
		if((c&CONTROL_UP) && (!reptCounter))
		{
			subcursor--;
			if(subcursor==255)
				subcursor=4;
		}
		if((c&CONTROL_DN) && (!reptCounter))
		{
			subcursor++;
			if(subcursor==5)
				subcursor=0;
		}
		if(((c&CONTROL_B1) && (!(oldc&CONTROL_B1))) ||
		   ((c&CONTROL_B2) && (!(oldc&CONTROL_B2))))
		{
			if(cursor==2)	// Load
			{
				if(!strcmp(area[subcursor],"Unused"))
				{
					MakeNormalSound(SND_MENUCANCEL);
				}
				else
				{
					LoadGame(subcursor);
					CameraOnPlayer(0);
					ExitBullets();
					InitBullets();
					NewBigMessage("Game Loaded!",30);
					UndoWindDown();
					return 0;
				}
			}
			else if(cursor==3)	// Save
			{
				SaveGame(subcursor);
				if(player.cheatsOn&PC_HARDCORE)
					return 4;	// returns exit if you saved in hardcore mode
				return 0;
			}
			subMode=SUBMODE_NONE;
		}
	}
	oldc=c;

	HandlePauseKeyPresses(mgl);
	if(lastKey==27)	// hit ESC to exit pause menu
	{
		if(subMode==SUBMODE_NONE || cursor==3)
			return 0;
		else
			subMode=SUBMODE_NONE;
		lastKey=0;
	}
	return 1;
}
