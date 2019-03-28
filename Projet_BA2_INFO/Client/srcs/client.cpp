#ifndef CLIENT_CPP
#define CLIENT_CPP

#include "../includes/client.hpp"
#include "../../Gui/incl/FenPrincipale.hpp"

Client::Client(const char* ip, bool terminalMod, int argc, char** argv): _game(nullptr), _interface(nullptr){
	this->_server = new Request(this, ip);
	if (terminalMod){
		AbstractInterface* useless = new Terminal(this);
		(void)useless;
	} else {
		showGui(argc, argv);
	}
}


Client::~Client(){
	;;
}

void Client::setInterface(AbstractInterface* interface){
	this->_interface = interface;
}

void Client::waitForMatch(int gameMod){
	this->_gameMod = gameMod;
	this->_server->findMatch(gameMod);
}


void Client::startingGame(bool playerTurn, std::string opponentName){
	this->_game = new SuperGame(this->_gameMod, this, playerTurn);
	this->_interface->gameStart(opponentName);//+ bord
}

void Client::click(std::string square){
	this->_game->click(square);
}

void Client::mov(std::string mov){
	this->_server->mov(mov);
	this->_interface->pingForUpdate();
}

void Client::opponentMov(std::string mov){
	bool res = _game->opponentMov(mov);
	_interface->pingForUpdate();
	if (res){
		this->lose();
	}
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
		this->_name = username;
		answer = static_cast<bool>(_server->letsRegister(username, password1, email));
	}
	return answer; //pair
}

bool Client::login(std::string username,std::string password){
	if (username.empty() || password.empty()){
		return false;
	} else {
		this->_name = username;
		int res = _server->login(username,password);
		return static_cast<bool>(res);
	}
}

void Client::connectionError(){
	if (_interface != nullptr){
		this->_interface->connectionError();
	}
}

void Client::exit(){
	//if (_interface != nullptr) --------------------------------------------------------- // !!! enleve warning (classe abstraite)
	//	delete this->_interface;
	if (_game != nullptr)
		delete _game;
	delete _server;
	try {
		delete this; //:(
	} catch(std::exception& e){
		;;
	}
}

//bool Client::get_inverted() const{}
//void Client::set_inverted(bool){}
//
//bool Client::get_ennemy_inverted() const{}
//void Client::set_ennemy_inverted(bool){}

void Client::movPossibleUpdate(std::vector<std::string> listMov){
	//TO DO
	(void)listMov;
}

void Client::sendMessage(std::string name,std::string msg){
	if( (name != "") || (msg != "") )
		this->_server->sendMessage(name, msg);
}
void Client::addFriend(std::string name){
	if (this->_name != name){
		this->_server->addFriend(name);
	}
}
void Client::removeFriend(std::string name){
	this->_server->removeFriend(name);
}
void Client::acceptFriend(std::string name, bool accept){
	this->_server->acceptFriend(name, accept);
}
void Client::getFriendRequests() {
	this->_server->getFriendRequests();
}
void Client::getFriendList(){
	this->_server->getFriendList();
}

void Client::getUserInfo(){
	this->getUserInfo(this->_name);
}

void Client::getUserInfo(std::string username){
	this->_server->getUserInfo(username);
}
void Client::recvMessage(std::string name, std::string msg){
	this->_interface->recvMessage(name, msg);
}
void Client::recvFriendRequestsList(std::vector<std::string> vec){
	this->_interface->recvFriendRequestsList(vec);
}
void Client::recvFriendList(std::vector<std::pair<std::string, bool> > frendList){
	this->_interface->recvFriendList(frendList);
}
void Client::recvInfo(std::string username, int nbrgames, int win, int elo){
	this->_interface->recvInfo(username, nbrgames, win, elo);
}


int Client::showGui(int argc, char** argv){
	QApplication app(argc, argv);
	this->_interface = new FenPrincipale(this);
    this->_interface->myShow();
    return app.exec(); 
}
#endif
