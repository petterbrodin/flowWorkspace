/*
 * gatingSet_test.cpp
 *
 *  Created on: May 15, 2012
 *      Author: wjiang2
 */

#include "test_header.hpp"

void gs_test(string xml){

		//create gating set object
		GatingSet gs(xml,true,4);

//		valarray<double> x(gs.ws->toArray(""));
//		for(unsigned i=0;i<x.size();i++)
//			cout<<x[i]<<endl;
		//parse a particular sample group
//		unsigned short groupID=0;
//		cout<<endl<<"parseWorkspace for Group:"<<groupID<<endl;
//		gs.parseWorkspace(groupID,true);

		//parse a set of sampleIDs
		vector<string> sampleIDs;
//		sampleIDs.push_back("1");
//		sampleIDs.push_back("2");
		sampleIDs.push_back("14");
		sampleIDs.push_back("13");
		gs.parseWorkspace(sampleIDs,true);
		/*
		 * get sample list from gating set
		 */
		cout<<endl<<"get samples from gating set"<<endl;

		vector<string> samples=gs.getSamples();
		for(vector<string>::iterator it=samples.begin();it!=samples.end();it++)
			cout<<*it<<endl;

		GatingHierarchy* gh;
		gh=gs.getGatingHierarchy("Exp2_Sp005_1_Tcell.fcs");
//		gh=gs.getGatingHierarchy("Specimen_001_A1_A01.fcs");
//		gh=gs.getGatingHierarchy("004_A1_A01.fcs");
//		gh=gs.getGatingHierarchy("004_B1_B01.fcs");
//		gh=gs.getGatingHierarchy(0);
		/*
		 * getNodes by the T order
		 */

//		cout<<endl<<"tsorted node list"<<endl;
		VertexID_vec vertices;
		vertices=gh->getVertices(true);
		for(VertexID_vec::iterator it=vertices.begin();it!=vertices.end();it++)
		{
			nodeProperties *node=gh->getNodeProperty(*it);
			cout<<*it<<"."<<node->getName()<<endl;
		}

		/*
		 * getNodes by vertices ID order
		 * and get stats from each node
		 */

		cout<<endl<<"node list in regular order and stats,gate"<<endl;
		vertices=gh->getVertices(false);
		for(VertexID_vec::iterator it=vertices.begin();it!=vertices.end();it++)
		{
			VertexID u=*it;
			nodeProperties *node=gh->getNodeProperty(u);
			cout<<u<<"."<<node->getName()<<":";
			cout<<node->getStats(false)["count"]<<endl;
			if(u!=ROOTNODE)
			{
				cout<<node->getGate()->getName()<<endl;


			}
		}

		/*
		 * getPopNames with full path
		 */
		cout<<endl<<"node list with/without full path:"<<endl;
		vector<string> popNames=gh->getPopNames(false,true);
		for(vector<string>::iterator it=popNames.begin();it!=popNames.end();it++)
			cout<<*it<<endl;
		popNames=gh->getPopNames(false,false);
		for(vector<string>::iterator it=popNames.begin();it!=popNames.end();it++)
			cout<<*it<<endl;

		/*
		 * getSample from gating hierarchy
		 */
		cout<<endl<<"get sample name from gating hierarchy"<<endl;
		cout<<gh->getSample()<<endl;
		/*
		 * get children and parent node index
		 */

		cout<<endl<<"check parent node"<<endl;
		for(int i=-1;i<=11;i++)
		{
			VertexID_vec parent=gh->getParent(i);
			cout<<i<<"<--";
			for(VertexID_vec::iterator it=parent.begin();it!=parent.end();it++)
				cout<<*it<<" ";
			cout<<endl;
		}



		cout<<endl<<"check children node"<<endl;
		for(int i=-1;i<=11;i++)
		{
			VertexID_vec children=gh->getChildren(i);
			cout<<i<<"-->";
			for(VertexID_vec::iterator it=children.begin();it!=children.end();it++)
						cout<<*it<<",";
			cout<<endl;
		}


		cout<<endl<<"do the gating after the parsing"<<endl;
		string ncFile="../output/HIPC_trial/nc1.nc";
//		string ncFile="../output/Yale/ncfs6fff9d1b9e6.nc";
		//read colnames from text
		vector<string> params;
		vector<string> sampleNames;
		sampleNames.push_back("004_A1_A01.fcs");
		sampleNames.push_back("004_B1_B01.fcs");

		std::ifstream myfile;
		myfile.open("../output/HIPC_trial/colnames.txt",ifstream::in);
//		myfile.open("../output/Yale/colnames.txt",ifstream::in);
		vector<string> myLines;
		string line;
		while (std::getline(myfile, line))
		{
			params.push_back(line);
		}

		myfile.close();
		gs.attachData(ncFile,sampleNames,params);
		//read transformed data once for all nodes
		gh->loadData();
//		gh->transforming(true);
		gh->gating();
		gh->unloadData();




		cout<<endl<<"flowJo(flowcore) counts after gating"<<endl;
		vertices=gh->getVertices(false);
		for(VertexID_vec::iterator it=vertices.begin();it!=vertices.end();it++)
		{
			VertexID u=*it;
			nodeProperties *node=gh->getNodeProperty(u);
			cout<<u<<"."<<node->getName()<<":";
			cout<<node->getStats(false)["count"];
			cout<<"("<<node->getStats(true)["count"]<<") "<<endl;

//			if(u!=ROOTNODE)
//				cout<<node->getGate()->getName()<<endl;
		}

		/*
		 * plot gating hierarchy tree
		 */

//		gh->drawGraph("../output/test.dot");
//		system("dot2gxl ../output/test.dot -o ../output/test.gxl");
		//	return("test.gxl");


}