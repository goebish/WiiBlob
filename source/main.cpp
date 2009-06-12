//#define _DEBUG_

// todo : dynamically change gamefps value (config.xml -> mGameFPS in OptionState.cpp) according to 50 or 60Hz TV standard.

#ifdef _DEBUG_
#include <debug.h> // usb gecko debugging
#endif
/*=============================================================================
Blobby Volley 2
Copyright (C) 2006 Jonathan Sieber (jonathan_sieber@yahoo.de)

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
=============================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>
#include <SDL/SDL.h>
#include <unistd.h>
#include "physfs.h"

#include "RenderManager.h"
#include "SoundManager.h"
#include "InputManager.h"
#include "UserConfig.h"
#include "IMGUI.h"
#include "state/State.h"
#include "SpeedController.h"
#include "Blood.h"

#include <cstring>
#include <sstream>

#if defined(WIN32)
#ifndef GAMEDATADIR
#define GAMEDATADIR "data"
#endif
#endif

void probeDir(const std::string& dirname)
{
	if (PHYSFS_isDirectory(dirname.c_str()) == 0)
	{
		if (PHYSFS_exists(dirname.c_str()))
		{
			PHYSFS_delete(dirname.c_str());
		}
		if (PHYSFS_mkdir(dirname.c_str()))
		{
			std::cout << PHYSFS_getWriteDir() <<
				dirname << " created" << std::endl;
		}
		else
		{
			std::cout << "Warning: Creation of" <<
				PHYSFS_getWriteDir() << dirname <<
				" failed!" << std::endl;
		}
	}
}

void deinit()
{
	RenderManager::getSingleton().deinit();
	SoundManager::getSingleton().deinit();
	SDL_Quit();
	PHYSFS_deinit();
}

void setupPHYSFS()
{
	std::string separator = PHYSFS_getDirSeparator();
	// Game should be playable out of the source package on all
	// platforms
	PHYSFS_addToSearchPath("SD:/data/", 1);
	PHYSFS_addToSearchPath("SD:/data/gfx/", 1);
	PHYSFS_addToSearchPath("SD:/data/sounds/", 1);
	PHYSFS_addToSearchPath("SD:/data/scripts/", 1);
#if not defined(WIN32)
	// Just write in installation directory
	PHYSFS_setWriteDir("SD:/data/");
#else
	// Create a search path in the home directory and ensure that
	// all paths exist and are actually directories
	std::string userdir = PHYSFS_getUserDir();
	std::string userAppend = ".blobby";
	std::string homedir = userdir + userAppend;
	PHYSFS_addToSearchPath(userdir.c_str(), 0);
	PHYSFS_setWriteDir(userdir.c_str());
	probeDir(userAppend);
	probeDir(userAppend + separator + "replays");
	probeDir(userAppend + separator + "gfx");
	probeDir(userAppend + separator + "sounds");
	probeDir(userAppend + separator + "scripts");
	PHYSFS_removeFromSearchPath(userdir.c_str());
	PHYSFS_setWriteDir(homedir.c_str());
	PHYSFS_addToSearchPath(homedir.c_str(), 0);
#if defined(GAMEDATADIR)
	// A global installation path makes only sense on non-Windows
	// platforms
	std::string basedir = GAMEDATADIR;
	PHYSFS_addToSearchPath(basedir.c_str(), 1);
	PHYSFS_addToSearchPath((basedir + separator + "gfx.zip").c_str(),
									1);
	PHYSFS_addToSearchPath((basedir + separator + "sounds.zip").c_str(),
									1);
	PHYSFS_addToSearchPath((basedir + separator + "scripts.zip").c_str(),
									1);
#endif
#endif
}
int main();
extern "C"
int main(int argc, char* argv[])
{
#ifdef _DEBUG_
	DEBUG_Init(GDBSTUB_DEVICE_USB, 1);
	_break(); // debug stop
#endif

	PHYSFS_init();
	setupPHYSFS();

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK);
	SDL_EnableUNICODE(1);
	atexit(SDL_Quit);
	srand(SDL_GetTicks());
	// Default is OpenGL and false
	// choose renderer
	RenderManager *rmanager = 0;
	SoundManager *smanager = 0;

	try
	{
		UserConfig gameConfig;
		gameConfig.loadFile("SD:/data/config.xml");
		std::string loc_test = gameConfig.getString("device");
		
		rmanager = RenderManager::createRenderManagerSDLWii(); // (goebish)
		
		/*if(gameConfig.getString("device") == "SDL")
			rmanager = RenderManager::createRenderManagerSDL(); 
		else if (gameConfig.getString("device") == "GP2X")
			rmanager = RenderManager::createRenderManagerGP2X();
		else if (gameConfig.getString("device") == "OpenGL")
		rmanager = RenderManager::createRenderManagerGL2D();
		else
		{
		std::cerr << "Warning: Unknown renderer selected!";
			std::cerr << "Falling back to OpenGL" << std::endl;
			rmanager = RenderManager::createRenderManagerGL2D();
		}*/

		// fullscreen?
		if(gameConfig.getString("fullscreen") == "true")
			rmanager->init(640, 480, true); // change to 640*480 ?
		else
			rmanager->init(800, 600, false);

		SpeedController scontroller(gameConfig.getFloat("gamefps"));
		SpeedController::setMainInstance(&scontroller);
		scontroller.setDrawFPS(gameConfig.getBool("showfps"));

		smanager = SoundManager::createSoundManager();
		smanager->init();
		smanager->setVolume(gameConfig.getFloat("global_volume"));
		smanager->setMute(gameConfig.getBool("mute"));
		smanager->playSound("SD:/data/sounds/bums.wav", 0.0);
		smanager->playSound("SD:/data/sounds/pfiff.wav", 0.0);

		std::string bg = std::string("SD:/data/backgrounds/") + gameConfig.getString("background");
		if (PHYSFS_exists(bg.c_str()))
			rmanager->setBackground(bg);

		InputManager* inputmgr = InputManager::createInputManager();
		int running = 1;
		//int limiter=1;
		while (running)
		{
			inputmgr->updateInput();
			running = inputmgr->running();

			// This is true by default for compatibility, GUI states may
			// disable it if necessary
			rmanager->drawGame(true);
			IMGUI::getSingleton().begin();
				State::getCurrentState()->step();
			rmanager = &RenderManager::getSingleton(); //RenderManager may change
			//draw FPS:
			/*if (scontroller.getDrawFPS())
			{
				// We need to ensure that the title bar is only set
				// when the framerate changed, because setting the
				// title can ne quite resource intensive on some
				// windows manager, like for example metacity.
				static int lastfps = 0;
				int newfps = scontroller.getFPS();
				if (newfps != lastfps)
				{
					std::stringstream tmp;
					tmp << AppTitle << "    FPS: " << newfps;
					rmanager->setTitle(tmp.str());
				}
				lastfps = newfps;
			}*/

			//if( ++limiter%5==0)
			//if (!scontroller.doFramedrop())
			{
				rmanager->draw();
				IMGUI::getSingleton().end();
				//BloodManager::getSingleton().step();
				rmanager->refresh();
			}
			scontroller.update();
		}
	}
	catch (std::exception& e)
	{
		std::cerr << std::endl << std::endl << e.what() << " oups :(" << std::endl;
		sleep(5);
		if (rmanager)
			rmanager->deinit();
		if (smanager)
			smanager->deinit();
		SDL_Quit();
		PHYSFS_deinit();
		exit (EXIT_FAILURE);
	}

	deinit();
	exit(EXIT_SUCCESS);
}


