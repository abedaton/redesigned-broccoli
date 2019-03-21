#ifndef TERMINAL_CPP
#define TERMINAL_CPP

#include "../includes/terminal.hpp"

Terminal::Terminal(AbstractClient* client): _user(client) {
    firstWindow();
};

Terminal::~Terminal(){
	std::cout << "Destructor" << std::endl;
}

void Terminal::connectionError(){
	std::cout << "Connection with server lost : " << strerror(errno) << std::endl;
	exit(EXIT_FAILURE); // tmp
	//delete this; // :(
}

void Terminal::gameStart(std::string opponent){ //bord
	this->_gameStart = true;
    std::cout << "Game is starting. Please press a random key to continue" << std::endl;
}

void Terminal::pingForUpdate(){
    //To Do
}

void Terminal::win() {
	std::cout << "you win" << std::endl;
	this->_end = true;
}

void Terminal::lose() {
	std::cout << "you lose" << std::endl;
	this->_end = true;
}

void Terminal::friendsWindow(){
	/*
	unsigned int res = 0;
	while(res != 7){
		std::cout << "Que désirez vous faire?: " << std::endl;
		std::cout << "1) Ajouter un ami " << std::endl;
		std::cout << "2) Lister tous les amis connectés " << std::endl;
		std::cout << "3) Supprimer un ami " << std::endl;
		std::cout << "4) Consulter mes demandes d'amis/de parties " << std::endl;
		std::cout << "5) Proposer à un ami de faire une partie " << std::endl;
		std::cout << "6) Chat avec des amis " << std::endl;
		std::cout << "7) Retourner au menu principal " << std::endl;

		std::cin >> res;
		while(res == 0 || res > 7){	
			std::cout << "Choix invalide veuillez réessayer:" << std::endl;
			std::cin >> res;
		}
		
		if(res == 1){
			std::string friendName;
			std::cout<< "Veuillez entrer le nom de l'ami à ajouter: ";
			std::cin >> friendName;
			if(!_user->addFriend(friendName))
				std::cout << friendName << " n'existe pas " << std::endl;
			else
				std::cout<< "La demande d'ami a été envoytée" << std::endl;	
		}

		else if(res == 2){
			_user->listOnlineFriends();
		}	

		else if(res == 3){
			std::string friendName;
			std::cout<< "Veuillez entrer le nom de l'ami à supprimer: ";
			std::cin >> friendName;
			_user->removeFriend(friendName);
		}
		else if(res == 4){
			_user->proceedGameAndFriendRequests();
		}		
			
		else if(res ==5){
			
			// 1. call listonlinefriends pour montrer tous les amis disponibles
			// 2. laiser l'utilisateur choisir l'ami puis l'inviter à la partie
			

		}

		else if(res ==6)
			;//chatWindow();
			
		else if(res == 7)
			;

		else
			std::cout << "Choix invalide veuillez réessayer:" << std::endl;
			
	}
	*/
}

void Terminal::firstWindow(){
	bool log = false;
	bool exit = false;
	char answer;
	while(! log && !exit){
		answer = ' ';
		while ( answer != '1' && answer != '2' && answer != '3' ) {
			std::cout << "Write 1 for login, 2 for register or 3 for exit:" << std::endl;
			std::cin >> answer;
			this->myFlush();
		}
		if (answer == '1')
			log=logInWindow();
		else if (answer == '2')
			log=registerWindow();
		else
			exit = true;
	}
	if (! exit){
		menuWindow();
	}
}

bool Terminal::registerWindow(){
    std::string username;
    std::string password;
    std::string password2;
    std::string email;
    std::regex regEmail("(?:[a-z0-9!#$%&'*+/=?^_`{|}~-]+(?:\\.[a-z0-9!#$%&'*+/=?^_`{|}~-]+)*|\"(?:[\x01-\x08\x0b\x0c\x0e-\x1f\x21\x23-\x5b\x5d-\x7f]|\\[\x01-\x09\x0b\x0c\x0e-\x7f])*\")@(?:(?:[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\\.)+[a-z0-9](?:[a-z0-9-]*[a-z0-9])?|\\[(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?|[a-z0-9-]*[a-z0-9]:(?:[\x01-\x08\x0b\x0c\x0e-\x1f\x21-\x5a\x53-\x7f]|\\[\x01-\x09\x0b\x0c\x0e-\x7f])+)\\])");
    while(true){
        std::cout << "Write your new username: ";
        std::cin >> username;
        myFlush();
        std::cout << "Write your email: ";
        std::cin >> email;
        myFlush();
        password = getpass("New password (will not be shown): ");
        password2 = getpass("New password (again): ");
        if (password != password2){
            std::cout << "Password does not match!" << std::endl;
            continue;
        
		}
        if (! std::regex_match(email.begin(), email.end(), regEmail)){
			std::cout << "Invalid email." << std::endl;
			continue;
        }
		if (this->_user->letsRegister(username, password, email)){
			std::cout << "You are now logged in !" << std::endl;
			this->_username = username;
			return true;
		}
		else{
			char answer;
			std::cout << "Invalide Username.\n Write 1 for continue or 2 to go back: ";
			std::cin >> answer;
			this->myFlush();
			while (answer != '1' && answer != '2'){
				std::cout << "Please write 1 for continue or 2 to go back: ";
				std::cin >> answer;
				this->myFlush();
			}
			if (answer == '2')
				return false;
		}
    }
}

bool Terminal::logInWindow(){
	std::string username;
    std::string password;
    while(true){
        std::cout << "Write your username :";
        std::cin >> username;
        this->myFlush();
        password = getpass("Password (password will not be shown) :");
        if(this->_user->login(username,password)){
			std::cout << "You are now logged in !" << std::endl;
			this->_username = username;
			return true;
		}
		else{
			char answer;
			std::cout << "Invalid username or password." << std::endl;
			std::cout << "Write 1 for continue or 2 to go back: ";
			std::cin >> answer;
			myFlush();
			while (answer != '1' && answer != '2'){
				std::cout << "Please write 1 for continue or 2 to go back: ";
				std::cin >> answer;
				myFlush();
			}
			if (answer == '2')
				return false;
		}
    }
}

void Terminal::menuWindow(){
	char answer;
	bool waitForGame = false;
    while (true){
        std::cout << "Enter 1 for exit 2 for friend list";//,(2 for chat)";
		if (!waitForGame)
			std::cout <<", 3 for game";
		std::cout << ": " << std::endl;
        std::cin >> answer;
		myFlush();
		if (this->_gameStart){
			_gameStart = false;
			gameWindow();
			waitForGame = false;
		} else{
        	if (answer == '1'){
        	    break;
        	}
        	else if (answer == '2'){
        	    friendsWindow();
        	}
        	else if (answer == '3' && !waitForGame){
        	    waitForGame = selectGameModeWindow();
        	}
		}
    }
}

void Terminal::recvMessage(std::string msg, std::string mov){
	std::cout << "Opponent: " << msg << std::endl;
}

bool Terminal::selectGameModeWindow(){
	char answer;
    std::cout << "Enter 1 for classic, 2 for Dark, 3 for Trappist, 4 for Anti or 5 for return to the menu: " << std::endl;
    std::cin >> answer;
	myFlush();
    while (answer != '1' && answer != '2' && answer != '3' && answer != '4' && answer != '5'){
        std::cout << "Please, enter 1 for classic, 2 for Dark, 3 for Trappist, 4 for Anti or 5 for return to the menu: " << std::endl;
        std::cin >> answer;
		myFlush();
    }
	if (answer == '5')
		return false;
	else {
		this->_user->waitForMatch(atoi(&answer));
		return true;
	}
}

void Terminal::gameWindow(){
	int answer;
	std::string square;
    while (true){
        std::cout << "Enter 1 for surrender, 2 for chat, 3 for click on the bord" << std::endl;
        std::cin >> answer;
		myFlush();
		if (_end){
			std::cout << "lose" << std::endl;
			break;
		}
		if (answer == 1){
			//this->_user->surrend(); 
            break;
        }
        else if (answer == 2){
            std::string msg;
			std::cout << "Enter a message for your opponent: \n" << std::endl;
			std::cin.clear();
        	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::getline(std::cin, msg);
			this->myFlush();
			this->_user->sendMessage("SOMEONE", msg);
        }
        else if (answer == 3){
			std::cout << "Choose a square" << std::endl;
			std::cin >> square;
			this->myFlush();
			//regex To Do
			_user->click(square);
			
        }
    }
}

void Terminal::myFlush(){
    if (std::cin.fail()){
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

#endif