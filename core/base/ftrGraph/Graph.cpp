#include "Graph.h"

#include<iostream>

using namespace std;
using namespace ttk;
using namespace ftr;

Graph::Graph()
{
}

Graph::~Graph()
{
}

void Graph::arcs2nodes(VertCompFN comp)
{
   const idSuperArc nbArcs = getNumberOfArcs();
   const idNode nbNodes    = getNumberOfNodes();

   // fix up down
   for(idSuperArc arcId = 0; arcId < nbArcs; ++arcId) {
      if(!getArc(arcId).isVisible()) continue;

      const idNode upNodeId   = getArc(arcId).getUpNodeId();
      const idNode downNodeId = getArc(arcId).getDownNodeId();

#ifndef TTK_ENABLE_KAMIKAZE
      if (upNodeId == nullNode || downNodeId == nullNode) {
         continue;
      }
#endif

      const idVertex upVertId   = getNode(upNodeId).getVertexIdentifier();
      const idVertex downVertId = getNode(downNodeId).getVertexIdentifier();


      if(comp(upVertId, downVertId)){
         getArc(arcId).setUpNodeId(downNodeId);
         getArc(arcId).setDownNodeId(upNodeId);
      } else {
         getArc(arcId).setUpNodeId(upNodeId);
         getArc(arcId).setDownNodeId(downNodeId);
      }
   }

   // reserve good size
   std::vector<valence> upVal(nbNodes, 0), downVal(nbNodes, 0);
   // count
   for(idSuperArc arcId = 0; arcId < nbArcs; ++arcId) {
      if(!getArc(arcId).isVisible()) continue;

      const idNode upNodeId = getArc(arcId).getUpNodeId();
      const idNode downNodeId = getArc(arcId).getDownNodeId();

#ifndef TTK_ENABLE_KAMIKAZE
      if (upNodeId == nullNode || downNodeId == nullNode) {
         continue;
      }
#endif

      ++downVal[upNodeId];
      ++upVal[downNodeId];
   }
   // alloc
   for(idNode nodeId = 0; nodeId < nbNodes; ++nodeId) {
      getNode(nodeId).reserveUpArc(upVal[nodeId]);
      getNode(nodeId).reserveDownArc(downVal[nodeId]);
   }

   // set the id
   for(idSuperArc arcId = 0; arcId < nbArcs; ++arcId) {
      if(!getArc(arcId).isVisible()) continue;

      const idNode upNodeId = getArc(arcId).getUpNodeId();
      const idNode downNodeId = getArc(arcId).getDownNodeId();

#ifndef TTK_ENABLE_KAMIKAZE
      if (upNodeId == nullNode || downNodeId == nullNode) {
         continue;
      }
#endif

      getNode(upNodeId).addDownArc(arcId);
      getNode(downNodeId).addUpArc(arcId);
   }
}

std::string Graph::print(const int verbosity) const
{
   stringstream res;
   if (verbosity >= 1) {
      res << "Graph:" << endl;
      res << "leaves: " << leaves_.size() << endl;
      res << "nodes: " << nodes_.size() << endl;
      res << "arcs: " << arcs_.size() << endl;
   }

   if(verbosity >= 3) {
      res << "Leaves: " << endl;
      for (const auto v : leaves_) {
         res << get<0>(v) << " ";
      }
      res << endl;
   }

   if(verbosity >= 4) {
      res << "Nodes:" << endl;
      const idNode nbn = nodes_.size();
      for(idNode i = 0; i < nbn; ++i) {
         res << printNode(i) << endl;
      }
      res << "Arcs:" << endl;
      const idSuperArc nba = arcs_.size();
      for(idSuperArc i = 0; i < nba; ++i) {
         res << printArc(i) << endl;
      }
   }
   return res.str();
}

std::string Graph::printArc(const idSuperArc arcId) const
{
   std::stringstream res;
   res << "a" << arcId << ":";
   if (!arcs_[arcId].isVisible()) {
      res << "hidden";
   }
   res << "(";
   const idNode did = arcs_[arcId].getDownNodeId();
   const idNode uid = arcs_[arcId].getUpNodeId();
   if (did != nullNode){
      res << nodes_[did].getVertexIdentifier();
   } else {
      res << "X";
   }
   res << " - ";
   if (uid != nullNode){
      res << nodes_[uid].getVertexIdentifier();
   } else {
      res << "X";
   }
   res << ")";
   return res.str();
}

std::string Graph::printNode(const idNode nodeId) const
{
   std::stringstream res;
   res << "n" << nodeId << "v" << nodes_[nodeId].getVertexIdentifier() << ":[v";
   const idSuperArc nbd = nodes_[nodeId].getNbDownArcs();
   for(idSuperArc i = 0; i < nbd; ++i) {
      res << " " << nodes_[nodeId].getDownArc(i);
   }
   res << " - ^";
   const idSuperArc nbu = nodes_[nodeId].getNbUpArcs();
   for(idSuperArc i = 0; i < nbu; ++i) {
      res << " " << nodes_[nodeId].getUpArc(i);
   }
   res << "]";
   return res.str();
}


std::string Graph::printVisit(const idVertex v) const
{
   std::stringstream res;
   if (isArc(v))
      res << "a: " << getArcId(v);
   if (isNode(v))
      res << "n: " << getNodeId(v);
   return res.str();
}

// DEBUG function

std::string Graph::printVisit() const
{
   std::stringstream res;
   res << "Segmentation: " << std::endl;
   for (idVertex s = 0; s < nbVerts_; ++s) {
      res << s << " : ";
      res << printVisit(s);
      res << std::endl;
   }
   return res.str();
}

// initialization

void Graph::alloc()
{
#ifndef TTK_ENABLE_KAMIKAZE
   if (nbVerts_ == nullVertex) {
      cout << "[FTR Graph]: ERROR, setNumberOfVertices not called before alloc in Graph" << endl;
   }
#endif
   leaves_.reserve(nbVerts_/2);
   nodes_.reserve(nbVerts_);
   arcs_.reserve(nbVerts_);
   segmentation_.resize(nbVerts_);
   valUp_.resize(nbVerts_);
   valDown_.resize(nbVerts_);
}

void Graph::init()
{
   // Consider max valence is 10
   fillVector<SegmInfo>(segmentation_, SegmInfo{});
   fillVector<valence>(valUp_, -1);
   fillVector<valence>(valDown_, -1);
}


