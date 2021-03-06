#pragma once
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../srcs/request.cpp"
#include "../../Gui/incl/FenPrincipale.hpp"
#include "../srcs/terminal.cpp"

#include "../../game/GameTypes/SuperGame/SuperGame.cpp"
#include <cstdlib>
#include <stdlib.h>


class Client: public AbstractPlayer, public AbstractClient{
	private:
		Request* _server;
		SuperGame* _game;
		AbstractInterface* _interface;
		bool _affichageGui;

		std::vector<std::pair<std::string,int> > _gameRequests;
		int _gameMod;
		std::string _name;
		void lose();
		void win();
	public:
		//Client(const char* ip, bool interface);
		Client(const char* ip, bool interface, int argc, char** argv);

		~Client() = default;
		
		//call by interface
		void setInterface(AbstractInterface* interface) override;
		bool letsRegister(std::string username,std::string password1, std::string password2, std::string email) override;
		bool login(std::string username,std::string password) override;
		void waitForMatch(int gameMod) override;

		void sendMessage(std::string name,std::string msg) override;
		void addFriend(std::string name) override;
		void removeFriend(std::string name) override;
		void acceptFriend(std::string name, bool accept) override;
		void getFriendList() override;
		void getFriendRequests() override;
		void getUserInfo(std::string) override;
		void getUserInfo() override;

		std::vector<std::pair<std::string,int > > getGRequests() override;
		void acceptFriendlyGame(std::string username, bool res) override;
		void sendRequestFriendlyGame(std::string username, int gameMod) override;
		
		void click(std::string square) override;
		void surrend() override;
		void exit() override;
		//call by server
		void startingGame(bool playerTurn, std::string opponentName) override;
		void opponentMov(std::string mov) override;
		void recvMessage(std::string name,std::string msg) override;
		void connectionError() override;
		void addGRequest(std::string username, int gameMod) override;

		//call by game
		void mov(std::string mov) override;
		void movPossibleUpdate(std::vector<std::pair<int,int> >* listMov) override;

		void recvFriendRequestsList(std::vector<std::string> vec) override;
		void recvFriendList(std::vector<std::pair<std::string, bool> > frendList) override;
		void recvInfo(std::string username, int nbrgames, int win, int elo) override;

		void exitQueue() override;
		void feedback(int info, std::string message) override;
		void opponentSurrend() override;

		int showGui(int argc, char** argv);
};

#endif
