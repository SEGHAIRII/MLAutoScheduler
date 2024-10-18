//===------------------------- MonteCarloSearch.cpp - Monte Carlo Tree Search  ----------------===//
//
///===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the implmentation of the MonteCarlo class, which contains
///  the implmentation of the Monte Carlo search method
///
//===----------------------------------------------------------------------===//

#include "MonteCarloSearch.h"




MonteCarloNode::MonteCarloNode(CodeIR *CodeIr, int stage, MonteCarloNode * parent, int level, int index): Node(CodeIr, stage), visitCount(0), level(level),
                                                                                    isFullyExpanded(false), bestValue(0), index(index) {
    
    this->parent = parent;
}


MonteCarloNode::MonteCarloNode(std::vector<Transformation*> TransformationList,CodeIR* CodeIr,
                 Transformation* TransformationApplied, MonteCarloNode * parent, int level, int index): Node(TransformationList,CodeIr,
                 TransformationApplied),level(level) ,visitCount(0), isFullyExpanded(false), bestValue(0), index(index) {

    this->parent = parent;
}

int MonteCarloNode::getVisitCount(){
    return this->visitCount;
}


void MonteCarloNode::incrementVisitCount(){
    this->visitCount++;
}


double MonteCarloNode::getBestValue(){
    return this->bestValue;
}


void MonteCarloNode::setBestValue(double value){
    this->bestValue = value;
}

bool MonteCarloNode::getIsFullyExpanded(){
    return this->isFullyExpanded;
}

int MonteCarloNode::getLevel(){
    return this->level;
}

void MonteCarloNode::setLevel(int level){
    this->level = level;
}

void MonteCarloNode::setIsFullyExpanded(bool value){
    this->isFullyExpanded = value;
}

int MonteCarloNode::getIndex(){
    return this->index;
}

void MonteCarloNode::setIndex(int index){
    this->index = index;
}


// SmallVector<MonteCarloNode*, 2> MonteCarloNode::getChildrenNodes(){
//     SmallVector<MonteCarloNode* , 2> new_nodes;
//     SmallVector<Node*, 2> nodes = Node::getChildrenNodes();
//     for (Node * node: nodes){
//         MonteCarloNode *new_node = static_cast<MonteCarloNode*>(node);
//         new_nodes.push_back(new_node);
//     }
//     return new_nodes;
// }

// SmallVector<Node*, 2>& MonteCarloNode::getChildrenNodes(){
    // SmallVector<MonteCarloNode* , 2> new_nodes = Node::getChildrenNodes();;
    // SmallVector<Node*, 2> nodes = Node::getChildrenNodes();
    // for (Node * node: nodes){
        // MonteCarloNode *new_node = static_cast<MonteCarloNode*>(node);
        // new_nodes.push_back(new_node);
    // }
    // return this->ChildrenNodes;
// }


MonteCarloNode * MonteCarloNode::getParent(){
    return this->parent;
}

void MonteCarloNode::deleteChild(MonteCarloNode* child){
    SmallVector<Node*, 2> children = this->getChildrenNodes();
    int index = child->getIndex();
    std::cerr << "it did enter here\n";
    std::cerr << children.size() << ": size of the children\n";
    auto predicate = [index](Node* node) {
        MonteCarloNode* child = static_cast<MonteCarloNode*>(node);
        std::cerr << index << " : is the index of the node we are searching for\n";
        std::cerr << child->getIndex() << " : in the index of the node we are in now\n";
        return (child->getIndex() == index);
    };
    auto it = std::find_if(children.begin(), children.end(), predicate);
    std::cerr << "and pass by here\n";
    if (it != children.end()) {
        std::cerr << "did find it\n";
        children.erase(it);
        this->setChildrenNodes(children);
        std::cerr << children.size() << ": the size\n";
    }
    else {
        std::cerr << "did not find it\n";
    }
}

void MonteCarloNode::setParent(MonteCarloNode* parent){
    this->parent = parent;
}

void MonteCarloNode::clearChildren(){
    SmallVector<Node*, 2> new_vect = this->getChildrenNodes();
    new_vect.clear();
    this->setChildrenNodes(new_vect);
}

MonteCarloNode * MonteCarloNode::UctSelect(double exploration_cte){
        MonteCarloNode* bestNode = nullptr;
        double bestUCTValue = std::numeric_limits<double>::infinity();
        SmallVector<Node*, 2> children = this->getChildrenNodes();
        std::cerr << children.size() << " : size of the children\n";
        for (Node* node : children) {
            MonteCarloNode* child = static_cast<MonteCarloNode*>(node);
            if (child->getVisitCount() == 0){
                auto it = std::find(children.begin(), children.end(), child);
                // Get the index using std::distance
                if (it != children.end()) {
                size_t index = std::distance(children.begin(), it);
                std::cerr << index << " the index of the chosen child\n";
                }
                return child;
            }
            else{
                double exploitation = child->getBestValue() / child->getVisitCount();
                double exploration = std::sqrt(std::log(this->getVisitCount()) / child->getVisitCount());
                double uctValue = exploitation - exploration_cte * exploration;
                if (uctValue < bestUCTValue) {
                    bestUCTValue = uctValue;
                    bestNode = child;
                }
            }
        }
        auto it = std::find(children.begin(), children.end(), bestNode);
        // Get the index using std::distance
        if (it != children.end()) {
            size_t index = std::distance(children.begin(), it);
            std::cerr << index << " the index of the chosen child\n";
        }
        return bestNode;
}

// was used only to test different configurations
MonteCarloNode* MonteCarloNode::epsilonGreedySelect(double epsilon, std::mt19937 gen) {
    std::uniform_real_distribution<> dis(0.0, 1.0);
    if ((dis(gen)) < epsilon) {
        // Explore: choose a random child
        // SmallVector<Node*, 2> children = this->getChildrenNodes();
        // std::uniform_int_distribution<> childDis(0, children.size() - 1);
        // return static_cast<MonteCarloNode*>(childDis(gen));
    } else {
        // Exploit: choose the best child
        return this->bestChild(); // 0 exploration probability for pure exploitation
    }
}


MonteCarloNode * MonteCarloNode::bestChild(){
    MonteCarloNode* bestNode;
    double bestValue = std::numeric_limits<double>::infinity();
    SmallVector<Node*, 2> children = this->getChildrenNodes();
    for (Node* node:children){
        MonteCarloNode* child = static_cast<MonteCarloNode*>(node);
        if(child->getVisitCount() == 0);
        else{
            double value = child->getBestValue() / child->getVisitCount();
            if (value < bestValue) {
                bestValue = value;
                bestNode = child;
            }
        }
    }  
    auto it = std::find(children.begin(), children.end(), bestNode);

// Get the index using std::distance
    if (it != children.end()) {
        size_t index = std::distance(children.begin(), it);
        std::cerr << index << " the index of the chosen child\n";
    }
    return bestNode;
}



MonteCarlo::MonteCarlo(mlir::MLIRContext *context, std::string functionName, double exploration):exploration(exploration)
{
    this->context = context;
    this->functionName = functionName;
}



MonteCarloNode * MonteCarlo::select(MonteCarloNode* node, std::mt19937 gen){
    MonteCarloNode* selectedNode = node;
    while (selectedNode->getIsFullyExpanded() && selectedNode->getLevel() < 2 && selectedNode->getChildrenNodes().size() > 0) {
            selectedNode = selectedNode->UctSelect(this->exploration);
        }
        return selectedNode;
}


SmallVector<MonteCarloNode *, 2> MonteCarlo::expand(MonteCarloNode * node, int level, int stage, std::unordered_map<std::string, std::pair<mlir::linalg::LinalgOp, LinalgMappingClassification>> LinalgOpStages,  std::mt19937 random_engine){
    SmallVector<MonteCarloNode *, 2> children;
    SmallVector<Node *, 2> candidates;
    SmallVector<Node *, 2> sampled_candidates;
    std::cerr << "level   : ";
    std::cerr << level << "         stage: " << stage << std::endl;
    /* generating the children for each transformation based on the level of the tree */ 
    if(!node->getIsFullyExpanded() && level <= 1){
        switch (level){
                // case 0:
                    // candidates = Tiling::createTilingCandidates(node, this->context, stage, LinalgOpStages);
                    // break;
                case 0:
                    candidates = Parallelization::createParallelizationCandidates(node, this->context, stage, LinalgOpStages);
                    break;
                // case 1:
                    // candidates = Interchange::createInterchangeCandidates(node, this->context);
                    // break;
                case 1:
                    candidates = Vectorization::createVectorizationCandidates(node, this->context);
                    break; 
        }
        //Random sample child nodes
         if(candidates.size() > 100){
            sampled_candidates.resize(100);
            std::sample(candidates.begin(),
                    candidates.end(), 
                    sampled_candidates.begin(), 
                    100, 
                    random_engine);
            candidates = sampled_candidates;
        }
        int index = 0;
        for (Node * candidate:candidates){
            CodeIR* codeir = candidate->getTransformedCodeIr();
            std::vector<Transformation*> TransformationList = candidate->getTransformationList();
            Transformation* TransformationApplied = candidate->getTransformation();
            MonteCarloNode *child = new MonteCarloNode(TransformationList, codeir, TransformationApplied, node, level + 1, index);
            children.push_back(child);
            node->createChild(child);
            index++;
        }
        node->setIsFullyExpanded(true);
        std::cerr << children.size() << ": size of the children\n";
        return children;
    }
    else{
        node->setIsFullyExpanded(true);
        return SmallVector<MonteCarloNode *, 2>();
    }
}


double MonteCarlo::simulate(MonteCarloNode* node, EvaluationByExecution evaluator){
    std::string evaluation = evaluator.evaluateTransformation(node);
    node->setEvaluation(evaluation);
    std::cerr << " -------evaluation : ";
    std::cerr << node->getEvaluation() << std::endl;
    return std::stod(evaluation);
}


void MonteCarlo::backpropagate(MonteCarloNode* node, double result){
    if (result == 9000000000000000000){
        std::cerr << "its in error\n";
        auto parent = node->getParent();
        while(parent != nullptr){
            parent->deleteChild(node);
            delete node;
            if(parent->getChildrenNodes().size() > 0){
                std::cerr << parent->getChildrenNodes().size() << " : the size of children\n";
                std::cerr << "this hasnt become a leaf node\n";
                break;
            }
            else{ 
                std::cerr << "this has  become a leaf node\n";
                node = parent;
                parent = node->getParent();
            }
        }
    }
    else{
        std::cerr << "its in correct\n";
        while (node != nullptr) {
            node->incrementVisitCount();
            node->setBestValue(node->getBestValue() + result);
            // node->setBestValue(std::min(node->getBestValue(), result));   
            node = node->getParent();
        }
    }
    
}

Node * MonteCarlo::runSearchMethod(Node * root){
    return root;
}

 Node * MonteCarlo::runSearchMethod(Node * root, EvaluationByExecution evaluator, std::unordered_map<std::string, std::pair<mlir::linalg::LinalgOp, LinalgMappingClassification>> LinalgOpStages, int iterations) {
    CodeIR* codeir = root->getTransformedCodeIr();
    //getting the current linalg operation in the mlir code
    int stage = root->getCurrentStage();
    // level lets us know which transformation im currently applying
    int level = 0;
    //transforming a Node to MonteCarloNode class
    MonteCarloNode* rootNode = new MonteCarloNode(codeir, stage);
    MonteCarloNode* selectedNode;
    SmallVector<MonteCarloNode*, 2> children;
    //initialize the seed for epsilon greedy algorithm
    std::random_device rd;
    std::mt19937 gen(rd());
    for(int i = 0; i < iterations; i++){
        std::cerr << "################################## ITERATION ";
        std::cerr << i << "########################################################" << std::endl;
        selectedNode = this->select(rootNode, gen); 
        std::cerr << "-------------SELECTION DONE----------------" << std::endl; 
        level = selectedNode->getLevel();
        if(level <= 1){
            children = this->expand(selectedNode, level, stage, LinalgOpStages, gen);
        }
        std::cerr << "------------EXPANSION DONE------------" << std::endl; 
        double simulationResult = this->simulate(selectedNode, evaluator);
        std::cerr << "--------------SIMULATION DONE--------------" << std::endl; 
        this->backpropagate(selectedNode, simulationResult);
        std::cerr << "-----------------BACKPROPAGATION DONE-----------------" << std::endl; 
    }
    MonteCarloNode* result = rootNode;
    selectedNode = rootNode;
    while(selectedNode->getChildrenNodes().size() > 0){
        selectedNode = selectedNode->bestChild();
        std::cerr << " -------evaluation : ";
        std::cerr << selectedNode->getEvaluation() << std::endl;
        if (std::stod(selectedNode->getEvaluation()) < std::stod(result->getEvaluation())){
            result = selectedNode;
        }
    }
    result->setParent(nullptr);
    result->clearChildren();
    std::cerr << "result evaluation : ";
    std::cerr << result->getEvaluation() << std::endl;
    return static_cast<Node *>(result);
}
