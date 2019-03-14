
#pragma once
#ifndef TEMPSREEL_HPP
#define TEMPSREEL_HPP

#include "../../Games/AntiChess/AntiChess.cpp"
#include "../../Games/ClassicChess/ClassicChess.cpp"
#include "../../Games/DarkChess/DarkChess.cpp"
#include "../../Games/TrappistChess/TrappistChess.cpp"

class TempsReel {
	
	private:
		BaseChess* game;
	
	public:
        TempsReel(BaseChess*); //*< Constructor
        TempsReel() noexcept = default; //*< Constructor
        virtual ~TempsReel() noexcept = default; //Destructor
        TempsReel(const TempsReel&) noexcept = default;
        TempsReel& operator= (const TempsReel&) noexcept = default;
        
        virtual std::pair<bool,std::string> execute_step();
        virtual std::pair<bool,bool> execute_step(std::string,std::string,bool);
        virtual std::pair<bool,bool> execute_step(std::string,std::string);
        
	protected:
        BaseChess* get_game() const;
        void set_game(BaseChess*);
        void change_active_player();

};
#endif
