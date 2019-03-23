#ifndef CLIENT_CPP
#define CLIENT_CPP

#include "../includes/client.hpp"
#include "../../Gui/incl/FenPrincipale.hpp"

Client::Client(const char* ip, bool terminalMod, int argc, char** argv): _game(nullptr){
	_server = new Request(this, ip);
	if (terminalMod){
		_interface = new Terminal(this);
	} else {
		showGui(argc, argv);
	}
}


Client::~Client(){
	if (_game != nullptr)
		delete _game;
	delete _interface;
	delete _server;
}

void Client::waitForMatch(int gameMod){
	this->_gameMod = gameMod;
	this->_server->findMatch(gameMod);
}


void Client::startingGame(bool playerTurn, std::string opponentName){
	int chessMod = this->_gameMod%4;
	int partyMod = this->_gameMod/4;

	//switch partyMod{
	//	case 0{
	//		_game = new party(chessMod, playerTurn);
	//		break;
	//	}
	//	case 1{
	//		_game = new party(chessMod, playerTurn);
	//		break;
	//	}
	//	case 2{
	//		_game = new party(chessMod, playerTurn);
	//		break;
	//	}
	//}
	//_game->getBord();
	_interface->gameStart(opponentName);//+ bord
}

void Client::click(std::string square){
	// TO DO
}

void Client::mov(std::string mov){
	_server->mov(mov);
	_interface->pingForUpdate();
}

void Client::opponentMov(std::string mov){
	//_game->opponentMov(mov);
	_interface->pingForUpdate();
}

void Client::win(){
	_interface->win();
	delete this->_game;
	this->_game = nullptr;
}

void Client::lose(){
	_interface->lose();
	delete this->_game;
	this->_game = nullptr;
}

bool Client::letsRegister(std::string username,std::string password1, std::string password2, std::string email){
	std::string error;
	bool answer = false;
	std::regex regEmail("(?:[a-z0-9!#$%&'*+/=?^_`{|}~-]+(?:\\.[a-z0-9!#$%&'*+/=?^_`{|}~-]+)*|\"(?:[\x01-\x08\x0b\x0c\x0e-\x1f\x21\x23-\x5b\x5d-\x7f]|\\[\x01-\x09\x0b\x0c\x0e-\x7f])*\")@(?:(?:[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\\.)+[a-z0-9](?:[a-z0-9-]*[a-z0-9])?|\\[(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?|[a-z0-9-]*[a-z0-9]:(?:[\x01-\x08\x0b\x0c\x0e-\x1f\x21-\x5a\x53-\x7f]|\\[\x01-\x09\x0b\x0c\x0e-\x7f])+)\\])");
	if (username.empty() || password1.empty()){
		error = "Please fill in all fields";
	} else if (! std::regex_match(email.begin(), email.end(), regEmail)){
		error = "Invalid email";
	} else if(password1 != password2){
		error = "Password doesn't match";
	} else {
		error = "Success";
		answer = static_cast<bool>(_server->letsRegister(username, password1, email));
	}
	return answer; //pair
}

bool Client::login(std::string username,std::string password){
	if (username.empty() || password.empty()){
		return false;
	} else {
		int res = _server->login(username,password);
		return static_cast<bool>(res);
	}
}

void Client::connectionError(){
	_interface->connectionError();
}

void Client::exit(){
	delete this; //:(
}

//bool Client::get_inverted() const{}
//void Client::set_inverted(bool){}
//
//bool Client::get_ennemy_inverted() const{}
//void Client::set_ennemy_inverted(bool){}

void Client::sendMessage(std::string name,std::string msg){
	//TO DO
}
void Client::addFriend(std::string name){
	//TO DO
}
void Client::removeFriend(std::string name) {
	//TO DO
}
void Client::acceptFriend(std::string name, bool accept){
	//TO DO
}
void Client::getFriendList(){
	//TO DO
}
void Client::getFriendRequests() {
	//TO DO
}
void Client::getOnlineFriendList(){
	//TO DO
}
void Client::getUserInfo(){
	//TO DO
}

void Client::recvMessage(std::string name, std::string msg){
	//TO DO
}

int Client::showGui(int argc, char** argv){
	QApplication app(argc, argv);
	_interface = new FenPrincipale(this);
    _interface->myShow();
    return app.exec(); 
}
#endif