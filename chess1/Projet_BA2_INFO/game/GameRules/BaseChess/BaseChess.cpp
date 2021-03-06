
#pragma once
#ifndef BASECHESS_CPP
#define BASECHESS_CPP

#include "BaseChess.hpp"

extern MyOstream mout;

/* principe général:
 * le serveur manipule un plateau,
 * le plateau ne doit pas connaitre ce qu'il contient pour exister */

//-----------------------HORS CLASSE BaseChess-----------------------------------------------

bool check_end_game(BitypeVar<Chesspiece*> adv_pe, bool switch_pos){
	
	/* fonction qui vérifie si la derniere piece prise etait un roi
	 * la piece prise étant la piece sur la 2e case (la case destination) selectionné lors d'un mouvement.
	 * le cas du roc étant implémenté comme une prise (selection de sa case comme "destination" [2e case selectionné]) est refusé */
	
	bool end_game;
	
	if (not(switch_pos)){
		end_game = verifier_type_pe<Roi>(adv_pe);
	}
	else{end_game = false;}
	
	return end_game;
}

BitypeVar<int>* detect_ennemy_pion_in_vect(std::vector<BitypeVar<Chesspiece*>> vect, Player* owner){
	
	/* fonction vérifiant si une piece ennemie se trouve dans le vecteur donné en paramètre
	 * piece ennemie = piece dont le propriétaire est différent de celui donné en paramètre */
	
	BitypeVar<Chesspiece*> elem;
	bool found = false;
	
	BitypeVar<int>* res = new BitypeVar<int>();
	res->set_state(false);
	
	long long unsigned int i=0;
	while((i<vect.size()) and (res->get_state() == false)){		
		found = verifier_type_pe<Pion>(vect[i]);
		if (found == true){
			if((vect[i].get_var())->get_owner() == owner){
				res->set_state(true);
				res->set_var(int(i));
			}
		}
		i++;
	}
	return res;
}

BitypeVar<Chesspiece*>* select_king(std::vector<Chesspiece*>* vect,Player* owner){
	
	/* fonction qui renvoi le roi du joeuer donné en paramètre se situant dans la liste */
	long long unsigned int i=0;
	bool found = false;	
	BitypeVar<Chesspiece*>* res = new BitypeVar<Chesspiece*>();
	res->set_state(false);
	
	while((i<vect->size()) and (found == false)){		
		
		if ((*vect)[i]->get_owner() == owner){
			res->set_var((*vect)[i]);
			res->set_state(true);
			found = true;
		}
		
		i++;
	}
	return res;
}

BitypeVar<std::pair<int,int>>* calculate_move_between_zones(MatPosi* begin, MatPosi* end){
	/* fonction utile pour calculer les positions se situant entre 2 positions,
	 * cette fonction détermine le facteur multiplicatif de direction du mouvement
	 * (par conséquent indique la direcion du mouvement souhaité)
	 * 
	 * ex: si horizontal se sera (1,0) comme 1 en x et 0 en y
	 * 
	 * limitations: pêut seulment détecter si les 2 positions sor sur une ligne horizontale,verticale ou diagonale */
	 // fonction crée pour amelioration roque
	
	bool lig,col;
	std::pair<int,int> paire;
	BitypeVar<std::pair<int,int>>* res = new BitypeVar<std::pair<int,int>>();
	
	res->set_state(false);
	
	if (begin->get_lig() == end->get_lig()){lig = true;}
	if (begin->get_col() == end->get_col()){col = true;}
	
	if (lig and not(col)){
		res->set_state(true);
		paire = std::make_pair(1,0);
		res->set_var(paire);
	}
	else if (not(lig) and col){
		res->set_state(true);
		paire = std::make_pair(0,1);
		res->set_var(paire);
	}
	else if (not(lig) and not(col)){
		
		int diff_lig = begin->get_lig() - end->get_lig();
		int diff_col = begin->get_col() - end->get_col();
		
		if(std::abs(diff_lig) == std::abs(diff_col)){ //diagonale
			
			// verification de signe
			if(diff_lig == diff_col){
				res->set_state(true);
				paire = std::make_pair(1,1);
				res->set_var(paire);
			}
			
			else{
				res->set_state(true);
				paire = std::make_pair(-1,1);
				res->set_var(paire);
			}	
		
		}
		//else{} // pas diagonale
		
	}
	else{throw MyException(&mout,"calcul de zones intermediares d'un même emplacement");}
	
	return res;
	
}

std::pair<int,int> calc_roc_destinations(bool gauche_isking,std::vector<MatPosi>* between_vect){
	/* fonction qui calucule la destination du roi et de la tour suite a un roc, peu import si c'est un prtit ou grand roque */
		
	int taille = int(between_vect->size());
	
	if (taille<2){throw MyException(&mout,"roc impossible, espace insuffisant");}
	
	int co_roi,co_tour;
	
	if (taille%2 != 0){
		co_roi = taille/2;
	}
	else{
		if (gauche_isking == true){
			co_roi = taille/2;
		}
		else{
			co_roi = (taille/2) - 1 ;
		}
		
	}
		
	if (gauche_isking == false){ // tour a gauche
		co_tour = co_roi+1;
	}
	else{
		co_tour = co_roi-1;
	}
	
	std::pair<int,int> res = std::make_pair(co_roi,co_tour);
	
	return res;
	
}

std::vector<MatPosi*>* calc_king_move_path(bool king_isleft,int king_dest,std::vector<MatPosi>* between_vect){
	/* fonction qui calucule les cases que le roi doit parcourir lors d'un roque,
	 * (utile pour vérifier si il est danger lors de son trajet)*/
	
	std::vector<MatPosi*>* res = new std::vector<MatPosi*>();
	
	if (king_isleft == true){
		for(int i=0;i<king_dest;i++){
			res->push_back(&((*between_vect)[i]));
		}
		
	}
	else{
		for(int i=king_dest+1;i<int(between_vect->size());i++){
			res->push_back(&((*between_vect)[i]));
		}
	}
	
	return res;
}

std::pair<MatPosi*,MatPosi*> sort_two_mpos(MatPosi* mpos_one,MatPosi* mpos_two){
	/* fonction qui classe 2 MatPosi, celui le plus a gauche est le 1e elemnt du retour, celui le plus a droite le second */
	
	MatPosi* mpos_gauche;
	MatPosi* mpos_droite;
	
	if(*mpos_one < *mpos_two) { // roi a gauche
		mpos_gauche = mpos_one;
		mpos_droite = mpos_two;
	}
	
	else if(*mpos_one > *mpos_two) {
		mpos_gauche = mpos_two;
		mpos_droite = mpos_one;
	}
	
	//(*mpos_one == *mpos_two) 
	else{throw MyException(&mout,"sort avec 2 fois la même piece");} // roi a gauche
	
	return std::make_pair(mpos_gauche,mpos_droite);
	
}

BitypeVar<std::vector<MatPosi>*>* calc_zones_between_zones(MatPosi* begin, MatPosi* end, Player* player){
	/* fonction qui calcule les positions réele se situant entre 2 positions
	 * 
	 * ex: entre (0,4) et (0,7) il y a (0,5) et (0,6)
	 * 
	 * limitations: pêut seulment détecter si les 2 positions sor sur une ligne horizontale,verticale ou diagonale
	 */
	
	// fonction crée pour amelioration roque
	
	BitypeVar<std::vector<MatPosi>*>* res = new BitypeVar<std::vector<MatPosi>*>();
	std::vector<MatPosi>* vect = new std::vector<MatPosi>();
	res->set_state(false);
	
	BitypeVar<std::pair<int,int>>* bit_paire;
	
	int lig,col;
	
	bit_paire = calculate_move_between_zones(begin, end);
	
	if (bit_paire->get_state() == true){
		
		std::pair<int,int > paire = bit_paire->get_var();
		
		MatPosi* debut;
		MatPosi* fin;
		
		if (*begin > *end){
			debut = end;
			fin = begin;
		}
		else{
			debut = begin;
			fin = end;
		}
		
		
		lig = debut->get_lig();
		col = debut->get_col();
		
		res->set_state(true);
		
		MatPosi* temp_mpos;
		
		temp_mpos = new MatPosi(col,lig);
		
		while(*temp_mpos < *fin){
			
			col += paire.first;
			lig += paire.second;
			
			temp_mpos->set_col(col);
			temp_mpos->set_lig(lig);
			
			std::stringstream ss;
			ss<<debut->to_string()<<" "<<temp_mpos->to_string()<<" "<<fin->to_string()<<std::endl;
			
			player->send_msg(ss.str());
			
			if(*temp_mpos < *fin){vect->push_back(*temp_mpos);}
			
		}
		res->set_var(vect);		
	}
	
	return res;
	
}

bool is_elem_in_vect(std::vector<MatPosi*>* vect,MatPosi* elem){
	
	bool found = false;
	
	long long unsigned int i=0;
	while(i<vect->size() and not found){
		if (*elem == (*((*vect)[i]))){found = true;}
		i++;
	}
	
	return found;
}


std::vector<MatPosi*>* get_path_intesection(std::vector<std::vector<MatPosi*>*>* vect){
	
	std::vector<MatPosi*>* res = new std::vector<MatPosi*>();
	
	for(long long unsigned int i=0;i<((*vect)[0])->size();i++){
		
		std::vector<MatPosi*> small_vect = *((*vect)[0]);
		MatPosi* elem = (small_vect[i]);
		
		bool common = true;
		long long unsigned int j=0;
		
		while(j<vect->size() and common == true){
			
			if(not is_elem_in_vect((*vect)[j],elem)){common = false;}
			j++;
		}
		
		if (common == true){res->push_back(elem);}
	}
	
	
	return res;
	
}

bool is_roquable(Chesspiece* pe){
	
	bool res = false;
	
	if(verifier_type_pe<Roi>(pe) or verifier_type_pe<Tour>(pe)){res = true;}
	
	return res;
}

std::vector<std::string>* split_string(std::string s,std::string delim){
	
	std::vector<std::string>* vect = new std::vector<std::string>();
	
	long long unsigned int start = 0;
    long long unsigned int end = s.find(delim);
    while (end != std::string::npos){
		vect->push_back(s.substr(start, end - start));
        start = end + delim.length();
        end = s.find(delim, start);
    }
	vect->push_back(s.substr(start, end));
	
	return vect;
	
}

//--------------------BaseChess----------------------------------------------------------------------------------------------------

BaseChess::BaseChess(Player* p_low,Player* p_high,Player* deb_player,Dico* dict, AbstractAffichage* aff) : plateau(nullptr) , low_player(p_low), high_player(p_high), active_player(deb_player), dico(dict), action_cnt(0), affich(aff){
	//this->initialisation(); /!\ impossible de le mettre cette fonction ici car contructeur appelé par fils, par conséquent utilisation de fonction virtuelle pure dans ce cas.
	// a mettre dans le constructeur du fils !!!
	
} //*< Constructor

Player* BaseChess::get_low_player() const {return this->low_player;}
Player* BaseChess::get_high_player() const {return this->high_player;}

Player* BaseChess::get_other_player(Player* play) const {
	/* fonction permettant de recuperer l'autre joueur de la partie (l'opossant de clui donné en paramètre) */
	
	if (play == this->get_low_player()){return this->get_high_player();}
	else if (play == this->get_high_player()){return this->get_low_player();}
	else{throw MyException(&mout, "probleme get other player");}
}

Player* BaseChess::get_active_player() const {return this->active_player;}
Player* BaseChess::get_non_active_player() const {return this->get_other_player(this->get_active_player());}

Player* BaseChess::get_player(std::string play_str){
	
	if (play_str == this->get_low_player()->get_name()){return this->get_low_player();}
	else if (play_str == this->get_high_player()->get_name()){return this->get_high_player();}
	else{throw MyException(&mout, " joueur inconnu dans get player (std::string)");}

}

void  BaseChess::set_active_player(Player* a){this->active_player = a;}
void  BaseChess::set_low_player(Player* l){this->low_player = l;}
void  BaseChess::set_high_player(Player* h){this->high_player = h;}

int BaseChess::get_action_cnt() const {return this->action_cnt;}
void BaseChess::inc_action_cnt(){this->action_cnt += 1;}

int BaseChess::get_player_row(Player* player) const {
	/* fonction permettant de savoir la ligne de départ (sur le plateau) d'un joueur donné en paramètre) */
	
	if (player == get_low_player()){return 0;}
	else if (player == get_high_player()){return this->get_plateau()->get_taille()-1;}
	else{throw MyException(&mout, "probleme player cote plateau");}
}

Plateau* BaseChess::get_plateau() const {return this->plateau;}
void BaseChess::set_plateau(Plateau* plat){this->plateau = plat;}

Dico* BaseChess::get_dico() const {return this->dico;}

std::string BaseChess::get_ret_symbol() const {return "ret";}
std::string BaseChess::get_roc_symbol() const {return "roc";}
std::string BaseChess::get_end_symbol() const {return "end";}
std::string BaseChess::get_lang_symbol() const {return "lang";}
std::string BaseChess::get_liste_depl_symbol() const {return "liste_depl";}
std::string BaseChess::get_liste_capt_symbol() const {return "liste_capt";}

void BaseChess::initial_set_piece(Chesspiece* pe,Player* own,std::string mov){
	/* fonction servant a placer avant que la partie commence les piece d'echec sur le tableau,
	 * c'est a ce moement qu'on leur attribue un propriétaire */
	
	pe->set_owner(own);
	
	//std::stringstream ss;
	//ss<<pe->get_name()<<","<<pe->get_owner()<<std::endl;
	//this->get_active_player()->send_msg(ss.str());
	
	MatPosi* mpos = new MatPosi(mov);	
	this->get_plateau()->set_piece(mpos->to_pair(),pe,true);
}

void BaseChess::initialise_size_affichage_plat_squares(){
	
	AbstractAffichage* aff = this->get_affich();
	
	aff->set_taille_case_hor(8);
	aff->set_taille_case_ver(3);
}

void BaseChess::initialise_affichage_partially(){
	
	AbstractAffichage* aff = this->get_affich();
	
	this->initialise_size_affichage_plat_squares();
	
	aff->set_plateau(this->get_plateau());
	aff->set_dico(this->get_dico());
	
	aff->set_dico_prefix("Symbole_");
	aff->set_dico_suffix("");
	aff->set_low_player(this->get_low_player());
	aff->set_high_player(this->get_high_player());
	aff->set_low_player_symbol("*");
	aff->set_high_player_symbol("");
	
}

void BaseChess::initialisation(){
	/* fonction regroupant toutes les actions a faire avant que la partie commence */
	
	this->initialise_board();
	this->initialise_low_pieces();
	this->initialise_high_pieces();
	this->initialise_affichage_partially();
}

void BaseChess::show_bitype_vect(std::vector<BitypeVar<Chesspiece*>> vect){
	/* fonction affichant une vecteur de cases (BitypeVar contenant une piece d'echec)
	 * si la case est vide None est affiché */
	std::stringstream ss_glob;
	
	BitypeVar<Chesspiece*> elem;
	ss_glob<<"[";
	for(long long unsigned int i=0;i<vect.size();i++){
		if (vect[i].get_state() == true){
			
			// !!!!!!!!!!!!!!!!!!!!!!!!!! probleme mout vs std::cout avec heritage --> complexe! --> demander a prof
			//(probleme avec mout<<*(vect[i].get_var())) 
			//solution
			std::stringstream ss;
			ss<<*(vect[i].get_var());
			ss_glob<<ss.str();}
			
		else{ss_glob<<"None";}
		
		if (i != vect.size()-1){ss_glob<<", ";}
	}
	ss_glob<<"]"<<std::endl;
	
	this->get_active_player()->send_msg(ss_glob.str());
}

std::vector<std::pair<int, int>>* BaseChess::check_all_mov(Chesspiece *pe){

    std::vector<std::pair<int, int>>* tmp;

    std::vector<std::pair<int, int>>* res = new std::vector<std::pair<int, int>>();
    
    std::vector<std::string> mode_vect {"depl", "capt", "capt_same", "capt_empty", "depl_full"};
    
    for (long long unsigned int i = 0; i < mode_vect.size(); i++){
		tmp = this->loop_moves(pe,mode_vect[i]);
		res->insert(res->end(), tmp->begin(), tmp->end());
	}

    return res;
}


std::vector<std::pair<int, int>>* BaseChess::loop_moves(Chesspiece *pe, std::string mode){

    std::string limited_mode = this->get_plateau()->get_limited_mode(mode);

    std::vector<std::pair<std::pair<int, int>, AdvTuple>> vect = pe->algo(limited_mode);

    std::vector<std::pair<int, int>> *res = new std::vector<std::pair<int, int>>();
    MatPosi *elem;
    Posi *origin = pe->get_posi();
    MatPosi *mposi_origi = new MatPosi(*origin);
    std::pair<int, int> paire_origi = mposi_origi->to_pair();
    for (long long unsigned int i = 0; i < vect.size(); i++){
        elem = new MatPosi(vect[i].first);
        AdvTuple adv_tup = vect[i].second;
        std::pair<int, int> paire = elem->to_pair();
        vect[i].first.first >= 0 && vect[i].first.second >= 0;
        if (this->check_danger_mouvement_and_path(paire_origi, adv_tup, paire, mode)){
            res->push_back(vect[i].first);
        }
    }
    return res;
}

BitypeVar<std::vector<MatPosi>*>* BaseChess::calculate_zones_between_zones(MatPosi* begin, MatPosi* end){
	/* surcharge de fonction permettant d'ommetre le Player*, qui sera par default le joueur actif */
	
	return calc_zones_between_zones(begin, end, this->get_active_player());
	
}

void BaseChess::changement_langue_input(){
	/* fonction gêrant le changement de langue demandé par le joueur,
	 * langue servant pour tous le affichages de textes */
	
	std::vector<std::string> languages = this->get_dico()->get_possible_laguages();
	
	bool valid = false;
	std::string inp_lang;
	
	while (not valid){
		std::stringstream deb_ss;
		deb_ss<<this->get_dico()->search(this->get_active_player()->get_langue(),"select_lang")<<": ";
		for(long long unsigned int i=0;i<languages.size();i++){
			std::string langue = ((languages)[i]);
			deb_ss<<langue;
						
			if (i < languages.size()-2){deb_ss<<", ";}
			//else if (i == languages.size()-2){deb_ss<<" "<<this->get_dico()->search(this->get_active_player()->get_langue(),"et")<<" ";}
			// enlevee car souci de derniere colonne donc placebo mis en place
			else if (i == languages.size()-1){deb_ss<<std::endl;}
		}
			
		this->get_active_player()->send_msg(deb_ss.str());
			
		inp_lang = this->get_active_player()->recieve_msg();
		valid = is_in_vect(&languages, inp_lang);
	}
	this->get_active_player()->set_langue(inp_lang);

}

Trinome<bool,bool,bool>* BaseChess::check_in_for_special_symbol(std::string inp,bool ret_accept){
	/* fonction verifiant si l'input IN est un string particulier (appéle symbole ici)
	 * et qui effectue des actions particuliere solon celle trouvé */
		
	bool found = true, go_back = false, end_game = false;
	
	if ( (ret_accept == true) and (inp == this->get_ret_symbol()) ){go_back = true;}
	else if (inp == this->get_end_symbol()) {end_game = true;}
	else if (inp == this->get_lang_symbol()){this->changement_langue_input();}
	else{found = false;}
	
	Trinome<bool,bool,bool>* res = new Trinome<bool,bool,bool>(found,go_back,end_game);
	return res;
		
}

Trinome<Quadrinome<bool,bool,bool,bool>*, BitypeVar<Chesspiece*>, std::string > BaseChess::check_out_for_special_symbol(BitypeVar<Chesspiece*> in_pe, std::string inp,bool roc_accept){
	/* fonction verifiant si l'input OUT est un string particulier (appéle symbole ici)
	 * et qui effectue des actions particuliere solon celle trouvé */
	
	Chesspiece* pe_in;
	if (in_pe.get_state() == true){
		pe_in = in_pe.get_var();
	}
	else{throw MyException(&mout,"IN invaldie alors que demande de OUT");}
		
	BitypeVar<Chesspiece*> dst;
	
	bool found = true, go_back = false, end_game = false, correspond = false;
	std::string out = inp;
	
	if (inp == this->get_ret_symbol()){go_back = true;}
	else if (inp == this->get_end_symbol()) {end_game = true;}
	
	else if ((roc_accept == false) and (inp == this->get_liste_depl_symbol())) {
		this->get_active_player()->send_msg("liste deplacements : ");
		this->show_possible_mouvement(pe_in, "depl");
		
	}
	else if ((roc_accept == false) and (inp == this->get_liste_capt_symbol())) {
		this->get_active_player()->send_msg("liste capture : ");
		this->show_possible_mouvement(pe_in, "capt");
		
	}
	
	else if (inp == this->get_lang_symbol()){this->changement_langue_input();}
	
	else if ((roc_accept == true) and (inp == this->get_roc_symbol())){
		if (roc_accept == true){
			Trinome<Trinome<bool,bool,bool>*,BitypeVar<Chesspiece*>,std::string>* roc_trinome = this->roc_output_check(in_pe);
			
			//
			
			Trinome<bool,bool,bool>* trinome_bool_res = roc_trinome->get_first();
			go_back = trinome_bool_res->get_first();
			correspond = trinome_bool_res->get_second();
			end_game = trinome_bool_res->get_third();
			
			dst = roc_trinome->get_second();
			out = roc_trinome->get_third();				
			//
			
			
		}
		else{this->get_active_player()->send_msg(this->get_dico()->search(this->get_active_player()->get_langue(),"roc_pe_imp"),true);}
	}
	
	else{found = false;}
	
	Quadrinome<bool,bool,bool,bool>* info_res = new Quadrinome<bool,bool,bool,bool>(found,go_back,correspond,end_game);
	Trinome<Quadrinome<bool,bool,bool,bool>*, BitypeVar<Chesspiece*>, std::string > res = Trinome<Quadrinome<bool,bool,bool,bool>*, BitypeVar<Chesspiece*>, std::string >(info_res,dst,out);
	
	return res;
		
}

std::pair<bool,BitypeVar<Chesspiece*>> BaseChess::check_in_validity_non_symbol(std::string in,std::string second_comment,std::string third_comment){
	
	BitypeVar<Chesspiece*> dst;
	Chesspiece* cap_piece;
	
	bool part_a;
				
	bool again = false;
	bool valid = this->verify_validity_input(in);
	
	if (valid == true){
		MatPosi* mpos_in = new MatPosi(in);
		dst = this->get_plateau()->get_piece(mpos_in->to_pair());
		
		if (dst.get_state() == true){
			cap_piece = dst.get_var();
			//mout<<"propio = "<<cap_piece->get_owner()<<std::endl;
			if (cap_piece->get_owner() != get_active_player()){
				this->get_active_player()->send_msg(second_comment,true);
				again = true;
			}
		}
		else{
			this->get_active_player()->send_msg(third_comment,true);
			again = true;
		}
			
		part_a = not(again);
	}
	else{part_a = false;}
	
	std::pair<bool,BitypeVar<Chesspiece*>> paire = std::make_pair(part_a,dst);
	
	return paire;
	
}

Trinome<std::string,BitypeVar<Chesspiece*>,std::pair<bool,bool>>* BaseChess::in_input(Interpret* first_comment, Interpret* second_comment,Interpret* third_comment,bool ret_accept){
	/* l'input IN est l'input déterminant principalement la piece a sellectionner dans le tableau */
	BitypeVar<Chesspiece*> dst;
	
	std::pair<int,int> conv;
	
	bool go_back = false;
	bool end_game = false;
	
	bool symbol_found;
	bool part_a = false;
	
	std::string in = "";
	while (not part_a and not go_back and not end_game){
		
		this->get_active_player()->send_msg(first_comment->get_text(this->get_active_player()->get_langue()),true);
		
		in = this->get_active_player()->recieve_msg();
		
		Trinome<bool,bool,bool>* symbol_info = this->check_in_for_special_symbol(in,ret_accept);
		symbol_found = symbol_info->get_first();
		
		if (symbol_found == false){
			
			std::string second_com = second_comment->get_text(this->get_active_player()->get_langue());
			std::string third_com = third_comment->get_text(this->get_active_player()->get_langue());
			
			std::pair<bool,BitypeVar<Chesspiece*>> non_symbol_rep = this->check_in_validity_non_symbol(in,second_com,third_com);
			part_a = non_symbol_rep.first;
			dst = non_symbol_rep.second;

		}
		else{		
			go_back = symbol_info->get_second();
			end_game = symbol_info->get_third();
		}
	}
	
	Trinome<std::string,BitypeVar<Chesspiece*>,std::pair<bool,bool>>* res = new Trinome<std::string,BitypeVar<Chesspiece*>,std::pair<bool,bool>>(in,dst,std::make_pair(go_back,end_game));
		
	return res;
}

Trinome<std::string,BitypeVar<Chesspiece*>,std::pair<bool,bool>>* BaseChess::in_input(Interpret* first_comment, Interpret* second_comment,Interpret* third_comment){
	/* surchage de fonction permettant d'ommetre le bool indiuant si un retour en arrière est permis */
	return this->in_input(first_comment, second_comment,third_comment,false);
}

Trinome<std::string,BitypeVar<Chesspiece*>,std::pair<bool,bool>>* BaseChess::in_depl_input(){
	/* depl en opossition avec roc, en effet cette fonction demnade un input de type IN et définit des messages d'erreurs différentes de sa variante roc */
	
	Interpret* interpret_one = new Interpret(this->get_dico());
	
	interpret_one->add_text("pe_a_depl", true, true);
	interpret_one->add_text("ou", false, true);
	
	// " end "
	interpret_one->add_text(" ", false, false);
	interpret_one->add_text(this->get_end_symbol(), false, false);
	interpret_one->add_text(" ", false, false);

	interpret_one->add_text(this->get_end_symbol(), true, true);
	
	interpret_one->add_text("ou", false, true);
	
	// " lang "
	interpret_one->add_text(" ", false, false);
	interpret_one->add_text(this->get_lang_symbol(), false, false);
	interpret_one->add_text(" ", false, false);
	
	interpret_one->add_text(this->get_lang_symbol(), false, true);
	
	
	Interpret* interpret_two = new Interpret(this->get_dico());
	
	interpret_two->add_text("retry", false, true);
	interpret_two->add_text(", ", false, false);
	interpret_two->add_text("move_pe_oth", false, true);
	
	
	Interpret* interpret_three = new Interpret(this->get_dico());
	
	interpret_three->add_text("retry", false, true);
	interpret_three->add_text(", ", false, false);
	interpret_three->add_text("empty", false, true);
	
	return this->in_input(interpret_one, interpret_two, interpret_three);
}

Trinome<std::string,BitypeVar<Chesspiece*>,std::pair<bool,bool>>* BaseChess::in_roc_input(){
	/* roc en opossition avec depl, en effet cette fonction demnade un input de type IN et définit des messages d'erreurs différentes de sa variante depl */
	
	Interpret* interpret_one = new Interpret(this->get_dico());
	
	interpret_one->add_text("roc_pe", true, true);
	interpret_one->add_text("ou", false, true);
	
	// " ret "
	interpret_one->add_text(" ", false, false);
	interpret_one->add_text(this->get_ret_symbol(), false, false);
	interpret_one->add_text(" ", false, false);
	
	interpret_one->add_text(this->get_ret_symbol(), true, true);
	
	interpret_one->add_text("ou", false, true);
	
	// " end "
	interpret_one->add_text(" ", false, false);
	interpret_one->add_text(this->get_end_symbol(), false, false);
	interpret_one->add_text(" ", false, false);
	
	interpret_one->add_text(this->get_end_symbol(), true, true);
	
	interpret_one->add_text("ou", false, true);
	
	// " lang "
	interpret_one->add_text(" ", false, false);
	interpret_one->add_text(this->get_lang_symbol(), false, false);
	interpret_one->add_text(" ", false, false);

	interpret_one->add_text(this->get_lang_symbol(), false, true);
	
	Interpret* interpret_two = new Interpret(this->get_dico());
	
	interpret_two->add_text("retry", false, true);
	interpret_two->add_text(", ", false, false);
	interpret_two->add_text("roc_oth", false, true);
	
	
	Interpret* interpret_three = new Interpret(this->get_dico());
	
	interpret_three->add_text("retry", false, true);
	interpret_three->add_text(", ", false, false);
	interpret_three->add_text("roc_empty", false, true);
		
	return this->in_input(interpret_one, interpret_two, interpret_three,true);
}

bool BaseChess::check_illegal_move(std::string in,std::string out){
	/* fonction qui vérifie si un mouvement est autorisé, si nécéssaire affiche un message d'echec,
	 * revoi un bool pour indiquer si l'input doit être recommancé */
	
	bool again;
	if (not(this->verify_move(in,out))){
		
		std::stringstream ss;
		ss<<this->get_dico()->search(this->get_active_player()->get_langue(),"retry")<<", "<< this->get_dico()->search(this->get_active_player()->get_langue(),"illegal_move")<<"!"<<std::endl;
		this->get_active_player()->send_msg(ss.str());
		
		again = true;
		}
	else{again = false;}
	return again;
}

std::pair<bool,BitypeVar<Chesspiece*>> BaseChess::normal_output_check(std::string in,std::string out){
	/* normal en oposition avec roc_output_check,
	 * output designe la 2e partie de la selectiond de piece c.a.d. le choix de la destination de la piecesélectionné */
	
	bool res;
	bool valid = this->verify_validity_input(out);
	Chesspiece* cap_piece;
	std::pair<int,int> conv;
	BitypeVar<Chesspiece*> dst;
	
	
	bool has_to_defend_king = this->verify_my_king();
	//std::cout<<"has_to_defend_king? "<<has_to_defend_king<<std::endl;
				
	if (valid){
		bool again = false;
				
		MatPosi* mpos_out = new MatPosi(out);
		dst = this->get_plateau()->get_piece(mpos_out->to_pair());
				
		if (dst.get_state() == true){
					
			cap_piece = dst.get_var();
						
			if (cap_piece->get_owner() == get_active_player()){ // piece de soi-meme
				
				std::stringstream ss;
				ss<<this->get_dico()->search(this->get_active_player()->get_langue(),"retry")<<", "<< this->get_dico()->search(this->get_active_player()->get_langue(),"cap_pe_self")<<std::endl;
				this->get_active_player()->send_msg(ss.str());
				
				again = true;
			}
						
			else{
				// piece de l'adversaire
				// verifier que in peut faire le déplacement vers out !
				if(this->check_illegal_move(in,out) == true){again = true;}
				else{again = this->consequences_legal_out_move(has_to_defend_king,mpos_out);}
			}
		}	
				
		// case vide
		else{
			if(this->check_illegal_move(in,out) == true){again = true;}
			else{again = this->consequences_legal_out_move(has_to_defend_king,mpos_out);}
		
		} //cette notation evite la mise a false de "again"
			
		res = not(again);
	}
	else{
		res = false;
	}
	
	std::pair<bool,BitypeVar<Chesspiece*>> paire = std::make_pair(res,dst);
	return paire;
}

bool BaseChess::verify_possible_roc(Roi* roi,Tour* tour){
	/* fonction qui ércupère les positions du roi et de la tour passé en paramètre et qu les transmet a une fonction qui calcule
	 * si un roc est permis */
	
	MatPosi mpos_roi = MatPosi(*(roi->get_posi()));
	MatPosi mpos_tour = MatPosi(*(tour->get_posi()));
	
	BitypeVar<std::pair<MatPosi*, MatPosi*>>* info = this->sort_mpos_and_calc_roc_info(&mpos_roi,&mpos_tour);
	
	return info->get_state();
	
}

bool BaseChess::check_roc_validity(Roi* roi,Tour* tour, BitypeVar<Chesspiece*> dst, bool in_is_king, bool in_is_tour){
	
	bool correspond = false;
	
	if (in_is_king == true){correspond = verifier_type_pe<Tour>(dst);}
	else{correspond = verifier_type_pe<Roi>(dst);}

	if (correspond == true){
		
		if (in_is_king == true){tour = dynamic_cast<Tour*>(dst.get_var());}
		else{roi = dynamic_cast<Roi*>(dst.get_var());}
		
		correspond = this->verify_possible_roc(roi,tour);

		if (correspond == false){this->get_active_player()->send_msg(this->get_dico()->search(this->get_active_player()->get_langue(),"not_pos_roc"),true);}
		else{
			
			// verfication de deplacement
			if (in_is_king == true){
				
				if(tour->get_has_moved() == true){
					correspond = false;
					this->get_active_player()->send_msg(this->get_dico()->search(this->get_active_player()->get_langue(),"roc_tower_no_perm_move"),true);
				}
			}
			else{
				
				if(roi->get_has_moved() == true){
					correspond = false;
					this->get_active_player()->send_msg(this->get_dico()->search(this->get_active_player()->get_langue(),"roc_king_no_perm_move"),true);
				}
			}
		}
	}
	else{
		
		std::stringstream ss;
		
		if (in_is_tour == true){ss<<this->get_dico()->search(this->get_active_player()->get_langue(),"roc_imp_chess")<<" "<<this->get_dico()->search(this->get_active_player()->get_langue(),"chess_tour")<<std::endl;}
		else{ss<<this->get_dico()->search(this->get_active_player()->get_langue(),"roc_imp_chess")<<" "<<this->get_dico()->search(this->get_active_player()->get_langue(),"chess_roi")<<std::endl;}
		
		this->get_active_player()->send_msg(ss.str());
		
	}
	
	return correspond;
}

Trinome<Trinome<bool,bool,bool>*,BitypeVar<Chesspiece*>,std::string>* BaseChess::roc_first_pe_is_waiting(Chesspiece* pe){
	/* fonction qui est la 2e partie de la verification du roc, c'est une partie qui se déroule alors que la 1e piece du roc a déjà été selectionné et validé */
	
	bool correspond = false;
	bool go_back = false;
	std::string out;
	BitypeVar<Chesspiece*> dst;
	Trinome<std::string,BitypeVar<Chesspiece*>,std::pair<bool,bool>>* roc_trinome;
	std::pair<bool,bool> roc_bool_info;
	
	Roi* roi = nullptr;
	Tour* tour = nullptr;
	
	bool in_is_king;
	bool in_is_tour;
	
	bool ok_roc = is_roquable(pe);
	if (not ok_roc){throw MyException(&mout,"piece roc ni roi ni tour!");}
	
	in_is_king = verifier_type_pe<Roi>(pe);
	in_is_tour = not in_is_king;
	
	if (in_is_king == true){roi = dynamic_cast<Roi*>(pe);}
	else{tour = dynamic_cast<Tour*>(pe);}
	
	bool end_game = false;
	while (not(correspond) and not(go_back) and not(end_game)){
			
		roc_trinome = this->in_roc_input();
		out = roc_trinome->get_first();
		
		roc_bool_info = roc_trinome->get_third();
		go_back = roc_bool_info.first;
		end_game = roc_bool_info.second;
		
		if (not(go_back) and not(end_game)){
		
			dst = roc_trinome->get_second();
			correspond = this->check_roc_validity(roi,tour, dst, in_is_king, in_is_tour);
			//
		}
	}
	
	Trinome<bool,bool,bool>* trinome_bool_res = new Trinome<bool,bool,bool>(go_back,correspond,end_game);
	Trinome<Trinome<bool,bool,bool>*,BitypeVar<Chesspiece*>,std::string>* res = new Trinome<Trinome<bool,bool,bool>*,BitypeVar<Chesspiece*>,std::string>(trinome_bool_res,dst,out);
	
	return res;
	
	
}

Trinome<Trinome<bool,bool,bool>*,BitypeVar<Chesspiece*>,std::string>* BaseChess::roc_output_check(BitypeVar<Chesspiece*> in_pe){
	/* fonction lancé suite a une demande de roc en ayant spécifié une piece sur deux */
	
	Chesspiece* in_piece;
	Trinome<Trinome<bool,bool,bool>*,BitypeVar<Chesspiece*>,std::string>* res = nullptr;

	if (in_pe.get_state() == true){
		in_piece = in_pe.get_var();
								
		if(Roi* roi = dynamic_cast<Roi*>(in_piece)){
			if (roi->get_has_moved() == false){
				res = this->roc_first_pe_is_waiting(roi);
			}
			else{this->get_active_player()->send_msg(this->get_dico()->search(this->get_active_player()->get_langue(),"roc_king_no_perm_move"),true);}
		}
		
		else if(Tour* tour = dynamic_cast<Tour*>(in_piece)) {
			if (tour->get_has_moved() == false){
				res = this->roc_first_pe_is_waiting(tour);
			}
			else{this->get_active_player()->send_msg(this->get_dico()->search(this->get_active_player()->get_langue(),"roc_tower_no_perm_move"),true);}
		}
		
		else{this->get_active_player()->send_msg(this->get_dico()->search(this->get_active_player()->get_langue(),"roc_pe_imp"),true);}
		
	}
	// else case vide (d'office non-selectionnable !)
	
	return res;
}

Trinome<std::string,BitypeVar<Chesspiece*>,Trinome<bool,bool,bool>*>* BaseChess::out_input(std::string in, BitypeVar<Chesspiece*> in_pe){
	
	/* out designe la 2e partie de la selectiond de piece c.a.d. le choix de la destination de la piece sélectionné (en in)
	 * boucle while gèrant toutes les possibilités concernat ce choix de destination*/
	
	BitypeVar<Chesspiece*> dst;
	std::pair<int,int> conv;
	std::pair<bool,BitypeVar<Chesspiece*>> norm_paire;
	
	std::string out = "";
	
	bool part_b = false;
	bool again = false;
	bool correspond = false;
	bool found;
	
	bool roc_accept = this->check_roc_accept(in_pe);
		
	bool end_game = false;
	while (not(part_b) and not(again) and not(correspond) and not(end_game)){
		
		std::stringstream ss_menu;
		ss_menu<<this->get_dico()->search(this->get_active_player()->get_langue(),"depl_pe")<<" "<<in<<std::endl;
		ss_menu<<this->get_dico()->search(this->get_active_player()->get_langue(),"ou")<<" "<<this->get_ret_symbol()<<" "<<this->get_dico()->search(this->get_active_player()->get_langue(),this->get_ret_symbol())<<std::endl;
		if (roc_accept == true){ss_menu<<this->get_dico()->search(this->get_active_player()->get_langue(),"ou")<<" "<<this->get_roc_symbol()<<" "<<this->get_dico()->search(this->get_active_player()->get_langue(),this->get_roc_symbol())<<std::endl;}
		ss_menu<<this->get_dico()->search(this->get_active_player()->get_langue(),"ou")<<" "<<this->get_end_symbol()<<" "<<this->get_dico()->search(this->get_active_player()->get_langue(),this->get_end_symbol())<<std::endl;
		ss_menu<<this->get_dico()->search(this->get_active_player()->get_langue(),"ou")<<" "<<this->get_liste_depl_symbol()<<" "<<this->get_dico()->search(this->get_active_player()->get_langue(),this->get_liste_depl_symbol())<<std::endl;
		ss_menu<<this->get_dico()->search(this->get_active_player()->get_langue(),"ou")<<" "<<this->get_liste_capt_symbol()<<" "<<this->get_dico()->search(this->get_active_player()->get_langue(),this->get_liste_capt_symbol())<<std::endl;
		ss_menu<<this->get_dico()->search(this->get_active_player()->get_langue(),"ou")<<" "<<this->get_lang_symbol()<<" "<<this->get_dico()->search(this->get_active_player()->get_langue(),this->get_lang_symbol())<<std::endl;
		
		this->get_active_player()->send_msg(ss_menu.str());
		
		out = this->get_active_player()->recieve_msg();
		
		
		Trinome<Quadrinome<bool,bool,bool,bool>*, BitypeVar<Chesspiece*>, std::string > symbol_info = this->check_out_for_special_symbol(in_pe, out, roc_accept);
		
		Quadrinome<bool,bool,bool,bool>* symbol_bool_info = symbol_info.get_first();
		
		found = symbol_bool_info->get_first();
		
		if (found){
			
			dst = symbol_info.get_second();
			out = symbol_info.get_third();
			
			again = symbol_bool_info->get_second();
			correspond = symbol_bool_info->get_third();
			end_game = symbol_bool_info->get_fourth();
			
		}
		else{
			norm_paire = this->normal_output_check(in,out);
			part_b = norm_paire.first;
			dst = norm_paire.second;
		}
	}

	Trinome<bool,bool,bool>* trinome_bool_res = new Trinome<bool,bool,bool>(again,correspond,end_game);
	Trinome<std::string,BitypeVar<Chesspiece*>,Trinome<bool,bool,bool>*>* res = new Trinome<std::string,BitypeVar<Chesspiece*>,Trinome<bool,bool,bool>*>(out,dst,trinome_bool_res);
	
	return res;
}

void BaseChess::show_depl_possibles(Chesspiece* pe){
	/* fonction qui s'occupe d'afficher les liste de deplacements et de captures possibles
	 * paramètre d'entree : une piece d'echec
	 * */
	
	this->get_active_player()->send_msg(this->get_dico()->search(this->get_active_player()->get_langue(),this->get_liste_depl_symbol()),false);
	this->get_active_player()->send_msg(" : ",false);
	this->show_possible_mouvement(pe, "depl");
	
	this->get_active_player()->send_msg(this->get_dico()->search(this->get_active_player()->get_langue(),this->get_liste_capt_symbol()),false);
	this->get_active_player()->send_msg(" : ",false);
	this->show_possible_mouvement(pe, "capt");
	
}

void BaseChess::show_depl_possibles(std::string str_co){
	/* fonction qui s'occupe d'afficher les liste de deplacements et de captures possibles
	 * paramètre d'entree : une coordonée de piece d'echec
	 * */
	 
	BitypeVar<Chesspiece*> zone;
	Chesspiece* pe;
	
	MatPosi* mpos = new MatPosi(str_co);
	zone = this->get_plateau()->get_piece(mpos->to_pair());
	pe = zone.get_var();
	
	this->show_depl_possibles(pe);
	
}

Trinome<std::pair<std::string,BitypeVar<Chesspiece*>>,std::pair<std::string,BitypeVar<Chesspiece*>>,std::pair<bool,bool>>* BaseChess::ask_for_input(){
	/* fonction principale des inputs, en effet c'est la fonction qui gêre la demande d'input au joueur (in, out) */
	
	std::stringstream ss;
	ss<<this->get_active_player()<<" "<<this->get_dico()->search(this->get_active_player()->get_langue(),"your_turn")<<"!"<<std::endl;
	this->get_active_player()->send_msg(ss.str());
	
	Trinome<std::string,BitypeVar<Chesspiece*>,Trinome<bool,bool,bool>*>* out_p;
		
	Trinome<std::string,BitypeVar<Chesspiece*>,std::pair<bool,bool>>* in_p;
	
	std::string in,out;
	
	bool again = true;
	BitypeVar<Chesspiece*> adv_pe_out;
	BitypeVar<Chesspiece*> adv_pe_in;
	Trinome<bool,bool,bool>* trinome_out_info;
	
	std::pair<bool,bool> pair_in_info;
	
	
	bool switch_pos = false;
	bool end_game = false;
	while (again and not end_game){
		in_p = this->in_depl_input();
		in = in_p->get_first();
		adv_pe_in = in_p->get_second();
		
		pair_in_info = in_p->get_third();
		//go_back = pair_in_info.first; on peut le recup mais sera tjs false car return pas admis dans "in"
		end_game = pair_in_info.second;
		
		if (end_game == false){
		
			this->show_depl_possibles(in);
			
			out_p = this->out_input(in,adv_pe_in);
					
			out = out_p->get_first();
			adv_pe_out = out_p->get_second();
			
			trinome_out_info = out_p->get_third();
			again = trinome_out_info->get_first();
			switch_pos = trinome_out_info->get_second();
			end_game = trinome_out_info->get_third();
		}
		
	}
		
	std::pair<std::string,BitypeVar<Chesspiece*>> in_paire = std::make_pair(in,adv_pe_in);
	std::pair<std::string,BitypeVar<Chesspiece*>> out_paire = std::make_pair(out,adv_pe_out);
	
	Trinome<std::pair<std::string,BitypeVar<Chesspiece*>>,std::pair<std::string,BitypeVar<Chesspiece*>>,std::pair<bool,bool>>* result = new Trinome<std::pair<std::string,BitypeVar<Chesspiece*>>,std::pair<std::string,BitypeVar<Chesspiece*>>,std::pair<bool,bool>>(in_paire,out_paire,std::make_pair(end_game,switch_pos));
	
	return result;
}

bool BaseChess::verify_validity_input(std::string inp){
	/* fonction qui vérifie si un input est valide, c.a.d si elle est bien composé d'une partie lettre suivi d'une paretie chiffre,
	 * sans autres parasites */
	 
	MatPosi mpos;
	bool valid_coord = mpos.isvalid_coord(inp);
		
	bool res = false;
	if (valid_coord == true){
		if (this->verify_in_board(inp)){res = true;}
		else{
			std::stringstream ss_one;
			ss_one<<this->get_dico()->search(this->get_active_player()->get_langue(),"retry")<<", "<< this->get_dico()->search(this->get_active_player()->get_langue(),"co_ext")<<std::endl;
			this->get_active_player()->send_msg(ss_one.str());
		}
	}
	else{
		std::stringstream ss_two;
		ss_two<<this->get_dico()->search(this->get_active_player()->get_langue(),"retry")<<", "<< this->get_dico()->search(this->get_active_player()->get_langue(),"co_inv")<<std::endl;
		this->get_active_player()->send_msg(ss_two.str());
	}
	return res;
}

void BaseChess::change_active_player(){
	/* fonction qui change le joueur référencé par le titre "actif" qui veut dire "en train de jouer / ayant la main"*/
	if (this->get_active_player() == get_low_player()){
		
		//this->get_active_player()->send_msg("new player (low --> high)");
		set_active_player(get_high_player());
		//mout<<"new player (low --> high): "<<get_high_player()->get_name()<<std::endl;
		//this->get_active_player()->send_msg("new player (low --> high)");
		}
	else if (this->get_active_player() == get_high_player()){
		//this->get_active_player()->send_msg("new player (high --> low)");
		set_active_player(get_low_player());
		//mout<<"new player (high --> low) : "<<get_low_player()->get_name()<<std::endl;
		//this->get_active_player()->send_msg("new player (high --> low)");
		}
	else{throw MyException(&mout, "probleme active player change");}
}

bool BaseChess::verify_in_board(std::string s){
	/* fonction qui vérifie si une coordonée se trouve bien dans les limites du plateau */
	MatPosi* mpos = new MatPosi(s);	
	return this->get_plateau()->verify_in_board(mpos->to_pair());
}

void BaseChess::print_mpos_vect(std::vector<MatPosi*>* vect,bool endline){
	/* fonction qui affiche un vecteur de MatPosi* */
	
	std::stringstream ss;
	
	ss<<"[";
	for(long long unsigned int i=0;i<vect->size();i++){
		
		ss<<((*vect)[i])->to_string();
		
		if (i<(vect->size()-1)){ss<<", ";}
		
	}
	ss<<"]";
	
	this->get_active_player()->send_msg(ss.str(),endline);
		
}

void BaseChess::print_mpos_vect(std::vector<MatPosi*>* vect){
	/* fonction surchargée qui permet d'ommetre de booléen qui indique si on veut un retour a la ligne */
	this->print_mpos_vect(vect,true);
	
}

bool BaseChess::roc_check_king_position_and_path_danger(MatPosi* mpos_roi, MatPosi* mpos_roi_dst, bool gauche_isking,int co_roi_dst,std::vector<MatPosi>* between_vect){
		
	BitypeVar<MatPosi*>* dang_roi_av = this->is_endangered(mpos_roi);
	
	bool valid_roc = not(dang_roi_av->get_state());
	
	//std::stringstream ss_roc_av;
	//ss_roc_av<<"valid roc dang av"<<valid_roc<<std::endl;
	//this->get_active_player()->send_msg(ss_roc_av.str());
	
	if (valid_roc == true){
		
		// si le roi n'est pas en danger apres le roque:
		BitypeVar<MatPosi*>* dang_roi_ap = this->is_endangered(mpos_roi_dst,this->get_non_active_player(),"capt");
		
		valid_roc = not(dang_roi_ap->get_state());
		
		//std::stringstream ss_roc_ap;
		//ss_roc_ap<<"valid roc dang ap"<<valid_roc<<std::endl;
		//this->get_active_player()->send_msg(ss_roc_ap.str());
		
		if (valid_roc == true){
		
			std::vector<MatPosi*>* king_steps_vect = calc_king_move_path((gauche_isking == true),co_roi_dst,between_vect);
			
			// si les cases par lequel passe le roi ne sont pas en danger:
			
			long long unsigned int j=0;
			bool stop = false;
			while (j<king_steps_vect->size() and stop == false){
				BitypeVar<MatPosi*>* step_danger = this->is_endangered((*king_steps_vect)[j],this->get_non_active_player(),"capt");
				if(step_danger->get_state() == true){
					stop = true;
				};
				
				j++;
			}
			
			valid_roc = not(stop);
			//this->get_active_player()->send_msg("etapes rois: ");
			//this->print_mpos_vect(king_steps_vect);
			
			//std::stringstream ss_king_steps;
			//ss_king_steps<<"valid king steps ? "<<valid_roc<<std::endl;
			//this->get_active_player()->send_msg(ss_king_steps.str(),true); // true ok car on souhaite une ligne d'espace
		}
	}
			
	return valid_roc;
	
}

BitypeVar<std::pair<MatPosi*,MatPosi*>>* BaseChess::sort_mpos_and_calc_roc_info(MatPosi* mpos_one,MatPosi* mpos_two){
	/* fonction qui trié des MatPosi reçu en paramètres et qui calcule les destination des 2 MatPosi suite a un roque */
	
	std::pair<MatPosi*,MatPosi*> sorted_mpos_pair = sort_two_mpos(mpos_one, mpos_two);
	MatPosi* mpos_gauche = sorted_mpos_pair.first;
	MatPosi* mpos_droite = sorted_mpos_pair.second;

	BitypeVar<std::vector<MatPosi>*>* bit_vect = this->calculate_zones_between_zones(mpos_gauche, mpos_droite);
	
	MatPosi* mpos_roi_dst;
	MatPosi* mpos_tour_dst;
	
	bool valid_roc = false;
	
	if (bit_vect->get_state() == true){
		
		std::vector<MatPosi>* between_vect = bit_vect->get_var();
		
		// si toutes les cases intermédiares sont vides: 
		long long unsigned int i=0;
		bool stop = false;
		while (i<between_vect->size() and stop == false){
			
			if(not(this->get_plateau()->is_empty_location(((*between_vect)[i]).to_pair()))){
				stop = true;
			};
			i++;
		}
		
		valid_roc = not(stop);
		if (valid_roc == true){
		
			BitypeVar<Chesspiece*> bit_chess_gauche = this->get_plateau()->get_piece(mpos_gauche->to_pair());
			if (bit_chess_gauche.get_state() == false){throw MyException(&mout,"recup chesspiece de case vide");}
			
			bool gauche_isking = verifier_type_pe<Roi>(bit_chess_gauche);
			
			std::pair<int,int> coords = calc_roc_destinations(gauche_isking,between_vect);
			int co_roi_dst = coords.first;
			int co_tour_dst = coords.second;
			
			mpos_roi_dst = &((*between_vect)[co_roi_dst]);
			mpos_tour_dst = &((*between_vect)[co_tour_dst]);
			
			//std::stringstream ss_gauche;
			//ss_gauche<<"gauche = "<<mpos_gauche->to_string()<<", droite = "<<mpos_droite->to_string()<<std::endl;
			//this->get_active_player()->send_msg(ss_gauche.str());
			
			//std::stringstream ss_droite;
			//ss_droite<<"dst roi = "<<mpos_roi_dst->to_string()<<", dst tour = "<<mpos_tour_dst->to_string()<<std::endl;
			//this->get_active_player()->send_msg(ss_droite.str());
			
			// si le roi n'est pas en danger avant le roque:
			
			MatPosi* king_mpos;
			
			if (gauche_isking == true){king_mpos = mpos_gauche;}
			else{king_mpos = mpos_droite;}
			
			valid_roc = this->roc_check_king_position_and_path_danger(king_mpos,mpos_roi_dst,gauche_isking,co_roi_dst,between_vect);
			
		}
	}
	else{throw MyException(&mout,"vecteur des zones entres 2 zones invalide");}
	
	BitypeVar<std::pair<MatPosi*,MatPosi*>>* res = new BitypeVar<std::pair<MatPosi*,MatPosi*>>();
	res->set_state(valid_roc);
	res->set_var(std::make_pair(mpos_roi_dst,mpos_tour_dst));
	
	return res; // retourner si roque ok (bool) et positions si ok
}

bool BaseChess::exec_roc(MatPosi* mpos_one,MatPosi* mpos_two){
	/* fonction d'execution du roque,test de validité puis mouvement si valide
	 * retourne si le roc était validé */
		
	BitypeVar<std::pair<MatPosi*,MatPosi*>>* info = this->sort_mpos_and_calc_roc_info(mpos_one,mpos_two);
	
	bool valid_roc = info->get_state();
	
	if (valid_roc == true){
		
		std::pair<MatPosi*,MatPosi*> dst_pair = info->get_var();
		MatPosi* mpos_roi_dst = dst_pair.first;
		MatPosi* mpos_tour_dst = dst_pair.second;
		
		bool one_isking = verifier_type_pe<Roi>(this->get_plateau()->get_piece(mpos_one->to_pair()));
		
		MatPosi* mpos_roi_ini;
		MatPosi* mpos_tour_ini;
		
		if (one_isking == true){
			mpos_roi_ini = mpos_one;
			mpos_tour_ini = mpos_two;
		}
		
		else{
			mpos_roi_ini = mpos_two;
			mpos_tour_ini = mpos_one;
		}
	
		// deplacement des pieces:
		//rois --> dest tour, et tour --> dest roi, puis switch position (fait ce cette façon pour pouvoir plustard faire une sort de cinematique/ monter visuelmment l'inversion de place)
		this->exec_move(mpos_roi_ini, mpos_tour_dst);
		this->exec_move(mpos_tour_ini, mpos_roi_dst);
		
		this->get_plateau()->switch_positions(mpos_roi_dst->to_pair(),mpos_tour_dst->to_pair());
	}
	
	return valid_roc;
	
}

void BaseChess::exec_move(MatPosi* mpos_in, MatPosi* mpos_out, bool switch_mode){
	/* fonction d'execution du mouvement, un bool switch_mode permet a la fonction de savoir si elle doit faire un depl normal ou un roque
	 * version avec 2 MatPosi en entree */

	std::pair<int,int> pair_in = mpos_in->to_pair();
	std::pair<int,int> pair_out = mpos_out->to_pair();
	
	if (switch_mode == false){this->get_plateau()->move(pair_in,pair_out);}
	else{this->exec_roc(mpos_in,mpos_out);}
}

void BaseChess::exec_move(MatPosi* mpos_in, MatPosi* mpos_out){
	/* fonction surchargée, permettant d'ommetre le bool indiquant le deplacement ou le roque */
	
	this->exec_move(mpos_in, mpos_out, false);
}

void BaseChess::exec_move(std::string str_in, std::string str_out, bool switch_mode){
	/* fonction d'execution du mouvement, un bool switch_mode permet a la fonction de savoir si elle doit faire un depl normal ou un roque
	 * version avec 2 strings de coordonées en entree */
	
	MatPosi* mpos_in = new MatPosi(str_in);
	MatPosi* mpos_out = new MatPosi(str_out);
	
	this->exec_move(mpos_in, mpos_out, switch_mode);
}

void BaseChess::exec_move(std::string str_in, std::string str_out){
	/* fonction surchargée, permettant d'ommetre le bool indiquant le deplacement ou le roque */
	this->exec_move(str_in, str_out, false);
}

void BaseChess::exec_move(std::pair<int,int> pair_in, std::pair<int,int> pair_out, bool switch_mode){
	/* fonction d'execution du mouvement, un bool switch_mode permet a la fonction de savoir si elle doit faire un depl normal ou un roque
	 * version avec 2 paires de coordonees en entree */
	
	MatPosi* mpos_in = new MatPosi(pair_in);
	MatPosi* mpos_out = new MatPosi(pair_out);
	
	this->exec_move(mpos_in, mpos_out, switch_mode);
}

void BaseChess::exec_move(std::pair<int,int> pair_in, std::pair<int,int> pair_out){
	/* fonction surchargée, permettant d'ommetre le bool indiquant le deplacement ou le roque */
	this->exec_move(pair_in, pair_out, false);
}

bool BaseChess::verify_move(MatPosi* in, MatPosi* out, std::string mode){
	/* fonction vérifiant si un mouvement de piece est possible
	 * version avec 2 MatPosi en entree */
	
	BitypeVar<Chesspiece*> bi_t_in = this->get_plateau()->get_piece(in->to_pair());
	BitypeVar<Chesspiece*> bi_t_out = this->get_plateau()->get_piece(out->to_pair());
	
	bool res = false;	
	if (bi_t_in.get_state() == true){
		
		Chesspiece* pe = bi_t_in.get_var();
		
		mode = adaptive_mode(bi_t_out,mode);
		
		std::vector<MatPosi*>* sol = this->check_possible_mouvement(pe, mode);
		
		long long unsigned int i=0;
		while (i<sol->size() and res == false){
			MatPosi* elem = (*sol)[i];
			if (*elem == *out){res = true;}
			
			i++;
		}
	}
	return res;
}

bool BaseChess::verify_move(MatPosi* in, MatPosi* out){
	/* fonction surchargée, permettant d'ommetre le string indiquant le mode de la verification */
	return this->verify_move(in, out, "");
}

bool BaseChess::verify_move(std::string in, std::string out, std::string mode){
	/* fonction vérifiant si un mouvement de piece est possible
	 * version avec 2 strings de coordonees en entree */
	
	MatPosi* mpos_in = new MatPosi(in);
	MatPosi* mpos_out = new MatPosi(out);
	
	return this->verify_move(mpos_in, mpos_out, mode);
}

bool BaseChess::verify_move(std::string in, std::string out){
	/* fonction surchargée, permettant d'ommetre le string indiquant le mode de la verification */
	return this->verify_move(in, out, "");
}

bool BaseChess::verify_move(std::pair<int,int> in, std::pair<int,int> out, std::string mode){
	/* fonction vérifiant si un mouvement de piece est possible
	 * version avec 2 paires de coordonees en entree */
	
	MatPosi* mpos_in = new MatPosi(in);
	MatPosi* mpos_out = new MatPosi(out);
	
	return this->verify_move(mpos_in, mpos_out, mode);
}

bool BaseChess::verify_move(std::pair<int,int> in, std::pair<int,int> out){
	/* fonction surchargée, permettant d'ommetre le string indiquant le mode de la verification */
	return this->verify_move(in, out, "");
}

std::vector<MatPosi*>* BaseChess::check_possible_mouvement(Chesspiece* pe ,std::string mode){
	/* fonction qui récupère tout les mouvements possiles d'une piece */
	
	std::vector<std::pair<std::pair<int,int>,AdvTuple>> vect = pe->algo(mode);
	Posi* origin = pe->get_posi();
	
	std::vector<MatPosi*>* res = new std::vector<MatPosi*>();
	
	MatPosi* elem;
	bool keep;
	
	MatPosi* mposi_origi = new MatPosi(*origin);
	std::pair<int,int> paire_origi = mposi_origi->to_pair();
	
	for(long long unsigned int i=0;i<vect.size();i++){
		
		elem = new MatPosi(vect[i].first);
		AdvTuple adv_tup = vect[i].second;
		std::pair<int,int> paire = elem->to_pair();

		keep = this->check_danger_mouvement_and_path(paire_origi, adv_tup, paire, mode); // verification concrète si ce déplacement est authorisé
		
		if (keep == true){
			res->push_back(elem);
		}
	}
	
	return res;
}

BitypeVar<std::vector<AdvTuple*>*>* BaseChess::find_linking_advtuple(std::pair<int,int> pair_in, std::pair<int,int> pair_out, std::string mode){
	/* fonction qui récupère trouve les Advtuples liant les 2 coordonées via la recheche de la 2e coordonnes dans la liste de deplacement de la premiere
	 * (il y a concervation de l'advtuple originel pour chaque deplacement possible généré) */
	BitypeVar<std::vector<AdvTuple*>*>* found;
	BitypeVar<Chesspiece*> tup_in = this->get_plateau()->get_piece(pair_in);
	
	std::vector<AdvTuple*>* res = new std::vector<AdvTuple*>();
	
	std::string limited_mode;
	
	if (tup_in.get_state() == true){
		Chesspiece* pe_in = tup_in.get_var();
		
		MatPosi* mpos_out = new MatPosi(pair_out);

		limited_mode = this->get_plateau()->get_limited_mode(mode);
		
		std::vector<std::pair<std::pair<int,int>,AdvTuple>> vect = pe_in->algo(limited_mode); // mode
		
		found = new BitypeVar<std::vector<AdvTuple*>*>();
		found->set_state(false);
		
		long long unsigned int i=0;
		while(i<vect.size() and (found->get_state() == false)){
			MatPosi* mpos_elem = new MatPosi(vect[i].first);
			
			if (*mpos_elem == *mpos_out){
				//found->set_var(&(vect[i].second));
				if (found->get_state() == false){found->set_state(true);}
				res->push_back(&(vect[i].second));
			}
			
			i++;
		}
	}
	else{throw MyException(&mout, "demande de find_linking_advtuple() sur case vide en entree !");}
	
	found->set_var(res);
	
	return found;
}

bool BaseChess::complete_danger_test(std::pair<int,int> pair_in, std::pair<int,int> pair_out, std::string mode){
	/* fonction qui recerche si le mouvement d'une piece est sécurisé (en terme de danger d'être capturée) */
	bool keep = false;
	BitypeVar<std::vector<AdvTuple*>*>* bit_var = this->find_linking_advtuple(pair_in, pair_out, mode);
	
	//mout<<"complete_danger_test with:"<<std::endl;
	//mout<<" - "<<pair_in.first<<","<<pair_in.second<<std::endl;
	//mout<<" - "<<pair_out.first<<","<<pair_out.second<<std::endl;
	//mout<<" - "<<mode<<std::endl;
	
	//mout << "link advt state check";
	
	if (bit_var->get_state() == true){
		
		std::vector<AdvTuple*>* advtuples_vect = bit_var->get_var();

		long long unsigned int i=0;
		while (i<advtuples_vect->size() and keep == false){ // si un chemin sécurisé cela suffit
			
			AdvTuple adv_tup = *((*advtuples_vect)[i]);
			
			//mout << "link advt"<<std::endl;//<<adv_tup<<std::endl;
			
			keep = this->check_danger_mouvement_and_path(pair_in, adv_tup, pair_out, mode);
			//mout << "result keep"<<keep<<std::endl;
		
			i++;
		}
	}
	
	//mout<<"exit complete_danger_test with (keep): "<<keep<<std::endl;
	
	return keep;
	
}

void BaseChess::show_possible_mouvement(Chesspiece* pe ,std::string mode){
	/* fonction qui affiche les mouvements possibles d'une piece */
	
	std::vector<MatPosi*>* vect = this->check_possible_mouvement(pe, mode);
	
	std::vector<std::string>* stock = new std::vector<std::string>();
	
	std::stringstream ss;
	
	ss<<"[";
	for(long long unsigned int i=0;i<vect->size();i++){
		std::string temp = (*vect)[i]->to_string();
		if (not(is_in_vect(stock,temp))){
			if (i != 0){ss<<", ";}
			ss<<temp;
			stock->push_back(temp);
		}
	}
	ss<<"]";
	
	this->get_active_player()->send_msg(ss.str(),true);
}

Chesspiece* BaseChess::ask_evolution_input(std::vector<Chesspiece*>* vect){
	/* fonction demandant le choix d'évolution que l'on veut faire pour son pion
	 * recoit en paramètre une liste de type parmis lequel a choisir */
	
	Chesspiece* res;
	bool found = false;
	std::string in;
	while (found == false){
		
		std::stringstream deb_ss;
		
		deb_ss<<this->get_dico()->search(this->get_active_player()->get_langue(),"choix_evolve")<<": ";
		for(long long unsigned int i=0;i<vect->size();i++){
			
			std::stringstream temp_ss;
			temp_ss<<"Nom_"<<*((*vect)[i]);

			deb_ss<<this->get_dico()->search(this->get_active_player()->get_langue(),temp_ss.str());
			
			if (i < vect->size()-2){deb_ss<<", ";}
			else if (i == vect->size()-2){deb_ss<<" "<<this->get_dico()->search(this->get_active_player()->get_langue(),"et")<<" ";}
			else if (i == vect->size()-1){deb_ss<<std::endl;}
		
		}
		
		this->get_active_player()->send_msg(deb_ss.str());
		
		in = this->get_active_player()->recieve_msg();
		
		std::stringstream end_ss;
		
		long long unsigned int j=0;
		while ((found == false) and (j<vect->size())){
		
			std::stringstream ss;
			ss<<"Nom_"<<*((*vect)[j]);
			
			end_ss<<in<<"VS"<<this->get_dico()->search(this->get_active_player()->get_langue(),ss.str())<<std::endl;
			if (in == this->get_dico()->search(this->get_active_player()->get_langue(),ss.str())){
				found = true;
				res = (*vect)[j];
			}
			
			j++;
		}
		if (found == false){end_ss<<this->get_dico()->search(this->get_active_player()->get_langue(),"retry")<<std::endl;}
		
		this->get_active_player()->send_msg(end_ss.str());
		
	}
	return res;
}

int BaseChess::get_evolution_row(Player* play){
	return this->get_player_row(this->get_other_player(play));
}

void BaseChess::check_evolution(){
	/* fonction vérifiant si un pion peut évoluer et qui demande en quele type le pion doit obtenir */
	
	//std::stringstream ss;
	
	//ss<<"debut check_evolution() !"<<std::endl;
	
	int num_row = this->get_evolution_row(this->get_active_player());
	std::vector<BitypeVar<Chesspiece*>> lig = this->get_plateau()->get_row(num_row);
	
	//this->show_bitype_vect(lig);

	BitypeVar<int>* rep = detect_ennemy_pion_in_vect(lig, this->get_active_player());
	
	//ss<<"etat lig "<<rep->get_state()<<std::endl;
	if (rep->get_state() == true){
		
		// evolve
		int num_col = rep->get_var();
		//ss<<"indice lig "<<num_col<<std::endl;
		
		std::pair<int,int> paire = std::make_pair(num_col,num_row);
				
		std::vector<Chesspiece*>* chess_vect = this->evolution_possibilities();
		Chesspiece* nv_pe = ask_evolution_input(chess_vect);
		
		nv_pe->set_owner(this->get_active_player());
		this->get_plateau()->set_piece(paire,nv_pe);
		
	}
	
	//ss<<"fin check_evolution() !"<<std::endl;
	
	//this->get_active_player()->send_msg(ss.str());
	
}

bool BaseChess::can_escape_position(Chesspiece* pe ,std::string mode){
	/* fonction qui verifie si une piece peut s'echapper de sa position actuelle
	 * c.a.d : se déplacer ailleur sans être menacée par une piece (qui pourrait la prendre a son tour) */
	 
	//mout<<"entree can escape position! "<<std::endl; 
	
	bool escape = false;
	
	std::vector<MatPosi*>* vect = this->check_possible_mouvement(pe ,mode);
		
	long long unsigned int i=0;
	while(i<vect->size() and escape == false){
		MatPosi* mpos = (*vect)[i];
		//mout<<"cheking "<<mpos->to_string()<<std::endl;
		escape = not(this->is_endangered(mpos,this->get_other_player(pe->get_owner()),"capt")->get_state());
		//mout<<"reponce "<<escape<<std::endl;
		
		//bool test = not(this->is_endangered(mpos,pe->get_owner())->get_state());
		//plustôt joueur ennemie de celui possdenadant la piece en dangé
		//mout<<"TEST reponce "<<test<<std::endl;
		
		i++;
	}
	
	//mout<<"sortie can escape position! "<<std::endl;
	
	return escape;
}

bool BaseChess::can_player_move(Player* play){
	/* fonction implémenté en vue du pat */ 
	
	int taille = this->get_plateau()->get_taille();
		
	bool escape = false;

	int cnt = this->get_plateau()->begin_position();
	int end_cnt = this->get_plateau()->end_position();
		
	BitypeVar<int>* bit = new BitypeVar<int>(false,0);
	BitypeVar<int>* plat_size = new BitypeVar<int>(true,taille);

	while (cnt <= end_cnt and not escape){
		
		PlatPosi* ppos = new PlatPosi(cnt,*bit,*plat_size);
		std::pair<int,int> paire = ppos->to_pair();
		delete ppos;
		
		if (not(this->get_plateau()->is_empty_location(paire))){
			Chesspiece* pe = this->get_plateau()->get_piece(paire).get_var();
			if (play == pe->get_owner()){
				
				escape = this->can_escape_position(pe ,"depl");
				if (not(escape)){escape = this->can_escape_position(pe ,"capt");}
				
			}
		}
		cnt++;
	}
	
	
	return escape;
	
}

bool BaseChess::more_dangers_part(std::pair<int,int> paire_zone, Player* limitator,int taille,std::string mode){
	/* fonction étant une sous-fonction de "check_more_than_one_danger"
	 * fonction qui vérifie si la piece est menacée par plus qu'une piece adverse
	 * (ou dans le cas d'une case vide si elle est menacée par n'importe qu'elle piece) */
		
	int count = 0;
	
	int cnt = this->get_plateau()->begin_position();
	int end_cnt = this->get_plateau()->end_position();
		
	BitypeVar<int>* bit = new BitypeVar<int>(false,0);
	BitypeVar<int>* plat_size = new BitypeVar<int>(true,taille);
	
	while (cnt <= end_cnt and count < 2){
		
		//mout<<"more_dangers_part cnt: "<<cnt<<std::endl;
		
		PlatPosi* ppos = new PlatPosi(cnt,*bit,*plat_size);
		std::pair<int,int> paire = ppos->to_pair();
		//mout<<"(1) more_dangers_part paire: "<<paire.first<<","<<paire.second<<std::endl;
		delete ppos;
		//mout<<"(2) more_dangers_part paire: "<<paire.first<<","<<paire.second<<std::endl;
		
		if (not(this->get_plateau()->is_empty_location(paire))){				
			if ((limitator->get_name() == "") or ((limitator->get_name() != "") and (limitator == this->get_plateau()->get_piece(paire).get_var()->get_owner()))){
				bool compl_danger = this->complete_danger_test(paire,paire_zone,mode);
				//mout<<"passed complete_danger_test more_dangers_part"<<std::endl;
				if (compl_danger == true){count +=1;}
			}
		}
		cnt++;
	}
	//mout<<"exit more_dangers_part "<<std::endl;
	return (count >= 2);
}

BitypeVar<MatPosi*>* BaseChess::in_endangered_part(std::pair<int,int> paire_zone, Player* limitator,int taille,std::string mode){
	/* fonction étant une sous-fonction de "is_endangered"
	 * fonction qui vérifie si la piece est menacée par une piece adverse, si oui elle la retourne.
	 * (ou dans le cas d'une case vide si elle est menacée par n'importe qu'elle piece, si oui elle la retourne) */
	 
	BitypeVar<MatPosi*>* danger = new BitypeVar<MatPosi*>();

	int cnt = this->get_plateau()->begin_position();
	int end_cnt = this->get_plateau()->end_position();
		
	BitypeVar<int>* bit = new BitypeVar<int>(false,0);
	BitypeVar<int>* plat_size = new BitypeVar<int>(true,taille); // this->get_plateau()->get_taille()
	
	std::string limited_mode;
	
	//mout<<"in_endangered_part before while -----------------"<<std::endl;
	
	while (cnt <= end_cnt and danger->get_state() == false){
		
		//mout<<"in_endangered_part cnt: "<<cnt<<std::endl;
		PlatPosi* ppos = new PlatPosi(cnt,*bit,*plat_size);
		std::pair<int,int> paire = ppos->to_pair();
		//mout<<"(1) in_endangered_part paire: "<<paire.first<<","<<paire.second<<std::endl;
		MatPosi* mpos = ppos;
		//delete ppos; // <---------------- SEGFAULT!
		//mout<<"(2) in_endangered_part paire: "<<paire.first<<","<<paire.second<<std::endl;
		
		if (not(this->get_plateau()->is_empty_location(paire))){

			bool is_same_owner = (limitator == this->get_plateau()->get_piece(paire).get_var()->get_owner());
			//mout<<"in_endangered_part after get paire?"<<std::endl;
			if ((limitator->get_name() == "") or ((limitator->get_name() != "") and is_same_owner)){

				limited_mode = this->get_plateau()->get_limited_mode(mode);
				
				bool compl_danger = this->complete_danger_test(paire,paire_zone,limited_mode);
				danger->set_state(compl_danger);
				danger->set_var(mpos);
			}
		}
		cnt++;
	}
	//mout<<"exit in_endangered_part with (danger->get_state()): "<<danger->get_state()<<std::endl;
	
	return danger;
}

BitypeVar<MatPosi*>* BaseChess::is_endangered(MatPosi* mpos_zone, Player* limitator){
		return this->is_endangered(mpos_zone,limitator,"");
}

BitypeVar<MatPosi*>* BaseChess::is_endangered(MatPosi* mpos_zone, Player* limitator,std::string forced_mode){ // pe owner
	/* fonction qui vérifie si la piece est menacée par une piece adverse, si oui elle la retourne.
	 * (ou dans le cas d'une case vide si elle est menacée par n'importe qu'elle piece, si oui elle la retourne) */	
	 	
	int taille = this->get_plateau()->get_taille();
	std::pair<int,int> paire_zone = mpos_zone->to_pair();
	
	std::string mode;
	
	//mout<< "is empty? "<<paire_zone.first<<","<<paire_zone.second<<std::endl;
	if (forced_mode == ""){
		if (this->get_plateau()->is_empty_location(paire_zone)){mode = "depl";}
		else {mode = "capt";}
	}
	else{mode = forced_mode;}
	
	//mout<<"entering is_endagered_part with "<<paire_zone.first<<","<<paire_zone.second<<" ; "<<limitator->get_name()<< " ; " <<taille << " ; "<<mode<<std::endl;
	return this->in_endangered_part(paire_zone, limitator, taille, mode);
}

BitypeVar<MatPosi*>* BaseChess::is_endangered(MatPosi* mpos_zone){
	/* fonction surchargée, permettant d'ommetre le Human indiquant le type de danger */
	
	Human* player = new Human();
	
	return this->is_endangered(mpos_zone,player);
}


bool BaseChess::check_more_than_one_danger(MatPosi* mpos_zone, Player* limitator){
	/* fonction qui vérifie si la piece est menacée par plus qu'une piece adverse
	 * (ou dans le cas d'une case vide si elle est menacée par n'importe qu'elle piece) */
	 	
	int taille = this->get_plateau()->get_taille();
	std::pair<int,int> paire_zone = mpos_zone->to_pair();
	
	std::string mode;
	if (this->get_plateau()->is_empty_location(paire_zone)){mode = "depl";}
	else {mode = "capt";}
	
	//mout<<"entering more_dangers_part with "<<paire_zone.first<<","<<paire_zone.second<<" ; "<<limitator->get_name()<< " ; " <<taille << " ; "<<mode<<std::endl;
	
	return this->more_dangers_part(paire_zone, limitator, taille, mode);
	
}

bool BaseChess::check_more_than_one_danger(MatPosi* mpos_zone){
	/* fonction surchargée, permettant d'ommetre le Human indiquant le type de danger */
		
	Human* player = new Human();
	
	return this->check_more_than_one_danger(mpos_zone, player);
}

std::vector<MatPosi*>* BaseChess::recup_zones_between_part(std::vector<std::pair<int,int>>* temp_vect, MatPosi* mposi_origi, MatPosi* mposi_end){
	/* fonction étant une sous-fonction de "recup_zones_between"
	 * fonction qui recupere les cases contenu entres 2 positions dans un plateau.
	 * utilisant l'Advtuple utile pour les reliès */
	
	std::vector<MatPosi*>* res = new std::vector<MatPosi*>();
		
	long long unsigned int j=0;
	bool between;
	while (j<temp_vect->size()){
		MatPosi* mposi = new MatPosi((*temp_vect)[j]);
		
		if (*mposi_end > *mposi_origi){
			between = (*mposi > *mposi_origi) and (*mposi < *mposi_end);
		}
		else if (*mposi_end < *mposi_origi){
			between = (*mposi < *mposi_origi) and (*mposi > *mposi_end);
		}
		else{throw MyException(&mout, "deplacement immobile impossible");}
		
		if (between == true){res->push_back(mposi);}

		j++;
	}
	return res;
	
}

std::vector<MatPosi*>* BaseChess::recup_zones_between(std::pair<int,int> begin, AdvTuple adv_tup, std::pair<int,int> end){
	/* fonction qui recupere les cases contenu entres 2 positions dans un plateau. 
	 * utilisant l'Advtuple utile pour les reliès */
	
	std::vector<MatPosi*>* res = new std::vector<MatPosi*>();
	
	MatPosi* mposi_origi = new MatPosi(begin);
	MatPosi* mposi_end = new MatPosi(end);
	
	int limite = this->get_plateau()->get_taille();
	
	if (adv_tup.get_depl_type() == "translation"){
		std::vector<std::pair<int,int>>* temp_vect = adv_tup.get_res(limite,*mposi_origi);
		// get_res donnes toutes les positions sauf position actuelle!
				
		res = recup_zones_between_part(temp_vect, mposi_origi, mposi_end);
		
	}
	
	else if (adv_tup.get_depl_type() != "saut"){throw MyException(&mout, "type deplacement inconnu");}
	
	return res;	
	
}

std::vector<std::vector<MatPosi*>*>* BaseChess::get_zones_between(std::pair<int,int> begin, std::pair<int,int> end, std::string mode){
	/* fonction qui recupere les cases contenu entres 2 positions dans un plateau.
	 * en ayant cherché au préalable d'Advtuple utile pour les reliès*/
	
	std::vector<std::vector<MatPosi*>*>* result = new std::vector<std::vector<MatPosi*>*>();
	
	std::vector<MatPosi*>* res = new std::vector<MatPosi*>();
	
	BitypeVar<std::vector<AdvTuple*>*>* bit_adv = find_linking_advtuple(begin,end,mode); //
	
	if (bit_adv->get_state() == true){ //
		
		std::vector<AdvTuple*>* advtuples_vect = bit_adv->get_var();

		long long unsigned int i=0;
		while (i<advtuples_vect->size()){ // si un chemin sécurisé cela suffit
			
			AdvTuple* adv_tup = (*advtuples_vect)[i];
		
			res = this->recup_zones_between(begin, *adv_tup, end);
			result->push_back(res);
					
			i++;
		}
	}
	else{throw MyException(&mout,"mouvement impossible entre ces 2 positions!!!");}
	
	return result;		
}

bool BaseChess::check_between_is_empty_part(std::vector<std::pair<int,int>>* temp_vect, MatPosi* mposi_origi, MatPosi* mposi_end){
	/* fonction étant une sous-fonction de "check_between_is_empty"
	 * fonction qui vérifie que toutes les cases contenu entres 2 positions dans un plateau soient vides.
	 * utilisant l'Advtuple utile pour les reliès */
	 
	bool res = true;
	
	bool stop = false; // creer pour plus de comprehension avec res
	long long unsigned int j=0;
	while (j<temp_vect->size() and stop == false){
		MatPosi* mposi = new MatPosi((*temp_vect)[j]);
		std::pair<int,int> paire_norm = std::make_pair(mposi->get_col(),mposi->get_lig());
		
		bool between;
		if (*mposi_end > *mposi_origi){
			between = (*mposi > *mposi_origi) and (*mposi < *mposi_end);
		}
		else if (*mposi_end < *mposi_origi){
			between = (*mposi < *mposi_origi) and (*mposi > *mposi_end);
		}
		else{throw MyException(&mout, "deplacement immobile impossible");}
		
		
		if (between == true){stop = not(this->get_plateau()->is_empty_location(paire_norm));}

		j++;
	}
	res = not(stop);
	
	return res;	
}

bool BaseChess::check_between_is_empty(std::pair<int,int> begin, AdvTuple adv_tup, std::pair<int,int> end){
	/* fonction qui vérifie que toutes les cases contenu entres 2 positions dans un plateau soient vides.
	 * utilisant l'Advtuple utile pour les reliès */
	
	bool res = true;
	
	MatPosi* mposi_origi = new MatPosi(begin);
	MatPosi* mposi_end = new MatPosi(end);
	
	int limite = this->get_plateau()->get_taille();
	
	if (adv_tup.get_depl_type() == "translation"){
		std::vector<std::pair<int,int>>* temp_vect = adv_tup.get_res(limite,*mposi_origi);
		// get_res donnes toutes les positions sauf position actuelle!
				
		res = check_between_is_empty_part(temp_vect, mposi_origi, mposi_end);
		
	}
	
	else if (adv_tup.get_depl_type() != "saut"){throw MyException(&mout, "type deplacement inconnu");}
	
	return res;	
	
}

std::pair<bool,MatPosi*> BaseChess::check_if_echec(MatPosi* mpos){

	BitypeVar<MatPosi*>* danger_res =  this->is_endangered(mpos);
	//mout << "apres danger res is_endangered" <<std::endl;
	bool mode_echec = danger_res->get_state();
	MatPosi* mpos_menace = danger_res->get_var();

	return std::make_pair(mode_echec,mpos_menace);
}

bool BaseChess::check_non_active_player_king(Chesspiece* pe){
	/* fonction qui vérifie l'état du roi adverse sur le plateau
	 * (en echec, en echec et mat, coincé, ...) */
	
	std::stringstream ss;
	
	bool mode_echec_et_mat = false;
	
	Posi* position = pe->get_posi();
	MatPosi* mpos = new MatPosi(*position);
	
	//mout<<mpos->to_string()<<":"<<mpos->to_pair().first<<","<<mpos->to_pair().second<<std::endl;
	
	std::pair<bool,MatPosi*> echec_paire = this->check_if_echec(mpos);
	bool mode_echec = echec_paire.first;
	MatPosi* mpos_menace = echec_paire.second;
	
	//ss << mpos->to_string()<<" is in danger?: "<< mode_echec <<std::endl;
	//mout << mpos->to_string()<<" is in danger?: "<< mode_echec <<std::endl;
	
	//mout<<"mpos_menace avant escape: "<<mpos_menace->to_pair().first<<","<<mpos_menace->to_pair().second<<std::endl;
	//--
	bool escape = false;
	if (mode_echec == true){ // deteter aussi le PAT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! si roi est seul piece restante du jouer et qu'il ne peut plus bouger --> pat !!!!!!!!!!!!!!!!!!!!!!!!
				
		escape = this->can_escape_position(pe ,"depl");
		
		// verifier si le roi peu s'écchapper --> tester toutes les methodes de déplacments
		//(sauf roc car si echec --> roc pas permis sinon inutile a verifier car roc ne peut pas empecher echec ou echec et mat (voir regles))
		
		if (not(escape)){escape = this->can_escape_position(pe ,"capt");}
		//ss<<"depl_esacpe? "<<escape<<std::endl;
		//mout << "depl_esacpe? "<<escape<<std::endl;
		if (not(escape)){
			
			//mout<<"before first check_more_than_one_danger of check_non_active_player_king"<<std::endl;
			if(this->check_more_than_one_danger(mpos)){ // <-------------------------------------------------- CRASH !!!!
				mode_echec_et_mat = true;
			}
			else{ // 1 seul danger
				//mout<<" 1 seul danger"<<std::endl;
				//mout<<"mpos_menace: "<<mpos_menace->to_pair().first<<","<<mpos_menace->to_pair().second<<std::endl;
				BitypeVar<MatPosi*>* dangerception = this->is_endangered(mpos_menace);
				bool danger_indanger = dangerception->get_state();
				
				if(danger_indanger == true){ // si menance en danger --> on peut alors eleiminer la menace -->ok
					
					//mode_echec_et_mat = false;
					
					//mout<<"before second check_more_than_one_danger of check_non_active_player_king"<<std::endl;
					
					bool danger_two_dangers = this->check_more_than_one_danger(mpos_menace);
					
					//mout<<"plusieurs danger menance la menace?"<<danger_two_dangers<<std::endl;
					
					
					if (not(danger_two_dangers)){
						MatPosi* dangerception_mpos = dangerception->get_var();
						
						BitypeVar<Chesspiece*> danger_bity = this->get_plateau()->get_piece(dangerception_mpos->to_pair());
						
						if (pe == danger_bity.get_var()){mode_echec_et_mat = true;} // si le seul danger de la menace est le roi
						else{mode_echec_et_mat = false;}
						
						//impossible que ce soit le roi (déjà geré dans can_escape) !!!
						
						// menace peut être prise --> pas echec et mat !!!!						
							
					}
					else{mode_echec_et_mat = true;}
					
					
				}
				else{
					//allie peut-il s'interposer?
					
					//mout<<"INSIDE ELSE"<<std::endl;
					
					std::vector<std::vector<MatPosi*>*>* zones_between_vect_vect = this->get_zones_between(mpos_menace->to_pair(), mpos->to_pair(), "capt");
					
					// plusieurs vecteur = plusieur chemin que la menace peut emprunté --> il faut tous pouvoir les bloquer
					// --> chercher les intersections !!!
					
					
					std::vector<MatPosi*>* intersect_vect = get_path_intesection(zones_between_vect_vect);

					bool blocked = false;
					long long unsigned int i=0;
					while(i<intersect_vect->size() and not(blocked)){
						
						MatPosi* mpos_zone = (*intersect_vect)[i];
						
						//mout<<"[2]endager"<<std::endl;
						BitypeVar<MatPosi*>* dangerzone = this->is_endangered(mpos_zone,this->get_non_active_player());
						blocked = dangerzone->get_state();
						
						//if (blocked){mout<<" '1' pe dont on parle: "<<mpos_zone->to_string()<<std::endl;}
						
						if (blocked){
							bool two_blockers = this->check_more_than_one_danger(mpos_zone,this->get_non_active_player());
							if (not two_blockers){
								
								//verifier que bloqueur est difféernt de roi // --------------------------------------------------------------------- !!! siùilaire a au dessus avec dangerception!
								
								//mout<<"pas 2 blockers!"<<std::endl;
								
								MatPosi* mpos_blocker = dangerzone->get_var();
								
								//mout<<"blocker: "<<mpos_blocker->to_string()<<std::endl;
								
								BitypeVar<Chesspiece*> blocker_bit = this->get_plateau()->get_piece(mpos_blocker->to_pair());
								
								if (pe == blocker_bit.get_var()){blocked = false;} // si le seul danger de la menace est le roi
							}
						}
						
						
						//if (blocked){mout<<"pe dont on parle: "<<mpos_zone->to_string()<<std::endl;}
					
						i++;
					}
					
					//ss<<"une pe peut-elle bloquer la menace? "<<blocked<<std::endl;
					//mout<<"une pe peut-elle bloquer la menace? "<<blocked<<std::endl;

					if (not(blocked)){mode_echec_et_mat = true;}
					
				}
			}
		}
		//else{mode_echec_et_mat = false;}
		
		if (not(mode_echec_et_mat)){ss<<this->get_dico()->search(this->get_active_player()->get_langue(),"mode_echec")<<std::endl;}
		//else{ss<<this->get_dico()->search(this->get_active_player()->get_langue(),"mode_echec_et_mat")<<std::endl;} // geré dorénavent ailleur
	}
	
	this->get_active_player()->send_msg(ss.str());
	
	return mode_echec_et_mat;
	
}

bool BaseChess::verify_kings(){
	/* fonction vérifiant la présence des roi des deux joueurs sur le plateau */
	
	std::vector<Chesspiece*>* vect = this->get_kings();
	Chesspiece* pe;
	//this->get_active_player()->send_msg("DEB VERIFY KINGS:",true);
	
	bool mode_echec_et_mat = false;
	bool act_found = false;
	bool non_act_found = false;
	
	//MatPosi* mpos_echec;
	
	Chesspiece* active_player_king = nullptr;
	Chesspiece* non_active_player_king = nullptr;
	
	long long unsigned int i=0;
	while(i<vect->size() and (not(act_found) or not(non_act_found))){ // 1 seul roi a trouver
		pe = (*vect)[i];
		
		if (pe->get_owner() == this->get_non_active_player()){
			non_active_player_king = pe;
			non_act_found = true;
		}
			
		else if (pe->get_owner() == this->get_active_player()){
			active_player_king = pe;
			act_found = true;
		}
		
		i++;
	}

	if (not(act_found) or not(non_act_found)){throw MyException(&mout,"deux Roi non trouves!");}
	
	//mout << "en echec?"<<mode_echec<<std::endl;
	//mout << "en echec et mat?"<<mode_echec_et_mat<<std::endl;
	
	MatPosi* vict_mpos = new MatPosi(*(active_player_king->get_posi()));
	BitypeVar<MatPosi*>* vict = this->is_endangered(vict_mpos);
	
	bool victory = vict->get_state();
	
	if (not(victory)){
		mode_echec_et_mat = this->check_non_active_player_king(non_active_player_king);
	}
	else{
		std::stringstream ss;
		ss<<this->get_dico()->search(this->get_active_player()->get_langue(),"vict")<<" "<<get_non_active_player()<<" ! (previson avant le deplacement reel)"<<std::endl;
		this->get_active_player()->send_msg(ss.str());
	}
	
	//this->get_active_player()->send_msg("FIN VERIFY KINGS:",true);
	
	return mode_echec_et_mat;
	
}

std::vector<Chesspiece*>* BaseChess::get_kings(){
	/* fonction permettant de recuperer les roi des deux joueurs sur le plateau */
	
	std::vector<Chesspiece*>* res = new std::vector<Chesspiece*>();
	
	int cnt = this->get_plateau()->begin_position();
	int end_cnt = this->get_plateau()->end_position();
	
	for(;cnt <= end_cnt;cnt++){ // 1e case for vide --> ok, car pas besoin d'initialiser cnt car déjà fait avant (noter simplement cnt sert a rien et génère un warning)
		
		BitypeVar<Chesspiece*> dst = this->get_plateau()->get_piece(cnt);
		if (verifier_type_pe<Roi>(dst) == true){
			res->push_back(dst.get_var());
		}
	}
	return res;
}

bool BaseChess::check_danger_mouvement_and_path(std::pair<int,int> paire_origi, AdvTuple adv_tup, std::pair<int,int> paire, std::string mode){
	/* fonction verifiant qu'une piece peut effectuer le mouvement car ses mouvement possible le permettent
	 * ainsi que les cases qu'elle doit parcourir lors du mouvement soint vides */
	 
	bool keep = false;
	bool valid;
		
	valid = this->get_plateau()->isvalid_move(paire_origi, paire, mode); // <-------------------------------------------------------------------------- rajouter un ignore owner
	
	//mout << "valid"<<valid<<"(normalement tjs non avec limitator avant)"<<std::endl;
	
	if(valid == true){		
		keep = this->check_between_is_empty(paire_origi, adv_tup, paire);
		//mout << "bewteen"<<keep<<std::endl;
	}
	//else{keep = false;}
	
	return keep;
	
}

Trinome<std::string,std::string,bool>* BaseChess::decode_merged_string(std::string merged_string){
	
	Trinome<std::string,std::string,bool>* res_trinome = new Trinome<std::string,std::string,bool>();
	
	std::vector<std::string>* res_vect = split_string(merged_string,";");
	
	std::string in,out;
	bool switch_pos = false;
	
	if (res_vect->size() == 2){
		
		in = (*res_vect)[0];
		out = (*res_vect)[1];
	}
	
	else if (res_vect->size() == 3){
		
		in = (*res_vect)[0];
		out = (*res_vect)[2];
		
		if ((*res_vect)[1] != this->get_roc_symbol()){throw MyException(&mout,"symbole invalide !");}
		else {switch_pos = true;}

	}
	else {throw MyException(&mout,"merged_string invalide !");}
	
	res_trinome->set_first(in);
	res_trinome->set_second(out);
	res_trinome->set_third(switch_pos);
	
	return res_trinome;
	
}

//---------------------

std::pair<bool,bool> BaseChess::execute_forced_step(std::string merged_coords,bool invert_y,Player* play){
		
	Trinome<std::string,std::string,bool>* res_trinome = this->decode_merged_string(merged_coords);
	
	std::string in = res_trinome->get_first();
	std::string out = res_trinome->get_second();
	
	BitypeVar<int>* bit_taille = new BitypeVar<int>(true,this->get_plateau()->get_taille());
	
	bool valid_coords = true;
	
	try{
		
		PlatPosi* ppos_in = new PlatPosi(in,*bit_taille);
		if(invert_y == true){
			ppos_in->invert_y_axis();
			res_trinome->set_first(ppos_in->to_string());
		}
		delete ppos_in;
		
		PlatPosi* ppos_out = new PlatPosi(out,*bit_taille);
		if(invert_y == true){
			ppos_out->invert_y_axis();
			res_trinome->set_second(ppos_out->to_string());
		}
		delete ppos_out;
		
	}	
	catch(MyException& e){
		valid_coords = false;
	}

	BitypeVar<Trinome<std::string,std::string,bool>*>* bit_res = new BitypeVar<Trinome<std::string,std::string,bool>*>(valid_coords,res_trinome);
	
	return this->execute_forced_step(bit_res,play);
}

std::pair<bool,bool> BaseChess::execute_forced_step(std::string merged_coords,bool invert_y,std::string player_name,Player* play){
	
	(void)player_name;
	//if (this->get_active_player()->get_name() != player_name){throw MyException(&mout,"execution impossible, ce n'est pas le tour de ce joueur");}
	
	return this->execute_forced_step(merged_coords,invert_y,play);
	
}

std::pair<bool,bool> BaseChess::execute_forced_step(std::string merged_coords,std::string player_name,Player* play){
	
	return this->execute_forced_step(merged_coords,false,player_name,play);
	
}

std::pair<bool,bool> BaseChess::execute_forced_step(std::string merged_coords,Player* play){	
	return this->execute_forced_step(merged_coords,false,play);
}

//--------------------


std::pair<bool,bool> BaseChess::execute_forced_step(std::string merged_coords,bool invert_y){
	
	return this->execute_forced_step(merged_coords,invert_y,this->get_active_player());
	
}

std::pair<bool,bool> BaseChess::execute_forced_step(std::string merged_coords,bool invert_y,std::string player_name){
	
	return this->execute_forced_step(merged_coords,invert_y,player_name,this->get_active_player());
		
}

std::pair<bool,bool> BaseChess::execute_forced_step(std::string merged_coords,std::string player_name){
	
	return this->execute_forced_step(merged_coords,player_name,this->get_active_player());
	
}

std::pair<bool,bool> BaseChess::execute_forced_step(std::string merged_coords){
	return this->execute_forced_step(merged_coords,this->get_active_player());
}

//***********************************************************

std::pair<bool,bool> BaseChess::execute_forced_step_play(std::string merged_coords,bool invert_y,std::string str_play){
	
	return this->execute_forced_step(merged_coords,invert_y,this->get_player(str_play));
	
}

std::pair<bool,bool> BaseChess::execute_forced_step_play(std::string merged_coords,bool invert_y,std::string player_name,std::string str_play){
	
	return this->execute_forced_step(merged_coords,invert_y,player_name,this->get_player(str_play));
		
}

std::pair<bool,bool> BaseChess::execute_forced_step_play(std::string merged_coords,std::string player_name,std::string str_play){
	
	return this->execute_forced_step(merged_coords,player_name,this->get_player(str_play));
	
}

std::pair<bool,bool> BaseChess::execute_forced_step_play(std::string merged_coords,std::string str_play){
	return this->execute_forced_step(merged_coords,this->get_player(str_play));
}
//****************************************



bool BaseChess::check_pat(){
	
	bool can_move = this->can_player_move(this->get_active_player());
	
	bool echec_et_mat = not can_move;
	
	//mout << "		EN ECHEC ET MAT ?" << echec_et_mat <<std::endl;
	return echec_et_mat;
}

std::string BaseChess::get_affichage_pat() const{
	std::stringstream ss;
	ss<<this->get_dico()->search(this->get_active_player()->get_langue(),"draw")<<" !"<<std::endl;
	return ss.str();
}

std::string BaseChess::get_affichage_resultat(bool end, bool abandon, bool pat) const{
		
	std::stringstream ss;
	if (pat == true){this->get_affichage_pat();}
	else{
		
		Player* play;
		
		if (abandon == true){play = this->get_non_active_player();}
		else if (end == true){play = this->get_active_player();}
		else{throw MyException(&mout,"erreur dans affichage resultat, situation inconnue");}
		
		ss<<this->get_dico()->search(this->get_active_player()->get_langue(),"vict")<<" "<<play<<" !"<<std::endl;}
	
	ss<<this->get_dico()->search(this->get_active_player()->get_langue(),"fin_match")<<"!"<<std::endl;
	
	return ss.str();
	
}

void BaseChess::affichage_resultat(bool end, bool abandon, bool pat) const {
	
	std::string aff_res = this->get_affichage_resultat(end, abandon, pat);
	this->get_active_player()->send_msg(aff_res);
	
}

bool BaseChess::verify_all_eaten(){return false;}



bool BaseChess::exec_step(std::string in, std::string out, BitypeVar<Chesspiece*> adv_pe_out, bool switch_pos,bool abandon){
	
	bool end,pat;
	
	if (not abandon){
		end = check_end_game(adv_pe_out, switch_pos);

		// ici on suppose que input correcte puisque apres les verification
		this->exec_move(in,out,switch_pos);
	}

	if(not end and not abandon){end = this->verify_all_eaten();}

	if(not end and not abandon){pat = this->check_pat();}
	
	if (not end and not abandon and not pat){this->check_evolution();}
	
	if (not end and not abandon and not pat){end = this->verify_kings();} 
	
	if (end == true){this->get_active_player()->send_msg(this->get_dico()->search(this->get_active_player()->get_langue(),"mode_echec_et_mat"),true);} // si arret par consequences automatiquement echec et mat (pas possible de pat)
	
	this->affichage();
	
	//if (not end and not abandon and not pat){this->change_active_player();}
	//else{this->affichage_resultat(end, abandon, pat);}
	
	if (end or abandon or pat) {this->affichage_resultat(end, abandon, pat);}
	
	this->inc_action_cnt();

	return (end or abandon or pat);

}

bool BaseChess::check_roc_accept(BitypeVar<Chesspiece*> in_pe) const {return (verifier_type_pe<Roi>(in_pe) or verifier_type_pe<Tour>(in_pe));}

std::pair<bool,std::string> BaseChess::execute_step(std::string str_play){return this->execute_step(this->get_player(str_play));}
std::pair<bool,std::string> BaseChess::execute_step(){return this->execute_step(this->get_active_player());}

std::pair<bool,bool> BaseChess::execute_forced_step(BitypeVar<Trinome<std::string,std::string,bool>*>* bit,std::string str_play){
	return this->execute_forced_step(bit,this->get_player(str_play));
}
std::pair<bool,bool> BaseChess::execute_forced_step(BitypeVar<Trinome<std::string,std::string,bool>*>* bit){
	return this->execute_forced_step(bit,this->get_active_player());
}

AbstractAffichage* BaseChess::get_affich() const {return this->affich;}
void BaseChess::set_affich(AbstractAffichage* aff){this->affich = aff;}

bool BaseChess::verify_my_king(){
	/* fonction vérifiant si mon roi est en danger */
	
	std::vector<Chesspiece*>* vect = this->get_kings();
	
	Chesspiece* pe;
	Chesspiece* my_king;
	bool found = false;
	
	long long unsigned int i=0;
	while(i<vect->size() and not found){ // 1 seul roi a trouver
		pe = (*vect)[i];
		
		if (pe->get_owner() == this->get_active_player()){
			my_king = pe;
			found = true;
		}
		
		i++;
	}

	if (not found){throw MyException(&mout,"mon Roi introuvable!");}
		
	MatPosi* vict_mpos = new MatPosi(*(my_king->get_posi()));
	BitypeVar<MatPosi*>* vict = this->is_endangered(vict_mpos);
	
	return vict->get_state();
	
}

bool BaseChess::consequences_legal_out_move(bool has_to_defend_king,MatPosi* mpos_roi){
	
	std::string error_msg;
	
	BitypeVar<MatPosi*>* bit_rep = this->is_endangered(mpos_roi,this->get_non_active_player(),"capt"); //bool again = this->verify_my_king();
	bool again = bit_rep->get_state();
	
	//std::cout<<"king still in danger? "<<again<<std::endl;
	
	if (has_to_defend_king){
		if (again){error_msg = this->get_dico()->search(this->get_active_player()->get_langue(),"laisse_roi_danger");}
	} // si on doit proteger son roi et qu'a la fin il est tjs en danger on ne peut pas faire le movement
						
	else{
		if (again){error_msg = this->get_dico()->search(this->get_active_player()->get_langue(),"mise_roi_danger");}
	} // interdit de mettre son roi en danger!
	
	if (again){
		std::stringstream ss;
		ss<<this->get_dico()->search(this->get_active_player()->get_langue(),"retry")<<", "<< error_msg <<std::endl;
		this->get_active_player()->send_msg(ss.str());
	}
	
	return again;
	
}

std::vector<std::pair<int,int>>* BaseChess::return_possible_mouvement(Chesspiece* pe ,std::string mode){
	/* fonction qui affiche les mouvements possibles d'une piece */
	
	std::vector<MatPosi*>* vect = this->check_possible_mouvement(pe, mode);
	
	std::vector<std::pair<int,int>>* stock = new std::vector<std::pair<int,int>>();
	
	BitypeVar<int>* bit_taille = new BitypeVar<int>(true,this->get_plateau()->get_taille());
	
	for(long long unsigned int i=0;i<vect->size();i++){
		std::string temp = (*vect)[i]->to_string();

		PlatPosi* temp_plat = new PlatPosi(temp,*bit_taille);
		
		std::pair<int,int> temp_int = temp_plat->to_pair();
		
		if (not(is_in_vect(stock,temp_int))){stock->push_back(temp_int);}
		
		delete temp_plat;
		
	}
	
	return stock;
}

BitypeVar<Chesspiece*> BaseChess::return_bit_pe_from_str(std::string coords){
	
	BitypeVar<int>* bit_taille = new BitypeVar<int>(true,this->get_plateau()->get_taille());
	//BitypeVar<int>* bit_empty = new BitypeVar<int>(false,0);
	PlatPosi* pe_ppos = new PlatPosi(coords,*bit_taille);
	std::pair<int,int> paire = pe_ppos->to_pair();
	delete pe_ppos;

	BitypeVar<Chesspiece*> bit_pe = this->get_plateau()->get_piece(paire);
	
	//Chesspiece* pe;
	
	//if (bit_pe.get_state() == false){throw MyException(&mout,"demande piece case vide!");}
	//else{pe = bit_pe.get_var();}
	
	return bit_pe;
}

std::vector<std::pair<int,int>> BaseChess::return_pe_mov(std::string coords,std::string wanted_owner){
	
	BitypeVar<Chesspiece*> bit_pe = this->return_bit_pe_from_str(coords);
	
	std::vector<std::pair<int,int>>* mov_vect = new std::vector<std::pair<int,int>>();
	
	if (bit_pe.get_state() == true){
	
		Chesspiece* pe = bit_pe.get_var();
		
		if ((wanted_owner == "") or ((wanted_owner != "") and (pe->get_owner()->get_name() == wanted_owner))){
		
			mov_vect = this->return_possible_mouvement(pe,"depl");
			std::vector<std::pair<int,int>>* capt_vect = this->return_possible_mouvement(pe,"capt");
			
			mov_vect->insert( mov_vect->end(), capt_vect->begin(), capt_vect->end() );
		}
		//else{/* default */}
	}
	//else{/* default */}
	
	return *mov_vect;
}

std::vector<std::pair<int,int>> BaseChess::return_pe_mov(std::string coords){
	
	return this->return_pe_mov(coords,"");
	
}

std::vector<std::string> *BaseChess::possible_mov(std::string position){
	MatPosi pos(position);
	BitypeVar<Chesspiece*> piece = get_plateau()->get_piece(pos.to_pair());
	std::vector<MatPosi*> *depl = check_possible_mouvement(piece.get_var(),"depl");
	std::cout << depl->size() << std::endl;
	std::vector<MatPosi*> *capt = check_possible_mouvement(piece.get_var(),"capt");
	std::vector<std::string> *res = new std::vector<std::string>();
	for (unsigned int i=0; i < depl->size();i++){
		std::cout<<"i : "<< i <<" depl :  "<< depl->at(i)->to_string() <<std::endl;
		res->push_back(depl->at(i)->to_string());
	}

	for (unsigned int i=0; i < capt->size();i++){
		res->push_back(capt->at(i)->to_string());
	}
	return res;
}

bool BaseChess::myTurn(std::string username){
	return (this->active_player->get_name() == username);
}

#endif
