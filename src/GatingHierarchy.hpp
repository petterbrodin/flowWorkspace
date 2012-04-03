/*
 * GatingHierarchy.hpp
 *
 *  Created on: Mar 17, 2012
 *      Author: mike
 */

#ifndef GATINGHIERARCHY_HPP_
#define GATINGHIERARCHY_HPP_
#include <iostream>
#include <string>
#include <vector>
#include "populationTree.hpp"
#include "flowJoWorkspace.hpp"
#include <libxml/xpath.h>
using namespace std;


struct OurVertexPropertyWriter {

	OurVertexPropertyWriter(populationTree &g_) : g(g_) {}

    template <class Vertex>
    void operator() (std::ostream &out, Vertex u) {

    	out<<"[shape=record,label=\"{"<<g[u].getName()<<"|count:"<<g[u].fjStats["count"]<<"|"<<typeid(*g[u].getGate()).name()<<"}\"]";


    }

    populationTree &g;
};
class transformation{

};

class compensation{

};


/*GatingHierarchy is a tree that holds the gate definitions hierarchically,
 along with the transformation functions and compensation matrix,
 Once the one FCS file is associated,the tree can also hold indices that subset the events
 It can serves as a gating template when data is empty
 */

class GatingHierarchy{
//	transformation trans;
//	compensation comp;
//	double ** data;

	populationTree tree;
	map<string,VertexID> nodelist;
	workspace * thisWs;
public:
	string sampleName;
	/*retrieve the gate definition from a particular node*/
//	gate getGate(unsigned short gateid);
//	gate getGate(string popName);

	/*remove the gate from */
//	void removeGate(unsigned short popId);
//	void removeGate(string popName);

	/*append the gate to the tree*/
	void addChild(VertexID parent,VertexID child);
	void addGate(gate& g,string popName);
	void addPopulation(VertexID parentID,wsNode * parentNode);
	VertexID addRoot(populationNode rootNode);
	GatingHierarchy();
	~GatingHierarchy();
	GatingHierarchy(string sampleID,workspace * ws);
	GatingHierarchy(wsSampleNode curSampleNode,workspace * ws);

	/*associate the tree with data matrix*/
//	void addData();

	/**/
//	vector<bool> * getIndice(string popName);
//	vector<bool> * getData(string popName);

//	void gating(gate& g,string popName);
	void gating();
	string drawGraph();
};



#endif /* GATINGHIERARCHY_HPP_ */