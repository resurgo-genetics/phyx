/*
 * main_bd.cpp
 *
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <getopt.h>
#include <algorithm>
#include <iterator>
#include <cmath>

using namespace std;

#include "tree_reader.h"
#include "tree.h"
#include "tree_utils.h"
#include "utils.h"
#include "bd_fit.h"
#include "log.h"

/*
Give two options:
1. pass in 2 trees
2. pass in 1 distribution of trees
 */

void print_help () {
    cout << "Combine a set of trees from one file into a tree from another." << endl;
    cout << "Pass in 2 trees with `t` and `a`." << endl;
    cout << endl;
    cout << "Usage: pxtcomb [OPTION]... " << endl;
    cout << endl;
    cout << " -t, --treef=FILE    reference treefile, stdin otherwise" << endl;
    cout << " -a, --addtree=FILE  alternate treefile" << endl;
    cout << " -o, --outf=FILE     output file, stout otherwise" << endl;
    cout << " -h, --help          display this help and exit" << endl;
    cout << " -V, --version       display version and exit" << endl;
    cout << endl;
    cout << "Report bugs to: <https://github.com/FePhyFoFum/phyx/issues>" << endl;
    cout << "phyx home page: <https://github.com/FePhyFoFum/phyx>" << endl;
}

string versionline("pxtcomb 0.1\nCopyright (C) 2017 FePhyFoFum\nLicense GPLv3\nwritten by Joseph W. Brown, Stephen A. Smith (blackrim)");

static struct option const long_options[] =
{
    {"treef", required_argument, NULL, 't'},
    {"addtree", required_argument, NULL, 'a'},
    {"outf", required_argument, NULL, 'o'},
    {"help", no_argument, NULL, 'h'},
    {"version", no_argument, NULL, 'V'},
    {NULL, 0, NULL, 0}
};

int main(int argc, char * argv[]) {
    
    log_call(argc, argv);
    
    bool outfileset = false;
    bool tfileset = false;
    bool addfileset = false;
    
    char * treef = NULL;
    char * addtreef = NULL;
    char * outf = NULL;
    
    while (1) {
        int oi = -1;
        int c = getopt_long(argc, argv, "t:a:o:x:hV", long_options, &oi);
        if (c == -1) {
            break;
        }
        switch(c) {
            case 't':
                tfileset = true;
                treef = strdup(optarg);
                check_file_exists(treef);
                break;
            case 'a':
                addfileset = true;
                addtreef = strdup(optarg);
                check_file_exists(addtreef);
                break;
            case 'o':
                outfileset = true;
                outf = strdup(optarg);
                break;
            case 'h':
                print_help();
                exit(0);
            case 'V':
                cout << versionline << endl;
                exit(0);
            default:
                print_error(argv[0], (char)c);
                exit(0);
        }
    }
    
    istream* pios = NULL;
    istream* apios = NULL;
    ostream* poos = NULL;
    ifstream* fstr = NULL;
    ifstream* afstr = NULL;
    ofstream* ofstr = NULL;

    if (outfileset == true) {
        ofstr = new ofstream(outf);
        poos = ofstr;
    } else {
        poos = &cout;
    }
    if (tfileset == true) {
        fstr = new ifstream(treef);
        pios = fstr;
    } else {
        cout << "you need to set an tfile (-t)" << endl;
    }
    if (addfileset == true){
        afstr = new ifstream(addtreef);
        apios = afstr;
    } else{
        cout << "you need to set an addfile (-a)" << endl;
    }
    
    string retstring;
    int ft = test_tree_filetype_stream(*pios, retstring);
    if (ft != 0 && ft != 1) {
        cerr << "this really only works with nexus or newick" << endl;
        exit(0);
    }
    
    bool going = true;
    Tree * bigtree;
    while (going) {
        if (retstring.size() > 1)
            bigtree = read_next_tree_from_stream_newick (*pios, retstring, &going);
        else
            going = false;
    }
    set<string> btns = bigtree->getRoot()->get_leave_names_set();

    ft = test_tree_filetype_stream(*apios, retstring);
    if (ft != 0 && ft != 1) {
        cerr << "this really only works with nexus or newick" << endl;
        exit(0);
    }
    going = true;
    while (going) {
        if (retstring.size() > 1){
            Tree * addtree = read_next_tree_from_stream_newick (*apios, retstring, &going);
            set<string> atns = addtree->getRoot()->get_leave_names_set();
            vector<string> v(btns.size());
            vector<string>::iterator it = set_difference(btns.begin(),btns.end(),atns.begin(),atns.end(),v.begin());
            v.resize(it-v.begin());
            map<string,Node *> diffnds;
            for (int i = 0; i<bigtree->getExtantNodeCount();i++){
                if (count(v.begin(),v.end(),bigtree->getExternalNode(i)->getName())==1)
                    diffnds[bigtree->getExternalNode(i)->getName()] = bigtree->getExternalNode(i);
            }
            bool didit = false;
            while (diffnds.size() > 0){
                for (map<string,Node*>::iterator it = diffnds.begin(); it != diffnds.end(); ++it){
                    //cout << it->first << endl;
                    Node * cn = it->second;
                    set <string> cs = cn->get_leave_names_set();
                    bool goi = true;
                    while (goi == true){
                        Node * prn = cn;
                        cn = prn->getParent();
                        cs = cn->get_leave_names_set();
                        vector<string> v_int;
                        set_intersection(cs.begin(),cs.end(),atns.begin(),atns.end(),back_inserter(v_int));
                        if(v_int.size() > 0){
                            //cout <<  "this is what we need to add " << prn->getNewick(false) << endl;
                            //get nodes
                            //get mrca
                            Node * nd = addtree->getMRCA(v_int);
                            //cout << "would add to " <<  nd->getNewick(false) << endl;
                            if (addtree->getRoot() == nd){
                                Node * newroot = new Node();
                                newroot->addChild(*prn);
                                newroot->addChild(*nd);
                                addtree->setRoot(newroot);
                            }else{
                                if (nd->getChildCount() == 0){
                                    Node p = (*nd->getParent());
                                    Node * nn = new Node(p);
                                    p.addChild(*nn);
                                    p.removeChild(*nd);
                                    nn->addChild(*nd);
                                    nn->addChild(*prn);
                                    cout << nn->getNewick(false) << endl;
                                }else{
                                    nd->addChild(*prn);
                                }
                            }
                            vector<string> lvsnms = prn->get_leave_names();
                            for(int i =0; i < lvsnms.size(); i++){
                                if (diffnds.count(lvsnms[i]) > 0){
                                    diffnds.erase(lvsnms[i]);
                                }
                            }
                            going = false;
                            didit = true;
                            break;
                        }
                    }
                    if (didit == true)
                        break;
                }
            }
            (*poos) << addtree->getRoot()->getNewick(true) << ";" << endl;
            delete addtree;
        }else
            going = false;
    }
    

    if (outfileset) {
        ofstr->close();
        delete poos;
    }
    return EXIT_SUCCESS;
}