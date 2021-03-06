/*
 * R_GatingSet.cpp
 *
 *these are R APIs
 *
 *  Created on: Mar 30, 2012
 *      Author: wjiang2
 */

/*
 * can't use module for exposing overloaded methods and non-standard wrap/as type of the constructor
 * Also each GatingHierarchy object is created by GatingSet method within c++
 * thus it is not initialized by Rcpp module as S4 class within R. So have to use this tedious way to
 * write R API
 */

#include "include/R_GatingHierarchy.hpp"
#include "include/R_GatingSet.hpp"
#include <stdexcept>
#include "include/gate.hpp"
#include "include/transformation.hpp"
using namespace std;

/*
 * only expose gating set pointer to R to avoid gc() by R
 */
RcppExport SEXP R_plotGh(SEXP _gsPtr,SEXP _sampleName,SEXP _output) {
BEGIN_RCPP



	GatingSet *	gs=getGsPtr(_gsPtr);

	string sampleName=as<string>(_sampleName);

	GatingHierarchy *gh=gs->getGatingHierarchy(sampleName);

	string output=as<string>(_output);
 	gh->drawGraph(output);

END_RCPP
}


/*
 * return node names as a character vector
 */
RcppExport SEXP R_getNodes(SEXP _gsPtr,SEXP _sampleName,SEXP _order,SEXP _isPath){
BEGIN_RCPP

	GatingSet *	gs=getGsPtr(_gsPtr);

	string sampleName=as<string>(_sampleName);

	GatingHierarchy* gh=gs->getGatingHierarchy(sampleName);
	unsigned short order=as<unsigned short>(_order);
	bool isPath=as<bool>(_isPath);

	return wrap(gh->getPopNames(order,isPath));
END_RCPP
}

RcppExport SEXP R_getParent(SEXP _gsPtr,SEXP _sampleName,SEXP _i){
BEGIN_RCPP



	GatingSet *	gs=getGsPtr(_gsPtr);

	string sampleName=as<string>(_sampleName);
	GatingHierarchy *gh=gs->getGatingHierarchy(sampleName);
	int u=as<int>(_i);
	return wrap(gh->getParent(u));
END_RCPP
}

RcppExport SEXP R_getChildren(SEXP _gsPtr,SEXP _sampleName,SEXP _i){
BEGIN_RCPP



	GatingSet *	gs=getGsPtr(_gsPtr);

	string sampleName=as<string>(_sampleName);
	GatingHierarchy* gh=gs->getGatingHierarchy(sampleName);
	int u=as<int>(_i);
	return wrap(gh->getChildren(u));
END_RCPP
}

RcppExport SEXP R_getPopStats(SEXP _gsPtr,SEXP _sampleName,SEXP _i){
BEGIN_RCPP



	GatingSet *	gs=getGsPtr(_gsPtr);

	string sampleName=as<string>(_sampleName);
	GatingHierarchy *gh=gs->getGatingHierarchy(sampleName);
	int u=as<int>(_i);
	nodeProperties *node=gh->getNodeProperty(u);

	return List::create(Named("FlowCore",node->getStats(true))
						,Named("FlowJo",node->getStats(false))
						);

END_RCPP
}



RcppExport SEXP R_getCompensation(SEXP _gsPtr,SEXP _sampleName){
BEGIN_RCPP



	GatingSet *	gs=getGsPtr(_gsPtr);

	string sampleName=as<string>(_sampleName);

	GatingHierarchy* gh=gs->getGatingHierarchy(sampleName);
	compensation comp=gh->getCompensation();
	return(List::create(Named("cid",comp.cid)
						,Named("prefix",comp.prefix)
						,Named("suffix",comp.suffix)
						,Named("comment",comp.comment)
						,Named("parameters",comp.marker)
						,Named("spillOver",comp.spillOver))
			);

END_RCPP
}



RcppExport SEXP R_getTransformations(SEXP _gsPtr,SEXP _sampleName){
BEGIN_RCPP


	GatingSet *	gs=getGsPtr(_gsPtr);

	string sampleName=as<string>(_sampleName);

	GatingHierarchy* gh=gs->getGatingHierarchy(sampleName);
	map<string,transformation* > trans=gh->getLocalTrans().getTransMap();
	List res;

	for (map<string,transformation* >::iterator it=trans.begin();it!=trans.end();it++)
	{
		transformation * curTrans=it->second;
		if(curTrans==NULL)
			throw(domain_error("empty transformation for channel"+it->first));

		string transName=curTrans->getName()+" "+it->first;

		switch(curTrans->getType())
		{

			case LOG:
			{

				res.push_back(List::create(Named("type","log"))
								,transName
								);
				break;
			}
			case LIN:
			{

				res.push_back(List::create(Named("type","lin"))
								,transName
								);
				break;
			}
			case CALTBL:
			{
				if(!curTrans->isInterpolated())
					throw(domain_error("non-interpolated calibration table:"+curTrans->getName()+curTrans->getChannel()+" from channel"+it->first));
				Spline_Coefs obj=curTrans->getSplineCoefs();

				res.push_back(List::create(Named("z",obj.coefs)
											,Named("method",obj.method)
											,Named("type",obj.type)
											)
								,transName
								);
				break;
			}
			default:
				throw(domain_error("unknown transformation in R_getTransformations!"));
		}

	}
	return (res);



END_RCPP
}
/*
 * cdf version
 * Deprecated: we don't want to keep a separate view of ncdfFlowSet in c++ due to the
 * data consistency concerns
 * even though it increases the communication overhead from R to C by using R_gating method
 */
//RcppExport SEXP R_gating_cdf(SEXP _gsPtr,SEXP _sampleName,SEXP _nodeInd,SEXP _recompute){
//BEGIN_RCPP
//
//
//
//	XPtr<GatingSet>gs(_gsPtr);
//
//	string sampleName=as<string>(_sampleName);
//	unsigned short nodeInd=as<unsigned short>(_nodeInd);
//	bool recompute=as<bool>(_recompute);
//
//	GatingHierarchy* gh=gs->getGatingHierarchy(sampleName);
//	gh->loadData(sampleName);
//	/*
//	 * assume the data has been transformed when recompute==true
//	 */
//	if(!recompute)
//	{
//		gh->extendGate();
//		gh->transforming(true);
//	}
//
//	gh->gating(nodeInd,recompute);
//	gh->unloadData();
//
//END_RCPP
//}
/*
 * non-cdf version
 */

RcppExport SEXP R_gating(SEXP _gsPtr,SEXP _mat,SEXP _sampleName,SEXP _nodeInd,SEXP _recompute){
BEGIN_RCPP



	GatingSet *	gs=getGsPtr(_gsPtr);

	string sampleName=as<string>(_sampleName);
	unsigned short nodeInd=as<unsigned short>(_nodeInd);
	bool recompute=as<bool>(_recompute);
	GatingHierarchy* gh=gs->getGatingHierarchy(sampleName);

	Rcpp::NumericMatrix orig(_mat);
	unsigned sampleID=numeric_limits<unsigned>::max();//dummy sample index
	flowData fdata(orig,sampleID);

	gh->loadData(fdata);
	if(!recompute)
	{
		gh->extendGate();
		gh->transforming(false);
	}

	gh->gating(nodeInd,recompute);

	if(!recompute)
	{
		/*
		 * copy the transformed data from gh before unload it
		 */
		valarray<double> updatedMat(gh->getData(0).getData());


		/*
		 * update the _mat
		 */
		for(int j=0;j<orig.ncol()*orig.nrow();j++)
			orig[j]=updatedMat[j];
	}
	gh->unloadData();

END_RCPP
}


RcppExport SEXP R_getGate(SEXP _gsPtr,SEXP _sampleName,SEXP _i){
BEGIN_RCPP



	GatingSet *	gs=getGsPtr(_gsPtr);

	string sampleName=as<string>(_sampleName);
	int u=as<int>(_i);

	GatingHierarchy* gh=gs->getGatingHierarchy(sampleName);
	if(u<0)
		throw(domain_error("not valid vertexID!"));
	if(u==0)
		throw(domain_error("no gate associated with root node."));
	gate *g=gh->getNodeProperty(u)->getGate();
	unsigned short gType=g->getType();
	if(gType==ELLIPSEGATE)
		gType=POLYGONGATE;
	switch(gType)
	{
		case POLYGONGATE:
			{
				vertices_vector vert=g->getVertices().toVector();

				 List ret=List::create(Named("parameters",g->getParamNames())
						 	 	 	 	 ,Named("x",vert.x),Named("y",vert.y)
						 	 	 	 	 ,Named("type",POLYGONGATE)
						 	 	 	 	 );
				return ret;
			}

		case RANGEGATE:
			{
				vertices_vector vert=g->getVertices().toVector();

				List ret=List::create(Named("parameters",g->getParamNames())
									 ,Named("range",vert.x)
									 ,Named("type",RANGEGATE)
									 );
				return ret;
			}
		case BOOLGATE:
			{
			  boolGate * bg=dynamic_cast<boolGate*>(g);
			  vector<BOOL_GATE_OP> boolOpSpec=bg->getBoolSpec();
			  vector<string> v;
			  vector<char>v2;
			  vector<vector<string> >ref;
			  for(vector<BOOL_GATE_OP>::iterator it=boolOpSpec.begin();it!=boolOpSpec.end();it++)
			  {
				  v.push_back(it->isNot?"!":"");
				  v2.push_back(it->op);
				  ref.push_back(it->path);
			  }

			  List ret=List::create(Named("v",v)
									 ,Named("v2",v2)
									 ,Named("ref",ref)
									 ,Named("type",BOOLGATE)
									 );
			  return ret;

			}
		default:
		{
//			cout<<g->getType()<<endl;
			throw(domain_error("unknown gate thrown by R_getGate!"));
		}

	}

END_RCPP
}

RcppExport SEXP R_getIndices(SEXP _gsPtr,SEXP _sampleName,SEXP _i){
BEGIN_RCPP


	GatingSet *	gs=getGsPtr(_gsPtr);

	string sampleName=as<string>(_sampleName);
	int u=as<int>(_i);
	if(u<0)throw(domain_error("not valid vertexID!"));

	GatingHierarchy* gh=gs->getGatingHierarchy(sampleName);
	return wrap(gh->getNodeProperty(u)->getIndices());

END_RCPP
}

RcppExport SEXP R_addGate(SEXP _gsPtr,SEXP _sampleName,SEXP _filter,SEXP _parentID,SEXP _popName) {
BEGIN_RCPP


		GatingSet *	gs=getGsPtr(_gsPtr);
		string sampleName=as<string>(_sampleName);
		GatingHierarchy* gh=gs->getGatingHierarchy(sampleName);

		unsigned parentID=as<unsigned>(_parentID);
		string popName=as<string>(_popName);

		/*
		 * convert R filter to specific gate class
		 */
		List filter=as<List>(_filter);

		StringVec names=filter.names();

		unsigned short gateType=as<unsigned short>(filter["type"]);

		bool isNeg=as<bool>(filter["negated"]);
		gate * g;

		switch(gateType)
		{
			case RANGEGATE:
			{
				StringVec params=as<StringVec>(filter["params"]);
				rangegate * rg=new rangegate();
				rg->setNegate(isNeg);

				DoubleVec p=as<DoubleVec>(filter["range"]);

				paramRange pRange;
				pRange.setName(params.at(0));
				pRange.setMin(p.at(0));
				pRange.setMax(p.at(1));

				rg->setParam(pRange);

				g=rg;

				break;
			}
			case POLYGONGATE:
			{
				StringVec params=as<StringVec>(filter["params"]);
				polygonGate * pg=new polygonGate();

				pg->setNegate(isNeg);

				paramPoly pp;
				pp.setName(params);

				vector<coordinate> v;
				NumericMatrix boundaries=as<NumericMatrix>(filter["boundaries"]);
				for(int i=0;i<boundaries.nrow();i++)
				{
					coordinate pCoord;
					pCoord.x=boundaries(i,0);
					pCoord.y=boundaries(i,1);
					v.push_back(pCoord);

				}
				pp.setVertices(v);

				pg->setParam(pp);

				g=pg;

				break;
			}
			case RECTGATE:
			{
				StringVec params=as<StringVec>(filter["params"]);
				rectgate * rectg=new rectgate();

				rectg->setNegate(isNeg);

				paramPoly pp;
				pp.setName(params);

				vector<coordinate> v;
				NumericMatrix boundaries=as<NumericMatrix>(filter["boundaries"]);
				for(int i=0;i<boundaries.nrow();i++)
				{
					coordinate pCoord;
					pCoord.x=boundaries(i,0);
					pCoord.y=boundaries(i,1);
					v.push_back(pCoord);

				}
				pp.setVertices(v);

				rectg->setParam(pp);

				g=rectg;
				break;

			}
			case BOOLGATE:
			{
				boolGate * bg=new boolGate();

				bg->setNegate(isNeg);
				/*
				 * get specification from R
				 */
				StringVec refs=as<StringVec>(filter["refs"]);
				StringVec op=as<StringVec>(filter["op"]);
				BoolVec isNot=as<BoolVec>(filter["isNot"]);

				/*
				 * convert to c class
				 */
				vector<BOOL_GATE_OP> res;
				for(unsigned i=0;i<refs.size();i++)
				{

					BOOL_GATE_OP gOpObj;

					boost::split(gOpObj.path,refs.at(i),boost::is_any_of("/"));
					if(gOpObj.path.at(0).empty())
						gOpObj.path.erase(gOpObj.path.begin());//remove the first empty string

					gOpObj.isNot=isNot.at(i);
					gOpObj.op=boost::iequals(op.at(i),"|")?'|':'&';

					res.push_back(gOpObj);
				}
				bg->boolOpSpec=res;
				g=bg;
				break;

			}
			case LOGICALGATE:
			{
				//TODO:create and implement logical gate class
				throw(domain_error("LOGICALGATE is not unsupported yet!"));
				break;
			}
			default:
				throw(domain_error("unsupported gate type!valid types: POLYGONGATE(1),RANGEGATE(2),BOOLGATE(3),RECTGATE(5),LOGICALGATE(6)"));

		}



		VertexID nodeID=gh->addGate(g,parentID,popName);


		return wrap(nodeID);



END_RCPP
}

RcppExport SEXP R_removeNode(SEXP _gsPtr,SEXP _sampleName,SEXP _nodeID) {
BEGIN_RCPP


		GatingSet *	gs=getGsPtr(_gsPtr);
		string sampleName=as<string>(_sampleName);
		GatingHierarchy* gh=gs->getGatingHierarchy(sampleName);


		unsigned nodeID=as<unsigned>(_nodeID);



		gh->removeNode(nodeID);






END_RCPP
}

