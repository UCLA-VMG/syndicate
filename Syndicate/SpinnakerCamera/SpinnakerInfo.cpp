#include "SpinnakerInfo.h"


// Define the maximum number of characters that will be printed out
// for any information retrieved from a node.
const unsigned int maxChars = 35;

using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;

int PrintEnumerationSelector(Spinnaker::GenApi::CNodePtr node, unsigned int level);


void Indent(unsigned int level)
{
    for (unsigned int i = 0; i < level; i++)
    {
        std::cout << "   ";
    }
}


int PrintValueNode(Spinnaker::GenApi::CNodePtr node, unsigned int level)
{
    int result = 0;

    try
    {
        // If this node is a selector and is an enumeration node print out its entries and selected features
        if (node->IsSelector() && (node->GetPrincipalInterfaceType() == intfIEnumeration))
        {
            return PrintEnumerationSelector(node, level);
        }

        // Cast as value node
        CValuePtr ptrValueNode = static_cast<CValuePtr>(node);

        //
        // Retrieve display name
        //
        // *** NOTES ***
        // A node's 'display name' is generally more appropriate for output and
        // user interaction whereas its 'name' is what the camera understands.
        // Generally, its name is the same as its display name but without
        // spaces - for instance, the name of the node that houses a camera's
        // serial number is 'DeviceSerialNumber' while its display name is
        // 'Device Serial Number'.
        //
        gcstring displayName = ptrValueNode->GetDisplayName();

        //
        // Retrieve value of any node type as string
        //
        // *** NOTES ***
        // Because value nodes return any node type as a string, it can be much
        // easier to deal with nodes as value nodes rather than their actual
        // individual types.
        //
        gcstring value = ptrValueNode->ToString();

        // Ensure that the value length is not excessive for printing
        if (value.size() > maxChars)
        {
            value = value.substr(0, maxChars) + "...";
        }

        // Print value
        Indent(level);
        std::cout << displayName << ": " << value << std::endl;
    }
    catch (Spinnaker::Exception& e)
    {
        std::cout << "Error: " << e.what() << std::endl;
        result = -1;
    }

    return result;
}

// This function retrieves and prints the display name and value of a string
// node, limiting the number of printed characters to a maximum defined by the
// maxChars global variable. Level parameter determines the indentation level
// for the output.
int PrintStringNode(Spinnaker::GenApi::CNodePtr node, unsigned int level)
{
    int result = 0;

    try
    {
        // Cast as string node
        CStringPtr ptrStringNode = static_cast<CStringPtr>(node);

        // Retrieve display name
        gcstring displayName = ptrStringNode->GetDisplayName();

        //
        // Retrieve string node value
        //
        // *** NOTES ***
        // The Spinnaker SDK includes its own wrapped string class, gcstring. As
        // such, there is no need to import the 'string' library when using the
        // SDK. If a standard string object is preferred, simply use a c-style
        // or static cast on the gcstring object.
        //
        gcstring value = ptrStringNode->GetValue();

        // Ensure that the value length is not excessive for printing
        if (value.size() > maxChars)
        {
            value = value.substr(0, maxChars) + "...";
        }

        // Print value; 'level' determines the indentation level of output
        Indent(level);
        std::cout << displayName << ": " << value << std::endl;
    }
    catch (Spinnaker::Exception& e)
    {
        std::cout << "Error: " << e.what() << std::endl;
        result = -1;
    }

    return result;
}

// This function retrieves and prints the display name and value of an integer
// node.
int PrintIntegerNode(Spinnaker::GenApi::CNodePtr node, unsigned int level)
{
    int result = 0;

    try
    {
        // Cast node as integer node
        CIntegerPtr ptrIntegerNode = static_cast<CIntegerPtr>(node);

        // Retrieve display name
        gcstring displayName = ptrIntegerNode->GetDisplayName();

        //
        // Retrieve integer node value
        //
        // *** NOTES ***
        // Keep in mind that the data type of an integer node value is an
        // int64_t as opposed to a standard int. While it is true that the two
        // are often interchangeable, it is recommended to use the int64_t
        // to avoid the introduction of bugs.
        //
        // All node types except for base and port nodes include a handy
        // ToString() method which returns a value as a gcstring.
        //
        int64_t value = ptrIntegerNode->GetValue();

        // Print value
        Indent(level);
        std::cout << displayName << ": " << value << std::endl;
    }
    catch (Spinnaker::Exception& e)
    {
        std::cout << "Error: " << e.what() << std::endl;
        result = -1;
    }

    return result;
}

// This function retrieves and prints the display name and value of a float
// node.
int PrintFloatNode(Spinnaker::GenApi::CNodePtr node, unsigned int level)
{
    int result = 0;

    try
    {
        // Cast as float node
        CFloatPtr ptrFloatNode = static_cast<CFloatPtr>(node);

        // Retrieve display name
        gcstring displayName = ptrFloatNode->GetDisplayName();

        //
        // Retrieve float node value
        //
        // *** NOTES ***
        // Please take note that floating point numbers in the Spinnaker SDK are
        // almost always represented by the larger data type double rather than
        // float.
        //
        double value = ptrFloatNode->GetValue();

        // Print value
        Indent(level);
        std::cout << displayName << ": " << value << std::endl;
    }
    catch (Spinnaker::Exception& e)
    {
        std::cout << "Error: " << e.what() << std::endl;
        result = -1;
    }

    return result;
}

// This function retrieves and prints the display name and value of a boolean,
// printing "true" for true and "false" for false rather than the corresponding
// integer value ('1' and '0', respectively).
int PrintBooleanNode(Spinnaker::GenApi::CNodePtr node, unsigned int level)
{
    int result = 0;

    try
    {
        // Cast as boolean node
        CBooleanPtr ptrBooleanNode = static_cast<CBooleanPtr>(node);

        // Retrieve display name
        gcstring displayName = ptrBooleanNode->GetDisplayName();

        //
        // Retrieve value as a string representation
        //
        // *** NOTES ***
        // Boolean node type values are represented by the standard bool data
        // type. The boolean ToString() method returns either a '1' or '0' as
        // a string rather than a more descriptive word like 'true' or 'false'.
        //
        gcstring value = (ptrBooleanNode->GetValue() ? "true" : "false");

        // Print value
        Indent(level);
        std::cout << displayName << ": " << value << std::endl;
    }
    catch (Spinnaker::Exception& e)
    {
        std::cout << "Error: " << e.what() << std::endl;
        result = -1;
    }

    return result;
}

// This function retrieves and prints the display name and tooltip of a command
// node, limiting the number of printed characters to a defined maximum.
// The tooltip is printed below because command nodes do not have an intelligible
// value.
int PrintCommandNode(Spinnaker::GenApi::CNodePtr node, unsigned int level)
{
    int result = 0;

    try
    {
        // Cast as command node
        CCommandPtr ptrCommandNode = static_cast<CCommandPtr>(node);

        // Retrieve display name
        gcstring displayName = ptrCommandNode->GetDisplayName();

        //
        // Retrieve tooltip
        //
        // *** NOTES ***
        // All node types have a tooltip available. Tooltips provide useful
        // information about nodes. Command nodes do not have a method to
        // retrieve values as their is no intelligible value to retrieve.
        //
        gcstring tooltip = ptrCommandNode->GetToolTip();

        // Ensure that the value length is not excessive for printing
        if (tooltip.size() > maxChars)
        {
            tooltip = tooltip.substr(0, maxChars) + "...";
        }

        // Print tooltip
        Indent(level);
        std::cout << displayName << ": " << tooltip << std::endl;
    }
    catch (Spinnaker::Exception& e)
    {
        std::cout << "Error: " << e.what() << std::endl;
        result = -1;
    }

    return result;
}

// This function retrieves and prints the display names of an enumeration node
// and its current entry (which is actually housed in another node unto itself).
int PrintEnumerationNodeAndCurrentEntry(Spinnaker::GenApi::CNodePtr node, unsigned int level)
{
    int result = 0;

    try
    {
        // If this enumeration node is a selector, cycle through its entries and selected features
        if (node->IsSelector())
        {
            return PrintEnumerationSelector(node, level);
        }

        // Cast as enumeration node
        CEnumerationPtr ptrEnumerationNode = static_cast<CEnumerationPtr>(node);

        //
        // Retrieve current entry as enumeration node
        //
        // *** NOTES ***
        // Enumeration nodes have three methods to differentiate between: first,
        // GetIntValue() returns the integer value of the current entry node;
        // second, GetCurrentEntry() returns the entry node itself; and third,
        // ToString() returns the symbolic of the current entry.
        //
        CEnumEntryPtr ptrEnumEntryNode = ptrEnumerationNode->GetCurrentEntry();

        // Retrieve display name
        gcstring displayName = ptrEnumerationNode->GetDisplayName();

        //
        // Retrieve current symbolic
        //
        // *** NOTES ***
        // Rather than retrieving the current entry node and then retrieving its
        // symbolic, this could have been taken care of in one step by using the
        // enumeration node's ToString() method.
        //
        gcstring currentEntrySymbolic = ptrEnumEntryNode->GetSymbolic();

        // Print current entry symbolic
        Indent(level);
        std::cout << displayName << ": " << currentEntrySymbolic << std::endl;
    }
    catch (Spinnaker::Exception& e)
    {
        std::cout << "Error: " << e.what() << std::endl;
        result = -1;
    }

    return result;
}

// Based on the read type specified, print the node using the correct print function.
int PrintNode(Spinnaker::GenApi::CNodePtr node, unsigned int level)
{
    switch (chosenRead)
    {
    case VALUE:
    {
        return PrintValueNode(node, level);
    }
    case INDIVIDUAL: // Cast all non-category nodes as actual types
    {
        switch (node->GetPrincipalInterfaceType())
        {
        case intfIString:
        {
            return PrintStringNode(node, level);
        }
        case intfIInteger:
        {
            return PrintIntegerNode(node, level);
        }
        case intfIFloat:
        {
            return PrintFloatNode(node, level);
        }
        case intfIBoolean:
        {
            return PrintBooleanNode(node, level);
        }
        case intfICommand:
        {
            return PrintCommandNode(node, level);
        }
        case intfIEnumeration:
        {
            return PrintEnumerationNodeAndCurrentEntry(node, level);
        }
        default:
        {
            std::cout << "Unexpected interface type." << std::endl;
            return -1;
        }
        }
    }
    default:
    {
        std::cout << "Unexpected read type." << std::endl;
        return -1;
    }
    }
}

// This function retrieves and prints the display names of enumeration selector nodes.
// The selector will cycle through every selector entry and print out all the selected
// features for that selector entry. It is possible for integer nodes to be selector
// nodes as well, but this function will only cycle through Enumeration nodes.
int PrintEnumerationSelector(Spinnaker::GenApi::CNodePtr node, unsigned int level)
{
    int result = 0;

    try
    {
        FeatureList_t selectedFeatures;
        node->GetSelectedFeatures(selectedFeatures);

        // Cast as an enumeration node
        CEnumerationPtr ptrSelectorNode = static_cast<CEnumerationPtr>(node);

        StringList_t entries;
        ptrSelectorNode->GetSymbolics(entries);

        // Note current selector node entry
        CEnumEntryPtr ptrCurrentEntry = ptrSelectorNode->GetCurrentEntry();

        // Retrieve display name
        gcstring displayName = ptrSelectorNode->GetDisplayName();

        // Retrieve current symbolic
        gcstring currentEntrySymbolic = ptrSelectorNode->ToString();

        // Print current entry symbolic
        Indent(level);
        std::cout << displayName << ": " << currentEntrySymbolic << std::endl;

        // For every selector node entry
        for (size_t i = 0; i < entries.size(); i++)
        {
            CEnumEntryPtr selectorEntry = ptrSelectorNode->GetEntryByName(entries[i]);
            FeatureList_t::const_iterator it;

            // Go through each enum entry of the selector node
            if (IsWritable(ptrSelectorNode))
            {
                if (IsAvailable(selectorEntry) && IsReadable(selectorEntry))
                {
                    ptrSelectorNode->SetIntValue(selectorEntry->GetValue());
                    Indent(level + 1);
                    std::cout << displayName << ": " << ptrSelectorNode->ToString() << std::endl;
                }
            }

            // Look at every node that is affected by the selector node
            for (it = selectedFeatures.begin(); it != selectedFeatures.end(); ++it)
            {
                Spinnaker::GenApi::CNodePtr ptrFeatureNode = *it;

                if (!IsAvailable(ptrFeatureNode) || !IsReadable(ptrFeatureNode))
                {
                    continue;
                }
                // Print the selected feature
                else
                {
                    result = result | PrintNode(ptrFeatureNode, level + 2);
                }
            }
        }

        // Restore the selector to its original value
        if (IsWritable(ptrSelectorNode))
        {
            ptrSelectorNode->SetIntValue(ptrCurrentEntry->GetValue());
        }
    }
    catch (Spinnaker::Exception& e)
    {
        std::cout << "Error: " << e.what() << std::endl;
        result = -1;
    }

    return result;
}

// This function retrieves and prints out the display name of a category node
// before printing all child nodes. Child nodes that are also category nodes are
// printed recursively.
int PrintCategoryNodeAndAllFeatures(Spinnaker::GenApi::CNodePtr node, unsigned int level)
{
    int result = 0;

    try
    {
        // Cast as category node
        CCategoryPtr ptrCategoryNode = static_cast<CCategoryPtr>(node);

        // Retrieve display name
        gcstring displayName = ptrCategoryNode->GetDisplayName();

        // Print display name
        Indent(level);
        std::cout << displayName << std::endl;

        //
        // Retrieve children
        //
        // *** NOTES ***
        // The two nodes that typically have children are category nodes and
        // enumeration nodes. Throughout the examples, the children of category
        // nodes are referred to as features while the children of enumeration
        // nodes are referred to as entries. Keep in mind that enumeration
        // nodes can be cast as category nodes, but category nodes cannot be
        // cast as enumerations.
        //
        FeatureList_t features;
        ptrCategoryNode->GetFeatures(features);

        //
        // Iterate through all children
        //
        // *** NOTES ***
        // If dealing with a variety of node types and their values, it may be
        // simpler to cast them as value nodes rather than as their individual
        // types. However, with this increased ease-of-use, functionality is
        // sacrificed.
        //
        FeatureList_t::const_iterator it;
        for (it = features.begin(); it != features.end(); ++it)
        {
            Spinnaker::GenApi::CNodePtr ptrFeatureNode = *it;

            // Ensure node is available and readable
            if (!IsAvailable(ptrFeatureNode) || !IsReadable(ptrFeatureNode))
            {
                continue;
            }

            // Category nodes must be dealt with separately in order to
            // retrieve subnodes recursively.
            if (ptrFeatureNode->GetPrincipalInterfaceType() == intfICategory)
            {
                result = result | PrintCategoryNodeAndAllFeatures(ptrFeatureNode, level + 1);
            }
            // Print the node
            else
            {
                result = result | PrintNode(ptrFeatureNode, level + 1);
            }
        }
        std::cout << std::endl;
    }
    catch (Spinnaker::Exception& e)
    {
        std::cout << "Error: " << e.what() << std::endl;
        result = -1;
    }

    return result;
}

int RunSingleCamera(Spinnaker::CameraPtr cam)
{
    int result = 0;
    unsigned int level = 0;

    try
    {
        //
        // Retrieve TL device nodemap
        //
        // *** NOTES ***
        // The TL device nodemap is available on the transport layer. As such,
        // camera initialization is unnecessary. It provides mostly immutable
        // information fundamental to the camera such as the serial number,
        // vendor, and model.
        //
        std::cout << std::endl << "*** PRINTING TRANSPORT LAYER DEVICE NODEMAP ***" << std::endl << std::endl;

        Spinnaker::GenApi::INodeMap& genTLNodeMap = cam->GetTLDeviceNodeMap();

        result = PrintCategoryNodeAndAllFeatures(genTLNodeMap.GetNode("Root"), level);

        //
        // Retrieve TL stream nodemap
        //
        // *** NOTES ***
        // The TL stream nodemap is also available on the transport layer. Camera
        // initialization is again unnecessary. As you can probably guess, it
        // provides information on the camera's streaming performance at any
        // given moment. Having this information available on the transport
        // layer allows the information to be retrieved without affecting camera
        // performance.
        //
        std::cout << "*** PRINTING TL STREAM NODEMAP ***" << std::endl << std::endl;

        Spinnaker::GenApi::INodeMap& nodeMapTLStream = cam->GetTLStreamNodeMap();

        result = result | PrintCategoryNodeAndAllFeatures(nodeMapTLStream.GetNode("Root"), level);

        //
        // Initialize camera
        //
        // *** NOTES ***
        // The camera becomes connected upon initialization. This provides
        // access to configurable options and additional information, accessible
        // through the GenICam nodemap.
        //
        // *** LATER ***
        // Cameras should be deinitialized when no longer needed.
        //
        std::cout << "*** PRINTING GENICAM NODEMAP ***" << std::endl << std::endl;

        cam->Init();

        //
        // Retrieve GenICam nodemap
        //
        // *** NOTES ***
        // The GenICam nodemap is the primary gateway to customizing
        // and configuring the camera to suit your needs. Configuration options
        // such as image height and width, trigger mode enabling and disabling,
        // and the sequencer are found on this nodemap.
        //
        Spinnaker::GenApi::INodeMap& appLayerNodeMap = cam->GetNodeMap();

        result = result | PrintCategoryNodeAndAllFeatures(appLayerNodeMap.GetNode("Root"), level);

        //
        // Deinitialize camera
        //
        // *** NOTES ***
        // Camera deinitialization helps ensure that devices clean up properly
        // and do not need to be power-cycled to maintain integrity.
        //
        cam->DeInit();
    }
    catch (Spinnaker::Exception& e)
    {
        std::cout << "Error: " << e.what() << std::endl;
        result = -1;
    }

    return result;
}