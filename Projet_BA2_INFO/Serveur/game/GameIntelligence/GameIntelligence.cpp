#include "GameIntelligence.hpp"

TourParTour* GameIntelligence::get_game() const{
	return this->game;
}

void GameIntelligence::set_game(TourParTour* jeu){
	this->game = jeu;
}

void GameIntelligence::make_game(std::string game_type, Player* player_one, Player* player_two){
	
	mout<<player_one<<std::endl;
	mout<<player_two<<std::endl;
	
	Dico* dico = make_dico("../../csv");
	
	BaseChess* serv;
	
	if (game_type == "classic"){serv = new ClassicChess(player_one,player_two,player_one,dico);}
	//else if (game_type == "dark"){serv = new DarkChess(player_one, player_two, player_one, dico);}
	//else if (game_type == "trappist"){serv = new TrappistChess(player_one,player_two,player_one,dico);}
	//else if (game_type == "anti"){serv = new AntiChess(player_one,player_two,player_one,dico);}
	else{throw MyException(&mout,"game_type inconnu!");}
	
	TourParTour* big_game = new TourParTour(serv);
	this->set_game(big_game);

}

void GameIntelligence::make_game_human_and_bot(std::string game_type, std::string langue,std::string tag_bot){
	/* fonction peremttant de lancer une partie classique avec 1 joueurs humains et un bot de test */
	
	Human* player_one = new Human("player1",langue);
	
	Bot* player_two = make_bot("player2",tag_bot,"../../bots_csv");
	
	this->make_game(game_type,player_one,player_two);
	
}

void GameIntelligence::make_game_two_bots(std::string game_type, std::string tag_bot_one, std::string tag_bot_two){
	/* fonction peremttant de lancer une partie classique avec 2 bots de test */
	
	Bot* player_one = make_bot("player1",tag_bot_one,"../../bots_csv");
	
	Bot* player_two = make_bot("player2",tag_bot_two,"../../bots_csv");
	
	this->make_game(game_type,player_one,player_two);
}

void GameIntelligence::make_game_two_humans(std::string game_type, std::string langue){
	/* fonction peremttant de lancer une partie classique avec 2 joueurs humains */
	
	Human* player_one = new Human("player1",langue);
	
	Human* player_two = new Human("player2",langue);
	
	this->make_game(game_type,player_one,player_two);
}

void GameIntelligence::make_game_two_silenced_humans(std::string game_type, std::string langue){
	/* fonction peremttant de lancer une partie classique avec 2 joueurs humains */
	
	SilencedHuman* player_one = new SilencedHuman("player1",langue);
	
	SilencedHuman* player_two = new SilencedHuman("player2",langue);
	
	this->make_game(game_type,player_one,player_two);
}

void GameIntelligence::execute_game(){
	
	try{
		bool end = false;
		while (not end){
			end = this->get_game()->execute_step();
			//std::cout<<"RESULTAT ENVOYE: "<<result.second<<std::endl;
		}
	}	
	catch(MyException& e){
		mout<<e.what()<<std::endl;
		mout<<"myexception catched"<<std::endl;
	}
	
}


