#include <iostream>
#include <string>
#include "MemoryAccess.h"
#include "AlbumManager.h"
#include <ctime>
#include <chrono>
#include "DataBaseAccess.h"
#include <signal.h>


// initialization data access
DataBaseAccess dataAccess;

// initialize album manager
AlbumManager albumManager(dataAccess);

void callback_closeProc(int signum)
{
	albumManager.closeProc();

	signal(SIGINT, callback_closeProc);
}

int getCommandNumberFromUser()
{
	std::string message("\nPlease enter any command(use number): ");
	std::string numericStr("0123456789");
	
	std::cout << message << std::endl;
	std::string input;
	std::getline(std::cin, input);
	
	while (std::cin.fail() || std::cin.eof() || input.find_first_not_of(numericStr) != std::string::npos) {

		std::cout << "Please enter a number only!" << std::endl;

		if (input.find_first_not_of(numericStr) == std::string::npos) {
			std::cin.clear();
		}

		std::cout << std::endl << message << std::endl;
		std::getline(std::cin, input);
	}
	
	return std::atoi(input.c_str());
}

void printDetailsStart()
{
	auto now = std::chrono::system_clock::now();
	std::time_t now_c = std::chrono::system_clock::to_time_t(now);
	std::string timeStr = std::ctime(&now_c);

	std::cout << "Shahar Danino " << timeStr << std::endl;
}

int main(void)
{
	signal(SIGINT, callback_closeProc);

	std::string albumName;
	printDetailsStart();
	std::cout << "Welcome to Gallery!" << std::endl;
	std::cout << "===================" << std::endl;
	std::cout << "Type " << HELP << " to a list of all supported commands" << std::endl;
	
	do {
		int commandNumber = getCommandNumberFromUser();
		
		try	{
			albumManager.executeCommand(static_cast<CommandType>(commandNumber));
		} catch (std::exception& e) {	
			std::cout << e.what() << std::endl;
		}
	} 
	while (true);
}


