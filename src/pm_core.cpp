#include "pm.hpp"

// NTHP Project manager; compiles scripts, stages, textures, and palettes! Planned with built-in
// debugger, allowing to step and breakpoint scripts with a virtual machine. Headless version first, then
// GUI.

nthp::EngineCore nthp::core;


int headless_runtime();
int help_headless(std::vector<std::string>& args);
void help_output() {
        PM_PRINT("NTHP Project Manager Help --\n\tNomenclature: [expression] = optional flag/argument.\n\tCommands: compile, gt, ct, debug, exit, help\ngt [-c (compress output)] palette_file input_image output_texture\nct input_texture output_compressed_texture\ncompile (src or stg) [-f (force)] input_file output_file\ndebug input_prog_directive_file [debug_log_output_file]\nexit\n");
}

int GUI_Runtime();
bool Kill_main_process = false;
bool inHeadlessMode = false;

int main(int argv, char** argc) {

        std::thread debuggerThread(headless_runtime);

	if(argv > 1) {
                if(std::string(argc[1]) == "-h") {
                        if(debuggerThread.joinable()) debuggerThread.join();
                        return 0;
                }
        }

        const int ret = GUI_Runtime();
        debuggerThread.join();
        return ret;
}


int GUI_Runtime() {

        nthp::core.init(nthp::RenderRuleSet(800, 800, 200, 200, nthp::vectFixed(0, 0)), "NTHP Debugger - " NTHP_VERSION, false, false);



        while(nthp::core.isRunning() && (!Kill_main_process)) {
                nthp::core.handleEvents();

                nthp::core.clear();

                nthp::core.display();
        }

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
		std::cout << "> ";
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
                                NTHP_GEN_DEBUG_INIT(stdout);


                        }
                        if(args[0] == "help") {
                                help_output();
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


		}
		
	}
	



        return 0;

}
