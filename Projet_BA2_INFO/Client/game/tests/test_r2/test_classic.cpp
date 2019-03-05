
#include <iostream>
#include <sstream>

#include "../../GameIntelligence/GameIntelligence.cpp"

MyOstream mout("logfile.txt");

int main(){
	/* fonction principale */
	
	// "anglais" , "francais" , "neerlandais"
	
	GameIntelligence* game = new GameIntelligence();
	
	game->make_game_two_bots("classic","bot_opponnent_2","bot_hum_2");
	game->execute_game();
	
	return 0;
}
