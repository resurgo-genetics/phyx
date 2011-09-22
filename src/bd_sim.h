
#ifndef BD_SIM_H_
#define BD_SIM_H_

#include <string>
#include <vector>

using namespace std;

#include "tree.h"
#include "node.h"

class BirthDeathSimulator{
private:
	int failures;
	int maxfailures;
	double birthrate;
	double deathrate;
	double sumrate;
	double relative_birth_rate;
	double extantstop;
	double timestop;
	int numofchanges;
	double currenttime;
	vector<Node*> extantnodes;
	map<Node*,double> BIRTHTIME;
	map<Node*,double> DEATHTIME;
	Node* root;
	Tree* tree;

	bool check_stop_conditions();
	double time_to_next_sp_event();
	void event();
	void node_death(Node *);
	void node_birth(Node *);
	void delete_dead_nodes();
	void setup_parameters();
	bool event_is_birth();
	void delete_a_node(Node *);
	double get_distance_from_tip(Node *innode);
	void set_distance_to_tip();

public:
	BirthDeathSimulator();
	BirthDeathSimulator(double,double,double,double);

	Tree * make_tree(bool);

	//~BirthDeathSimulator();
};

#endif