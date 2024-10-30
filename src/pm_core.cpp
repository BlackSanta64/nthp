#ifndef PM
        #define PM
#endif

#include "pm.hpp"

// NTHP Project manager; compiles scripts, stages, textures, and palettes! Planned with built-in
// debugger, allowing to step and breakpoint scripts with a virtual machine. Headless version first, then
// GUI.

nthp::EngineCore nthp::core;
nthp::script::stage::Stage currentStage;

bool debuggingActiveProcess = false;
bool suspendExecution = false;



int headless_runtime();
int help_headless(std::vector<std::string>& args);
inline void help_output() {
        PM_PRINT("NTHP Project Manager Help --\n\tNomenclature: [expression] = optional flag/argument.\n\tCommands: compile, gt, ct, debug, exit, help\ngt [-c (compress output)] palette_file input_image output_texture\nct input_texture output_compressed_texture\ncompile (src or stg) [-f (force)] input_file output_file\ndebug input_prog_directive_file [debug_log_output_file]\nexit\n");
}



int nthp::debuggerBehaviour(std::string target, FILE* debugOutputTarget) {
        // The DEBUG_INIT is called at the start of main, and DEBUG_CLOSE
        // is called after the destruction of the main core.
        std::mutex g_access;

        { // The entire engine debug context.
                auto frameStart = SDL_GetTicks();


                // Anyone would agree an infinite loop here is acceptable.
                while(true) {
                        if(currentStage.loadStage(target.c_str())) return 1;
                        memset(nthp::script::stageMemory, 0, STAGEMEM_MAX);

                        // Init phase.
                        PRINT_DEBUG("Beginning INIT phase...\n");
               
                        do {
                                
                                frameStart = SDL_GetTicks();

                                if(currentStage.init()) return 1;

                                nthp::deltaTime = nthp::intToFixed(SDL_GetTicks() - frameStart);
                        
                                if(nthp::deltaTime < nthp::frameDelay) {
                                        SDL_Delay(nthp::fixedToInt(nthp::frameDelay - nthp::deltaTime));
                                        nthp::deltaTime = nthp::frameDelay;
                                }

                        } while(!nthp::core.isRunning());
                        
                        while((nthp::core.isRunning()) && (!currentStage.data.changeStage) && debuggingActiveProcess) {
                                frameStart = SDL_GetTicks();

                                
                                currentStage.handleEvents();


                                
                                        // Tick phase.
                                currentStage.tick();
                                currentStage.logic();
                                


                                nthp::deltaTime = nthp::intToFixed(SDL_GetTicks() - frameStart);
                        
                                if(nthp::deltaTime < nthp::frameDelay) {
                                        SDL_Delay(nthp::fixedToInt(nthp::frameDelay - nthp::deltaTime));
                                        nthp::deltaTime = nthp::frameDelay;
                                }
                        }

                        // Exit Phase
                        PRINT_DEBUG("Beginning EXIT phase...\n");

                        
                        if(currentStage.exit()) return 1;

                        // If CHANGESTAGE is false after the exit phase, then the core must've been stopped,
                        // so exit the program. Otherwise, redo the init phase with the new stage. 
                        if(currentStage.data.changeStage && debuggingActiveProcess) {
                                currentStage.data.changeStage = false;
                                // StageMemory has been set to the new filename.
                                continue;
                        }
                        else {
                                debuggingActiveProcess = false;
                                break;
                        }
                }
        }


        nthp::core.cleanup();
        return 0;
}



bool Kill_main_process = false;
bool inHeadlessMode = false;

int main(int argv, char** argc) {
        nthp::setMaxFPS(30);
        std::mutex g_access;

        std::thread debuggerThread(headless_runtime);
        std::string targetName, debugOutput;
        FILE* debug_fd = stdout;

	if(argv > 1) {
                if(std::string(argc[1]) == "-h") {
                        inHeadlessMode = true;
                        if(debuggerThread.joinable()) debuggerThread.join();
                        return 0;
                }
                targetName = argc[1];

                if(argv > 2) {
                        debugOutput = argc[2];
                }
                else {
                        debugOutput = "stdout";
                }
        }
        else {
                targetName = "prog";
        }


        // Checks every frame if the debugger requests a session.
        do {
                std::this_thread::sleep_for(std::chrono::milliseconds(nthp::fixedToInt(nthp::frameDelay)));
                
                if(debuggingActiveProcess) {

                        if(debugOutput != "stdout") {
                                debug_fd = fopen(debugOutput.c_str(), "w+");
                                if(debug_fd == NULL) { debug_fd = stdout; }
                        }

                        NTHP_GEN_DEBUG_INIT(debug_fd);
                        
                        g_access.lock();
                        
                        nthp::script::debug::debugInstructionCall.x = nthp::script::debug::BREAK;
                        suspendExecution = true;
                        
                        g_access.unlock();

                        int ret = nthp::debuggerBehaviour(targetName, debug_fd);
                        if(ret) {
                                PM_PRINT_ERROR("\nCritical failure in debugger; return code %d\n", ret);
                        }
                        else {
                                PM_PRINT("\nCompleted debugging session without critical errors.\n");
                        }
                        
                        NTHP_GEN_DEBUG_CLOSE();

                        nthp::setMaxFPS(30);
                        std::cout << "> ";
                }
        }
        while(!Kill_main_process);


        if(debuggerThread.joinable()) debuggerThread.join();
        return 0;
}


int singleThread_debugger() {
        return 0;
}



int headless_runtime() {
        PM_PRINT("Pm.exe (C) 2024 Jonathan Hawes;\nNTHP Game Engine project manager v." NTHP_VERSION "\nType 'help' for instructions.\n\n");
	std::vector<std::string> args;
	std::mutex g_access;

	std::string input, arg;
	bool isRunning = true;
        

	while(isRunning) {
		args.clear();
		std::cin.clear();

                if(debuggingActiveProcess) std::cout << "debug> ";
		else std::cout << "> ";

		std::getline(std::cin, input);

		if(input != "") {
			{ // Separates all input symbols into the args vector.
				std::istringstream inputStream(input);
				while(std::getline(inputStream, arg, ' '))
					args.push_back(arg);
			}

			if(args[0] == "exit") {
                                g_access.lock();

                                Kill_main_process = true;
                                debuggingActiveProcess = false;

                                g_access.unlock();
				return 0;
			}

			if(args[0] == "compile") {
				nthp::script::CompilerInstance cc;
				if(args[1] == "src") {
					if(args.size() < 4) {
						PM_PRINT_ERROR("No output file specified.\n");
						continue;
					}
					
					if(!cc.compileSourceFile(args[2].c_str(), args[3].c_str(), false)) {
					        PM_PRINT("Script, Done. %s > %s\n", args[2].c_str(), args[3].c_str());
                                        }
					continue;
				}
				if(args[1] == "stg") {
                                        bool forceBuild = false;
                                        int sizeTarget = 0;
					if(args.size() < 4) {
						PM_PRINT_ERROR("No output file specified.\n");
						continue;
					}
                                        if(args[2] == "-f") { forceBuild = true; sizeTarget = 1; }
					if(!cc.compileStageConfig(args[2 + sizeTarget].c_str(), args[3 + sizeTarget].c_str(), forceBuild)) {
					        PM_PRINT("Stage, Done. %s > %s\n", args[2 + sizeTarget].c_str(), args[3 + sizeTarget].c_str());
                                        }
					continue;
				}
				
				PM_PRINT_ERROR("No compilation behaviour specified. Aborting.\n");
				continue;
			}
                        if(args[0] == "debug") {
                                if(!inHeadlessMode) {
                                        g_access.lock();

                                        debuggingActiveProcess = true;

                                        g_access.unlock();

                                        PM_PRINT("Now debugging target.\n");
                                }
                                else {
                                        PM_PRINT_ERROR("Currently running in headless mode; No target.\n");
                                }
                        }
                        if(args[0] == "help") {
                                help_output();
                        }

                        if(args[0] == "stop") {
                                if(debuggingActiveProcess) {
                                        PM_PRINT("Stopping active debug session.\n");
                                        debuggingActiveProcess = false;
                                }
                                else {
                                        PM_PRINT("Not in an active debug session.\n");
                                }

                        } 


                        if(args[0] == "gt") {
                                if(args.size() < 4) {
                                        PM_PRINT_ERROR("Invalid command argument. (gt [-flag] paletteFile imageFile outputFile)\n");
                                        continue;
                                }

                                int offset = 0;
                                bool compressOutput = false;
                                if(args[1] == "-c") {
                                        offset = 1;
                                        compressOutput = true;
                                }
                                nthp::texture::Palette tempPal;
                                if(tempPal.importPaletteFromFile(args[1 + offset].c_str())) {
                                        PM_PRINT_ERROR("Unable to import palette [%s].\n", args[1 + offset].c_str());
                                        continue;
                                }
                                PM_PRINT("Generating texture from file [%s] with palette [%s]...\n", args[2 + offset].c_str(), args[1 + offset].c_str());
                                PM_PRINT("Calculating color approximations (this may take a while)...\n");
                                if(nthp::texture::tools::generateSoftwareTextureFromImage(args[2 + offset].c_str(), &tempPal, args[3 + offset].c_str())) {
                                        PM_PRINT_ERROR("Failed to generate new softwareTexture.\n");
                                        continue;
                                }

                                if(compressOutput) {
                                        PM_PRINT("compressing...\n");
                                        std::string newFileName = args[3 + offset] + ".cst";
                                        nthp::texture::compression::compressSoftwareTextureFile(args[3 + offset].c_str(), newFileName.c_str());
                                }
                                PM_PRINT("done.\n");
                                continue;
                        }

                        if(args[0] == "ct") {
                                if(args.size() < 3) {
                                        PM_PRINT_ERROR("Invalid command argument. (ct textureFile outputFile)\n");
                                        continue;
                                }
                                PM_PRINT("Compressing texture [%s]...\n", args[1].c_str());
                                if(nthp::texture::compression::compressSoftwareTextureFile(args[1].c_str(), args[2].c_str())) {
                                        PM_PRINT_ERROR("Failed to compress softwareTexture [%s].\n", args[1].c_str());
                                        continue;
                                } 
                                PM_PRINT("Done.\n");
                                continue;
                        }



                        if(debuggingActiveProcess) {
                                if(args[0] == "break" || args[0] == "b") {
                                        g_access.lock();

                                        nthp::script::debug::debugInstructionCall.x = nthp::script::debug::BREAK;
                                        suspendExecution = true;
                                        PM_PRINT("Breakpoint read at instruction [%zu]; HEAD at [%zu], waiting for continue.\n", currentStage.data.currentNode, currentStage.data.currentNode);

                                        g_access.unlock();

                                }
                                if(args[0] == "continue" || args[0] == "c") {
                                        g_access.lock();

                                        nthp::script::debug::debugInstructionCall.x = nthp::script::debug::CONTINUE;
                                        suspendExecution = false;
                                        PM_PRINT("Continuing from instruction [%zu]; HEAD at [%zu].\n", currentStage.data.currentNode, currentStage.data.currentNode);

                                        g_access.unlock();

                                }

                                if(args[0] == "jump" || args[0] == "j") {
                                        g_access.lock();

                                        nthp::script::debug::debugInstructionCall.x = nthp::script::debug::JUMP_TO;
                                        try {
                                                nthp::script::debug::debugInstructionCall.y = std::stoi(args[1]);
                                        }
                                        catch(std::invalid_argument) {
                                                PM_PRINT_ERROR("JUMP instruction takes a valid script instruction index as target.\n");
                                                nthp::script::debug::debugInstructionCall.x = -1;

                                                g_access.unlock();
                                                continue;
                                        }

                                        PM_PRINT("Continuing from instruction [%zu]; HEAD at [%zu].\n", currentStage.data.currentNode, currentStage.data.currentNode);
                                        g_access.unlock();
                                }

                                if(args[0] == "step" || args[0] == "s") {
                                        if(!suspendExecution) {
                                                PM_PRINT_ERROR("Process must be suspended (break, b) to step through.\n");
                                                continue;
                                        }

                                        g_access.lock();
                                        

                                        nthp::script::debug::debugInstructionCall.x = nthp::script::debug::STEP;
                                        PM_PRINT("Stepping to next instruction [%zu], [%zu] -> [%zu]\n", currentStage.data.currentNode + 1, currentStage.data.currentNode, currentStage.data.currentNode + 1);

                                        g_access.unlock();

                                }

                        }


		} // if(input != "")


	} // while(isRunning)
	



        return 0;

}
