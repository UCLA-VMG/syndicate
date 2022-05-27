#pragma once

#include "Spinnaker.h"
#include "SpinGenApi/SpinnakerGenApi.h"
#include <iostream>
#include <sstream>


// Use the following enum and global constant to select whether nodes are read
// as 'value' nodes or their individual types.
enum readType
{
    VALUE,
    INDIVIDUAL
};
const readType chosenRead = VALUE;

/**
 * @brief 
 * 
 * @param level 
 */
void Indent(unsigned int level);

/**
 * @brief This function retrieves and prints the display name and value of all node
 * types as value nodes. A value node is a general node type that allows for
 * the reading and writing of any node type as a string.
 * 
 * @param node 
 * @param level 
 * @return int 
 */
int PrintValueNode(Spinnaker::GenApi::CNodePtr node, unsigned int level);

int PrintStringNode(Spinnaker::GenApi::CNodePtr node, unsigned int level);

int PrintStringNode(Spinnaker::GenApi::CNodePtr node, unsigned int level);

int PrintFloatNode(Spinnaker::GenApi::CNodePtr node, unsigned int level);

int PrintFloatNode(Spinnaker::GenApi::CNodePtr node, unsigned int level);

int PrintBooleanNode(Spinnaker::GenApi::CNodePtr node, unsigned int level);

int PrintCommandNode(Spinnaker::GenApi::CNodePtr node, unsigned int level);

int PrintEnumerationNodeAndCurrentEntry(Spinnaker::GenApi::CNodePtr node, unsigned int level);

int PrintNode(Spinnaker::GenApi::CNodePtr node, unsigned int level);

int PrintEnumerationSelector(Spinnaker::GenApi::CNodePtr node, unsigned int level);

int PrintCategoryNodeAndAllFeatures(Spinnaker::GenApi::CNodePtr node, unsigned int level);

int RunSingleCamera(Spinnaker::CameraPtr cam);

