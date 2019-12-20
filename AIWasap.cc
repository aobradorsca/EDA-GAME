#include "Player.hh"


/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME Wasap


// DISCLAIMER: The following Demo player is *not* meant to do anything
// sensible. It is provided just to illustrate how to use the API.
// Please use AINull.cc as a template for your player.


struct PLAYER_NAME : public Player {

  /**
   * Factory: returns a new instance of this class.
   * Do not modify this function.
   */
  static Player* factory () {
    return new PLAYER_NAME;
  }

  /**
   * Types and attributes for your player can be defined here.
   */

typedef vector<vector<bool>> selmat;
typedef vector<vector<int>> aux_info1;
typedef vector<vector<Pos>> aux_info2;
typedef pair<int, Pos> camino;
typedef pair<camino,int> primers;
typedef vector<int> VI;
typedef vector<Pos> VP;
vector<int> trolls;

Pos pcelda(const int& id, const int& d, const int& k){
    Pos p = unit(id).pos;
    for (int i = 0;i < (d-(k%d));++i) p += Dir(k/d);
    for (int i = 0;i < k%d;++i) p += Dir((k/d +1)%8);
    return p;
}

void posicions_trolls(map<Pos,bool>& postroll){
	for(int i= 0;i<4;++i){
		Pos p = unit(trolls[i]).pos;
		for (int k=0;k<8;++k){
			Pos adj = p + Dir(k);
			if(pos_ok(adj)) postroll[adj] = true;
		}
	}
	for(int d = 1; d<4;++d){
		for(int i=0;i<8*d;++i){
			Pos p = pcelda(balrog_id(),d,i);
			if(pos_ok(p) and d == 1 and cell(p).type != Outside){
				 postroll[p] = true;
			}
			else if(pos_ok(p) and d == 2  and i%4 != 2 and cell(p).type != Outside){
				 postroll[p] = true;
			}
		}
	}
}

void go_w(const int& id,const Pos& aux){
	Pos p = unit(id).pos;
	bool trobat = false;
	for(int i=0;i<4 and not trobat;++i){
		Pos pd = p + Dir(i*2);
		if(pos_ok(pd) and pd == aux){
			command(id,Dir(i*2));
			trobat = true;
		}
	}
}

void go_d(const int& id,const Pos& aux){
	Pos p = unit(id).pos;
	bool trobat = false;
	for(int i=0;i<8 and not trobat;++i){
		Pos pd = p + Dir(i);
		if(pos_ok(pd) and pd == aux){
			command(id,Dir(i));
			trobat = true;
		}
	}
}

bool comproba_enemic_adj(const Pos& base){
	for(int i = 0;i<8;++i){
		Pos p= base + Dir(i);
		if(pos_ok(p) and cell(p).id != -1 and unit(cell(p).id).player != me() and unit(cell(p).id).type != Wizard) return true;
	}
	return false;
}

bool comproba_d_adjecent(const int& id){
	for(int i =0;i<4;++i){
		Pos adj = unit(id).pos + Dir(i*2);
		if(pos_ok(adj) and cell(adj).id != -1 and unit(cell(adj).id).type == Dwarf and unit(cell(adj).id).player == unit(id).player) return true;
	}
	return false;
}

bool comproba_amics_d(const int& id){
	for(int i =0;i<4;++i){
		Pos adj = unit(id).pos + Dir(i*2);
		if(pos_ok(adj) and cell(adj).id != -1 and unit(cell(adj).id).type == Dwarf and unit(cell(adj).id).player == me()) return true;
	}
	return false;
}

bool comproba_amics_w(const int& id){
	for(int i =0;i<4;++i){
		Pos adj = unit(id).pos + Dir(i*2);
		if(pos_ok(adj) and cell(adj).id != -1 and unit(cell(adj).id).type == Wizard and unit(cell(adj).id).player == me()) return true;
	}
	return false;
}
	

bool comproba_w_adjecent(const Pos& base,const int& jug){
	for(int i =0;i<4;++i){
		Pos adj = base + Dir(i*2);
		if(pos_ok(adj) and cell(adj).id != -1 and unit(cell(adj).id).type == Wizard and unit(cell(adj).id).player == jug and unit(cell(adj).id).player != me()) return true;
	}
	return false;
}

bool comproba_adjecent(const int& id,const Pos& adj){
	Pos p = unit(id).pos;
	for(int i=0;i<8;++i){
		Pos aux = p+Dir(i);
		if (aux == adj){
			return (unit(cell(adj).id).type == Orc or unit(cell(adj).id).type == Wizard) or (not comproba_w_adjecent(aux,unit(cell(adj).id).player) or not (unit(cell(adj).id).health > 21));
		}
	}
	return false;
}

void comproba_w(const int& id,int d,int& amicd,VP& enemyw,bool& benemyw){
	if(d > 0){
		comproba_w(id,d-1,amicd,enemyw,benemyw);
		for(int i = 0; i < 8*d; ++i){
			Pos p2 = pcelda(id,d,i);
			if(pos_ok(p2)){
				int id2 = cell(p2).id;
				CellType ct = cell(p2).type;
				if(id2 != -1){
					if (ct == Cave or ct == Outside){
						UnitType ut = unit(id2).type;
						if (unit(id2).player == me()){
							if(ut == Dwarf and i%2 == 0) ++amicd;
						}
						else if(d==1 and ut != Wizard and not benemyw){
								 enemyw.push_back(p2);
								 benemyw = true;
						}
					}
				}
			}	
		}
	}
}

void comproba_d(const int& id,int d,int& amicd,VP& obj,bool& bobj,VP& objo,bool& bobjo,int& amicw,map<Pos,bool>& tpillat,map<Pos,int>& dtoenemy){
	if(bobj or bobjo) return;
	if(d > 0){
		comproba_d(id,d-1,amicd,obj,bobj,objo,bobjo,amicw,tpillat,dtoenemy);
		for(int i = 0; i < 8*d; ++i){
			Pos p2 = pcelda(id,d,i);
			if(pos_ok(p2)){
				int id2 = cell(p2).id;
				CellType ct = cell(p2).type;
				if(id2 != -1){
					if (ct == Cave or ct == Outside){
						UnitType ut = unit(id2).type;
						if(ut != Troll and unit(id2).player != me()){
							if((ut == Wizard or (ut == Dwarf and (not comproba_w_adjecent(p2,unit(id2).player) or unit(id2).health < 21))) and not bobj){
								if(obj.size() == 0) obj.push_back(p2);
								else if(unit(cell(obj[0]).id).health > unit(id2).health) obj[0] = p2;
							 }
							else if(ut == Orc and not bobjo and d<4 and (not dtoenemy.count(p2) or dtoenemy[p2] < 3)){
								objo.push_back(p2);
							}
						}
					}
				}
			}	
		}
		if(obj.size() > 0 and not bobj) {
			if(not dtoenemy.count(obj[0])) dtoenemy[obj[0]] = 1;
			else ++dtoenemy[obj[0]];
			bobj = true; 
		}
		if(objo.size() > 0 and not bobjo){
			if(not dtoenemy.count(objo[0])) dtoenemy[objo[0]] = 1;
			else ++dtoenemy[objo[0]];
			bobjo = true;
		 }
	}
}

bool dijkstra_d(const int& id,Pos& aux,const string s,bool pob, Pos& lloc,map<Pos,bool>& postroll,map<Pos,bool>& poscave,map<Pos,bool>& anar,primers& torn){ 
	int max = 20;
	if(nb_cells(me()) > 800) max =  7;
	CellType ct;
	if(s=="Cave") ct = Cave;
	else if(s=="Rock")ct = Rock;
	else ct = Outside;
	int area = 8;
	int areaw = 1;
	/*bool amic = comproba_amics_w(id);
	if(amic and s!= "Balrog"){
		area = 4;
		areaw = 2;
	}*/
	Pos pm = unit(id).pos;
	if(pob){
		pm = lloc;
		lloc = unit(id).pos;
	}
    selmat selected(60, vector<bool>(60, false));
    aux_info1 d(60, vector<int>(60, 9999));                  //60x60
    aux_info2 previo(60, vector<Pos>(60, Pos(-1,-1)));
    priority_queue<camino, vector<camino>, greater<camino>> Q;
    Q.push(make_pair(0,pm));
    d[pm.i][pm.j] = 0;
    bool found = false;
    while(not Q.empty() and not found){                          
        Pos u = Q.top().second; Q.pop();                         
        if(not selected[u.i][u.j]){
            selected[u.i][u.j] = true;
            for(int i = 0; i < area and not found; ++i) {
                Pos p = u+Dir(i*areaw);
                if(pos_ok(p) and not selected[p.i][p.j]){
					CellType t = cell(p).type;
					int id2 = cell(p).id;
					bool unitat = (id2 != -1);  
					if(t != Abyss and t != Granite and not postroll.count(p) and not anar.count(p)){
						if(pob){
							if((not unitat or (unitat and p == lloc))){
								int c = 0;
								if(t == Rock) c = cell(p).turns;
									if(d[p.i][p.j] > d[u.i][u.j] + (c+1)){  
										if(p == lloc){
											found = true;
											aux = u;
											torn.first.first = d[u.i][u.j] + 1;
											torn.second = id;
										}       
									d[p.i][p.j] = d[u.i][u.j] + c+1;
									previo[p.i][p.j].i = u.i;
									previo[p.i][p.j].j = u.j;
									Q.push(make_pair(d[p.i][p.j], p)); 
								}
							}
						}
						else if(s == "Cave"){
							if(not unitat){
								int c = 0;
								if(t == Rock) c = cell(p).turns;
									if(d[p.i][p.j] > d[u.i][u.j] + (c+1) and (d[u.i][u.j]+(c+1)) < max and not poscave.count(p)){  
										if(t == ct  and cell(p).owner != me()){
											poscave[p] = true;
											found = true;
											aux = p;
											torn.first.first = d[u.i][u.j] + 1;
											torn.second = id;
										}       
									d[p.i][p.j] = d[u.i][u.j] + c+1;
									previo[p.i][p.j].i = u.i;
									previo[p.i][p.j].j = u.j;
									Q.push(make_pair(d[p.i][p.j], p)); 
								}
							}
						}
						else if(s=="Rock"){
							if(not unitat){
								int c = 0;
								if(t == Rock) c = cell(p).turns;
									if(d[p.i][p.j] > d[u.i][u.j] + (c+1)){  
										if(t == ct){
											found = true;
											aux = p;
											torn.first.first = d[u.i][u.j] + 1;
											torn.second = id;
										}       
									d[p.i][p.j] = d[u.i][u.j] + c+1;
									previo[p.i][p.j].i = u.i;
									previo[p.i][p.j].j = u.j;
									Q.push(make_pair(d[p.i][p.j], p)); 
								}
							}
						}
						else if(s == "Cave"){
							if(not unitat){
								int c = 0;
								if(t == Rock) c = cell(p).turns;
									if(d[p.i][p.j] > d[u.i][u.j] + (c+1) and (d[u.i][u.j]+(c+1)) < max and not poscave.count(p)){  
										if(t == ct  and cell(p).owner != me()){
											poscave[p] = true;
											found = true;
											aux = p;
											torn.first.first = d[u.i][u.j] + 1;
											torn.second = id;
										}       
									d[p.i][p.j] = d[u.i][u.j] + c+1;
									previo[p.i][p.j].i = u.i;
									previo[p.i][p.j].j = u.j;
									Q.push(make_pair(d[p.i][p.j], p)); 
								}
							}
						}
						else if(s == "Enemic"){
							if((not unitat or (unitat and (unit(id2).type == Dwarf or unit(id2).type == Wizard)  and unit(id2).player != me()))){
								int c = 0;
								if(t == Rock) c = cell(p).turns;
									if(d[p.i][p.j] > d[u.i][u.j] + (c+1)){  
										if(unitat){
											found = true;
											aux = p;
											torn.first.first = d[u.i][u.j] + 1;
											torn.second = id;
										}       
									d[p.i][p.j] = d[u.i][u.j] + c+1;
									previo[p.i][p.j].i = u.i;
									previo[p.i][p.j].j = u.j;
									Q.push(make_pair(d[p.i][p.j], p)); 
								}
							}
						}
						else if(s=="Wizard"){
							if((not unitat or (unitat and unit(id2).type == Wizard and unit(id2).player == me() and i%2 == 0))){
								int c = 0;
								if(t == Rock) c = cell(p).turns;
								if(d[p.i][p.j] > d[u.i][u.j] + (c+1) and (d[u.i][u.j]+(c+1)) < 10){  
									if(unitat){
										found = true;
										aux = p;
										torn.first.first = d[u.i][u.j] + 1;
										torn.second = id;
									}       
									d[p.i][p.j] = d[u.i][u.j] + c+1;
									previo[p.i][p.j].i = u.i;
									previo[p.i][p.j].j = u.j;
									Q.push(make_pair(d[p.i][p.j], p)); 
								}
							}
						}
						else if(s=="Balrog"){
							if(t != Rock and not unitat){
								int c = 0;
									if(d[p.i][p.j] > d[u.i][u.j] + (c+1)){  
										if(t == Outside){
											found = true;
											aux = p;
											torn.first.first = d[u.i][u.j] + 1;
											torn.second = id;
										}       
									d[p.i][p.j] = d[u.i][u.j] + c+1;
									previo[p.i][p.j].i = u.i;
									previo[p.i][p.j].j = u.j;
									Q.push(make_pair(d[p.i][p.j], p)); 
								}
							}
						}
						else if((not unitat or (unitat and (unit(id2).type == Dwarf or (unit(id2).type == Wizard and i%2 == 0))  and unit(id2).player == me()))){
							int c = 0;
							if(t == Rock) c = cell(p).turns;
								if(d[p.i][p.j] > d[u.i][u.j] + (c+1)){  
									if(unitat){
										found = true;
										aux = p;
										torn.first.first = d[u.i][u.j] + 1;
										torn.second = id;
									}       
								d[p.i][p.j] = d[u.i][u.j] + c+1;
								previo[p.i][p.j].i = u.i;
								previo[p.i][p.j].j = u.j;
								Q.push(make_pair(d[p.i][p.j], p)); 
							}
						}
					}
				}
            }
        }
    }
    bool sigue = true;
    while(sigue and not Q.empty() and not pob){
        if(previo[aux.i][aux.j] != pm){
			 aux = previo[aux.i][aux.j];
		 }
        else sigue = false;
    }
    if(found and pos_ok(aux) and cell(aux).id == -1) anar[aux]= true;
    if(found) torn.first.second = aux;
    return found;
}

bool dijkstra_w(const int& id,const map<Pos,bool>& postroll,const string& s,map<Pos,int>& agafats,primers& torn){
	Pos pw = unit(id).pos;
	Pos aux;
    selmat selected(60, vector<bool>(60, false));
    aux_info1 d(60, vector<int>(60, 3600));                  //60x60
    aux_info2 previo(60, vector<Pos>(60, Pos(-1,-1)));
    priority_queue<camino, vector<camino>, greater<camino>> Q;
    Q.push(make_pair(0,pw));
    bool amicw = comproba_amics_w(id);
    d[pw.i][pw.j] = 0;
    bool found = false;
    while(not Q.empty() and not found){                          
        Pos u = Q.top().second; Q.pop();                         
        if(not selected[u.i][u.j]){
            selected[u.i][u.j] = true;
            for(int i = 0; i < 4 and not found; ++i) {
                Pos p = u+Dir(2*i);
                if(pos_ok(p) and not selected[p.i][p.j]){
					CellType t = cell(p).type;
					int id2 = cell(p).id;
					bool unitat = (id2 != -1);
					if(s == "Wizard"){
						if((t==Cave or t==Outside) and (not unitat or (unitat and unit(id2).type == Wizard and unit(id2).player == me())) and not postroll.count(p) and d[u.i][u.j] + 1 < 6){
							if(unitat){
								found = true;
								aux = p;
								torn.first.first = d[u.i][u.j] + 1;
								torn.second = id;
							}
							if(d[p.i][p.j] > d[u.i][u.j] + 1){                   
								d[p.i][p.j] = d[u.i][u.j] + 1;
								previo[p.i][p.j].i = u.i;
								previo[p.i][p.j].j = u.j;
								Q.push(make_pair(d[p.i][p.j], p)); 
							}
						}
					}
					else if(s=="D") {
						if((t==Cave or t==Outside) and (not unitat or (unitat and (unit(id2).type == Dwarf or unit(id2).type == Wizard) and unit(id2).player == me())) and not postroll.count(p)){
							if(unitat and unit(id2).type == Dwarf and (not agafats.count(p) or agafats[p] < 2)){
								if(not agafats.count(p)) agafats[p] = 1;
								else ++agafats[p];
								found = true;
								aux = p;
								torn.first.first = d[u.i][u.j] + 1;
								torn.second = id;
							}
							if(d[p.i][p.j] > d[u.i][u.j] + 1){                   
								d[p.i][p.j] = d[u.i][u.j] + 1;
								previo[p.i][p.j].i = u.i;
								previo[p.i][p.j].j = u.j;
								Q.push(make_pair(d[p.i][p.j], p)); 
							}
							
						}
					}
					else if(s=="Balrog") {
						if((t==Cave or t==Outside) and not unitat and not postroll.count(p)){
							if(cell(p).type == Outside){
								found = true;
								aux = p;
								torn.first.first = d[u.i][u.j] + 1;
								torn.second = id;
							}
							if(d[p.i][p.j] > d[u.i][u.j] + 1){                   
								d[p.i][p.j] = d[u.i][u.j] + 1;
								previo[p.i][p.j].i = u.i;
								previo[p.i][p.j].j = u.j;
								Q.push(make_pair(d[p.i][p.j], p)); 
							}
							
						}
					}
					else {
						if((t==Cave or t==Outside) and (not unitat or (unitat and ((unit(id2).type == Dwarf and not comproba_enemic_adj(u)) or unit(id2).type == Wizard) and unit(id2).player == me())) and not postroll.count(p) and (amicw or not comproba_enemic_adj(u) or (d[u.i][u.j] == 0 and not unitat)) and d[u.i][u.j] + 1 < 10){
							if(unitat and unit(id2).type == Dwarf and (not agafats.count(p) or agafats[p] < 2) and (not comproba_enemic_adj(u) or amicw)){
								if(not agafats.count(p)) agafats[p] = 1;
								else ++agafats[p];
								found = true;
								aux = p;
								torn.first.first = d[u.i][u.j] + 1;
								torn.second = id;
							}
							if(d[p.i][p.j] > d[u.i][u.j] + 1){                   
								d[p.i][p.j] = d[u.i][u.j] + 1;
								previo[p.i][p.j].i = u.i;
								previo[p.i][p.j].j = u.j;
								Q.push(make_pair(d[p.i][p.j], p)); 
							}
							
						}
					}
				}
            }
        }
    }
    bool sigue = true;
    while(sigue and not Q.empty()){
        if(previo[aux.i][aux.j] != pw){
			 aux = previo[aux.i][aux.j];
		 }
        else sigue = false;
    }
    if(found) torn.first.second = aux;
    return found;
}

void move_dwarves(map<Pos,bool>& postroll,set<primers>& ordre,set<primers>& atacants) {
	map<Pos,bool> tpillat;
	map<Pos,int> dtoenemy;
	map<Pos,bool> poscave;
	map<Pos,bool> anar;
	VI D = dwarves(me());
    for (int id : D) {
		VP obj;
		bool bobj = false;
		VP objo;
		bool bobjo = false;
		//bool amic = comproba_amics_w(id); 
		int d = 9;
		int amicd = 0;
		int amicw = 0;
		bool pob = false;
		comproba_d(id,d,amicd,obj,bobj,objo,bobjo,amicw,tpillat,dtoenemy);
		primers torn;
		Pos aux;
		Pos lloc;
		if(postroll.count(unit(id).pos) and dijkstra_d(id,aux,"Balrog",pob,lloc,postroll,poscave,anar,torn))  {
			if(comproba_enemic_adj(unit(id).pos) and not comproba_amics_w(id)){
				torn.first.first = 0;
				atacants.insert(torn);
			}
			else ordre.insert(torn);
		}
		else if(bobj and comproba_adjecent(id,obj[0])) {
			if(unit(cell(obj[0]).id).type != Wizard and unit(id).health < 40 and unit(id).health < unit(cell(obj[0]).id).health - 5 and dijkstra_d(id,aux,"Wizard",pob,lloc,postroll,poscave,anar,torn)){
				torn.first.first = -1*(unit(id).health);
				atacants.insert(torn); 
			}
			else {
				torn.first.first = unit(cell(obj[0]).id).health;
				//torn.first.first = unit(id).health;
				torn.first.second = obj[0];
				torn.second= id;
				atacants.insert(torn);
			}
		}
		else if(bobjo and comproba_adjecent(id,objo[0])){
			torn.first.first = 99;
			torn.first.second = objo[0];
			torn.second= id;
			ordre.insert(torn); 
		}
		else if(bobj){
			pob = true;
			lloc = obj[0];
		}
		else if (bobjo){
			pob = true;
			lloc = objo[0];
		}
		else if (dijkstra_d(id,aux,"Cave",pob,lloc,postroll,poscave,anar,torn)) {
			ordre.insert(torn);
		}
		else if (dijkstra_d(id,aux,"Rock",pob,lloc,postroll,poscave,anar,torn)) {
			ordre.insert(torn);
		}
		else {
			pob = false;
		}
		if((bobj or bobjo) and pob and dijkstra_d(id,aux,"",pob,lloc,postroll,poscave,anar,torn)){
			//cout<<"   "<<torn.first.first<<" "<<unit(id).pos.i<<"  "<<unit(id).pos.j<<endl; 
			pob = false;
			if(unit(id).health < 40 and dijkstra_d(id,aux,"Wizard",pob,lloc,postroll,poscave,anar,torn)){
				ordre.insert(torn);
			}
			else if(bobj){
				if (torn.first.first != 2){
					torn.first.first = 0;
					ordre.insert(torn);
				}
				else if(unit(cell(obj[0]).id).type == Wizard){
					if(comproba_d_adjecent(cell(obj[0]).id)){
						 torn.first.first = 900;
						 atacants.insert(torn);
					 }
					else {
						torn.first.first = 998;
						ordre.insert(torn);
					}
					
				}
				else {
					torn.first.first = 999;
					ordre.insert(torn);
				}
			}
			else ordre.insert(torn);
		 }
	}
	
}

void registra(const int& id,map<Pos,bool>& area,const Pos& aux){
	for(int i= 0;i<8;++i){
		Pos p = aux + Dir(i);
		if(pos_ok(p)) area[p] = true;
	}
}

void move_wizards(const map<Pos,bool>& postroll,set<primers>& ordre,set<primers>& atacants) {
	VI W = wizards(me());
	map<Pos,int> agafats;
	primers torn;
    for (int id : W) {
		if(postroll.count(unit(id).pos) and dijkstra_w(id,postroll,"Balrog",agafats,torn)) {
			ordre.insert(torn);
		}
		else if(dijkstra_w(id,postroll,"",agafats,torn)) {
			if(comproba_enemic_adj(unit(id).pos) and not comproba_amics_w(id)){
				torn.first.first = 0;
				atacants.insert(torn);
			}
			else ordre.insert(torn);
		}
		else if(dijkstra_w(id,postroll,"D",agafats,torn)) {
			ordre.insert(torn);
		}
		else if(dijkstra_w(id,postroll,"Wizard",agafats,torn)){
			ordre.insert(torn);
		}
	}
}

  /**
   * Play method, invoked once per each round.
   */
 void play () {
	 if(round()==0){
		  Pos p;
		  for(int i=0;i<60 and trolls.size()<4;++i){
			  p.i = i;
			  for(int j=0;j<60 and trolls.size()<4;++j){
				  p.j = j;
				  if(pos_ok(p) and cell(p).id != -1 and unit(cell(p).id).type == Troll) trolls.push_back(cell(p).id);
			  }
		  }
	  }
	map<Pos,bool> postroll;
	posicions_trolls(postroll);
	set<primers> ordre; set<primers> atacants;
    move_dwarves(postroll,ordre,atacants);
    move_wizards(postroll,ordre,atacants);
    
    for(set<primers>::iterator it = atacants.begin();it!= atacants.end();++it){
		go_d((*it).second,(*it).first.second);
	}
    for(set<primers>::iterator it = ordre.begin();it!= ordre.end();++it){
		go_d((*it).second,(*it).first.second);
	}
  }
};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
