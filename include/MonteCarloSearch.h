//===----------------------- MonteCarloSearch.h ---------------------------------===//
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the MonteCarloSearch class, which  
/// contains a definition of the monte carlo search method
///
//===----------------------------------------------------------------------===//
#ifndef MLSCEDULER_MONTE_CARLO_SEARCH_H_
#define MLSCEDULER_MONTE_CARLO_SEARCH_H_

#include "SearchMethod.h"
#include "Node.h"
#include "EvaluationByExecution.h"
#include "TilingTransformation.h"
#include "InterchangeTransformation.h"
#include "ParallelizationTransformation.h"
#include "VectorizationTransformation.h"

#include <queue>
#include <list>
#include <random>
#include <algorithm>

using namespace mlir;


/// A subclass of the node class in order to add specific montecarlo attributes such as the visit count
class MonteCarloNode : public Node{
    private:
        MonteCarloNode * parent;
        // SmallVector<MonteCarloNode *, 2> children;
        int visitCount;
        double bestValue;
        bool isFullyExpanded;
        int level;
        int index;
    public:
    static std::mt19937 gen;
        /// Constructor for the MonteCarloNode class
        MonteCarloNode(CodeIR *CodeIr, int stage, MonteCarloNode * parent = nullptr, int level = 0, int index = 0);
        /// Constructor with parameters for the transformation list, code IR, and current transformation
        MonteCarloNode(std::vector<Transformation*> TransformationList,CodeIR* CodeIr,Transformation* TransformationApplied, MonteCarloNode * parent = nullptr, int level = 0, int index = 0);
        /// returns the best node from the children list based on the upper confidence bound 
        MonteCarloNode * UctSelect(double explorationConstant);
        /// getter for the visit cound attribute
        int getVisitCount();
        /// setter for the visit count attribute
        void incrementVisitCount();
        /// Getter for the best value attribute
        double getBestValue();
        /// Setter for the best value attribute
        void setBestValue(double value);
        /// Getter for the fullyExpanded attribute
        bool getIsFullyExpanded();
        /// Setter for the FullyExpanded attribute
        void setIsFullyExpanded(bool value);
        ///Getter for the parent attribute
        MonteCarloNode * getParent();
        /// Getter for the level attribute
        int getLevel();
        void clearChildren();
        /// Setter for the level attribute
        void setLevel(int level);
        void setParent(MonteCarloNode* parent);
        int getIndex();
        void setIndex(int index);
        MonteCarloNode * bestChild();
        MonteCarloNode* epsilonGreedySelect(double epsilon, std::mt19937 gen);
        // SmallVector<Node*, 2>& getChildrenNodes();
        void deleteChild(MonteCarloNode* child);

};



class MonteCarlo:public SearchMethod{
    private:
        /// a float that defines the exploration rate during the search
        double exploration;
        /// the root node for which we start the search

        mlir::MLIRContext *context;
        std::string functionName;

    public:
        /// Constructor for the BeamSearch class, initializing beam size, MLIR context, and the function name.
        MonteCarlo(mlir::MLIRContext *context, std::string functionName, double exploration = std::sqrt(2));
        /// Selects and returns the best child node from the children list based on the upper confidence bound
        MonteCarloNode * select(MonteCarloNode* node, std::mt19937 gen);
        /// Creates children nodes for the current node
        SmallVector<MonteCarloNode *, 2> expand(MonteCarloNode * node, int level,int stage,  std::unordered_map<std::string, std::pair<mlir::linalg::LinalgOp, LinalgMappingClassification>> LinalgOpStages, std::mt19937 random_engine);
        ///simulates the current node performance (in our case it evaluates it)
        double simulate(MonteCarloNode* node, EvaluationByExecution evaluator);
        ///updates the current node parents performance based on its performance
        void backpropagate(MonteCarloNode* node, double result);
        /// Runs the monte carlo search algorithm starting from a given root node
        Node * runSearchMethod(Node * root, EvaluationByExecution evaluator, std::unordered_map<std::string, std::pair<mlir::linalg::LinalgOp, LinalgMappingClassification>> LinalgOpStages, int iterations = 100);
        Node * runSearchMethod(Node * root);

};

#endif // MLSCEDULER_BEAM_SEARCH_H_