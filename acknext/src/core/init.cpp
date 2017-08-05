#include <engine.hpp>
#include "log.hpp"
#include "config.hpp"
#include "input/inputmanager.hpp"

#include <chrono>
#include <getopt.h>
#include <physfs.h>

using std::chrono::steady_clock;
using std::chrono::high_resolution_clock;

// This clock point is used for frame time calculations
static high_resolution_clock::time_point lastFrameTime;

struct
{
	int no_sdl;
	int diag;
} options;

struct engine engine;

#define SDL_CHECKED(x, y) if((x) < 0) { engine_setsdlerror(); return y; }

// errorhandling.cpp
void engine_setsdlerror();

// graphics-core.cpp
void render_init();
void render_frame();
void render_shutdown();

ACKNEXT_API_BLOCK
{
	bool engine_open(int argc, char ** argv)
	{
		startupTime = std::chrono::steady_clock::now();

		engine_log("Begin initalizing engine.");

		engine_config.load("acknext.cfg");

		// Parse arguments
		{
			static struct option long_options[] =
			{
				/* These options set a flag. */
			{ "diag",    no_argument,       &options.diag, 1},
			/* These options don’t set a flag.
						  We distinguish them by their indices. */
			// {"diag",    no_argument,       0, 'd'},
			{ "config",  required_argument, 0, 'c'},
			{ "no-sdl", no_argument, &options.no_sdl, 'X' },
			{0, 0, 0, 0}
		};
			while (1)
			{
				/* getopt_long stores the option index here. */
				int option_index = 0;
				int c = getopt_long (argc, argv, "Xdc:",
				                     long_options, &option_index);

				/* Detect the end of the options. */
				if (c == -1)
					break;

				switch (c)
				{
					case 0:
						/* If this option set a flag, do nothing else now. */
						if (long_options[option_index].flag != 0)
							break;
						printf ("option %s", long_options[option_index].name);
						if (optarg)
							printf (" with arg %s", optarg);
						printf ("\n");
						break;

					case 'd':
						options.diag = 1;
						break;
					case 'X':
						options.no_sdl = 1;
						break;
					case 'c': {
						engine_config.load(optarg);
						break;
					}
					case '?': break;
					default:
						abort ();
				}
			}

			if (options.diag) {
				if(logfile == nullptr) {
					logfile = fopen("acklog.txt", "w");
					if(logfile == nullptr) {
						engine_log("Failed to open acklog.txt!");
					}
				}
			}

			for(int i = optind; i < argc; i++)
			{
				engine_config.sourceFiles.emplace_back(argv[i]);
			}
		}

		engine_log("Initialize virtual file system...");
		PHYSFS_init(argv[0]);

		PHYSFS_setSaneConfig (
			engine_config.organization.c_str(),
			engine_config.application.c_str(),
			"ARP",    // scan for default archive
			0,        // no cd
			0);       // filesys > pack

		engine_log("app dir:  %s", PHYSFS_getBaseDir());
		engine_log("save dir: %s", PHYSFS_getWriteDir());

		// if(compiler_init() == false) {
		// 	return false;
		// }

		// for(auto & str : engine_config.sourceFiles) {
		// 	if(compiler_add(str.c_str()) == false) {
		// 		return false;
		// 	}
		// }

		if(options.no_sdl == 0)
		{
			engine_log("Initialize SDL2...");
			SDL_CHECKED(SDL_Init(SDL_INIT_EVERYTHING), false)

			SDL_CHECKED(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3), false)
			SDL_CHECKED(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3), false)
			SDL_CHECKED(SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG), false)
			SDL_CHECKED(SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1), false)

			{ // Create window and initialize SDL
				auto flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_MOUSE_FOCUS;
				if(engine_config.fullscreen == FullscreenType::Fullscreen) {
					flags |= SDL_WINDOW_FULLSCREEN;
				}
				else if(engine_config.fullscreen == FullscreenType::DesktopFullscreen) {
					flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
				}

				engine.window = SDL_CreateWindow(
				            engine_config.title.c_str(),
				            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
				            engine_config.resolution.width, engine_config.resolution.height,
				            flags);
				if(engine.window == nullptr)
				{
					engine_setsdlerror();
					return false;
				}

				engine.context = SDL_GL_CreateContext(engine.window);
				if(engine.context == nullptr)
				{
					engine_setsdlerror();
					return false;
				}
			}
		}

		// engine_log("Initialize collision engine...");
		// collision_init();

		engine_log("Initialize input...");
		InputManager::init();

		engine_log("Initialize renderer...");
		render_init();

		// engine_log("Initialize scheduler...");
		// scheduler_initialize();

		engine_log("Engine ready.");

		lastFrameTime = high_resolution_clock::now();

		// if(compiler_start() == false) {
		// 	return false;
		// }

		return true;
	}

	bool engine_frame()
	{
		auto nextFrameTime = high_resolution_clock::now();
	    // Time Setup
	    {
	        std::chrono::duration<float> timePoint;
	        timePoint = std::chrono::duration_cast<std::chrono::milliseconds>(
	                    nextFrameTime - lastFrameTime);
	        time_step = timePoint.count();

	        timePoint = std::chrono::duration_cast<std::chrono::microseconds>(
	                    steady_clock::now() - startupTime);
	        total_time = timePoint.count();
	    }

		if(options.no_sdl == 0)
		{
			SDL_GetWindowSize(engine.window, &screen_size.width, &screen_size.height);

			InputManager::beginFrame();

			// Update Frame
			SDL_Event event;
			while(SDL_PollEvent(&event))
			{
				switch(event.type)
				{
					case SDL_QUIT:
						// TODO: Replace with event-call here
						return false;
					case SDL_KEYDOWN:
						InputManager::keyDown(event.key);
						break;
					case SDL_KEYUP:
						InputManager::keyUp(event.key);
						break;
					case SDL_MOUSEBUTTONDOWN:
						InputManager::mouseDown(event.button);
						break;
					case SDL_MOUSEBUTTONUP:
						InputManager::mouseUp(event.button);
						break;
					case SDL_MOUSEMOTION:
						InputManager::mouseMove(event.motion);
						break;
					case SDL_MOUSEWHEEL:
						InputManager::mouseWheel(event.wheel);
						break;
				}
			}
		}

	    // scheduler_update();

		// if(!(engine_flags & CUSTOMDRAW)) {
		//
		// }

		render_frame();

	    lastFrameTime = nextFrameTime;
	    total_frames++;
	    return true;
	}

	void engine_close()
	{
		// engine_log("Shutting down engine...");
	    // scheduler_shutdown();

		engine_log("Shutting down input...");
		InputManager::shutdown();

		engine_log("Shutting down renderer...");
		render_shutdown();

		if(!options.no_sdl)
		{
			engine_log("Destroy GL context.");
			SDL_GL_DeleteContext(engine.context);

			engine_log("Close window.");
			SDL_DestroyWindow(engine.window);
		}

		engine_log("Shutting down virtual file system...");
		PHYSFS_deinit();

	    engine_log("Engine shutdown complete.");
	}
}
