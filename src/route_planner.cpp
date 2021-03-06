#include "route_planner.h"
#include <algorithm>


RoutePlanner::RoutePlanner(RouteModel &model, float start_x, float start_y, float end_x, float end_y): m_Model(model) {
    // Convert inputs to percentage:

    start_x *= 0.01;
    start_y *= 0.01;
    end_x *= 0.01;
    end_y *= 0.01;   
    // TODO 2: Use the m_Model.FindClosestNode method to find the closest nodes to the starting and ending coordinates.
    // Store the nodes you find in the RoutePlanner's start_node and end_node attributes.
    start_node = &m_Model.FindClosestNode(start_x, start_y);
    end_node = &m_Model.FindClosestNode(end_x, end_y);

}

std::vector<RouteModel::Node> RoutePlanner::ConstructFinalPath(RouteModel::Node * current_node){
    std::vector <RouteModel:: Node> path_found;
    distance = 0.0f;
    RouteModel::Node parent;
    
    while (current_node->parent!=nullptr)
    {
        path_found.push_back(*current_node);
        parent = *(current_node->parent);
        distance += current_node->distance(parent);
        current_node = current_node->parent;
    }

    path_found.push_back(*current_node);
    this->distance *= m_Model.MetricScale();

    return path_found;
}

void RoutePlanner::AStarSearch()
{
    // end_node->parent = start_node;

    start_node->visited = true;
    open_list.push_back(start_node);

    RouteModel::Node *current_node = nullptr;

    while(open_list.size()>0)
    {
        current_node = NextNode();
        if (current_node->distance(*end_node)==0)
        {
            m_Model.path = ConstructFinalPath(current_node);
            return;
        }
  
        AddNeighbors(current_node);

    }

    
}


float RoutePlanner::CalculateHValue(RouteModel::Node *node)
{

    return node->distance(*end_node);

}


// bool RoutePlanner::Compare(RouteModel::Node *a, RouteModel::Node *b)
// {

//     return (a->g_value+a->h_value)>(b->g_value+b->h_value);
// };


RouteModel::Node *RoutePlanner::NextNode(){

    std::sort(open_list.begin(), open_list.end(), [](const auto &a, const auto &b){
        return (a->g_value+a->h_value)<(b->g_value+b->h_value);
    });

    // std::sort(open_list.begin(), open_list.end(), Compare);

    RouteModel::Node *min_dist_node = open_list.front();

    open_list.erase(open_list.begin());

    return min_dist_node;

}


void RoutePlanner::AddNeighbors(RouteModel::Node *current_node){

    current_node->FindNeighbors();

    for (auto neighbor : current_node->neighbors)
    {
        neighbor->parent = current_node;
        neighbor->g_value = current_node->distance(*neighbor) + current_node->g_value;
        neighbor->h_value = CalculateHValue(neighbor);
        open_list.push_back(neighbor);
        neighbor->visited = true;

    }


}