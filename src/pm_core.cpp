#include "pm.hpp"

// NTHP Project manager; compiles scripts, stages, textures, and palettes! Planned with built-in
// debugger, allowing to step and breakpoint scripts with a virtual machine. Headless version first, then
// GUI.

nthp::EngineCore nthp::core;

int headless_runtime();
int help_headless(std::vector<std::string>& args);


int main(int argv, char** argc) {
	if(argv > 1) {

	}
	else {
		return headless_runtime();
	}


	return 0;
}



int headless_runtime() {
	std::vector<std::string> args;
	
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
				return 0;
			}

			if(args[0] == "compile") {
				nthp::script::CompilerInstance cc;
				if(args[1] == "-src") {
					if(args.size() < 4) {
						PM_PRINT_ERROR("No output file specified.\n");
						continue;
					}
					
					cc.compileSourceFile(args[2].c_str(), args[3].c_str(), false);
					PM_PRINT("Script, Done. %s > %s\n", args[2].c_str(), args[3].c_str());
					continue;
				}
				if(args[1] == "-stg") {
					if(args.size() < 4) {
						PM_PRINT_ERROR("No output file specified.\n");
						continue;
					}
					cc.compileStageConfig(args[2].c_str(), args[3].c_str());
					PM_PRINT("Stage, Done. %s > %s\n", args[2].c_str(), args[3].c_str());
					
					continue;
				}
				
				PM_PRINT_ERROR("No compilation behaviour specified. Aborting.\n");
				continue;
			}


		}
		
	}
	



        return 0;

}
