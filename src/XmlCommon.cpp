//
//
//****************************************************************************************//
// Copyright (c) 2002-2009, The MITRE Corporation
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright notice, this list
//       of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright notice, this 
//       list of conditions and the following disclaimer in the documentation and/or other
//       materials provided with the distribution.
//     * Neither the name of The MITRE Corporation nor the names of its contributors may be
//       used to endorse or promote products derived from this software without specific 
//       prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
// SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
// OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
// TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//****************************************************************************************//

#include "XmlCommon.h"

//****************************************************************************************//
//										XmlCommon Class									  //	
//****************************************************************************************//
void XmlCommon::AddAttribute(DOMElement *node, string attName, string attValue) {

	const XMLCh *name = XMLString::transcode(attName.c_str());
	const XMLCh *value = XMLString::transcode(attValue.c_str());
	node->setAttribute(name, value);

}

DOMElement* XmlCommon::AddChildElement(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc, DOMElement *parent, string nodeName, string nodeValue) {

	DOMElement *newElem = NULL;

	newElem = CreateElement(doc, nodeName, nodeValue);
	parent->appendChild(newElem);

	return newElem;
}

void XmlCommon::CopyNamespaces(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* source, XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* dest) {

	DOMNamedNodeMap* attributes = source->getDocumentElement()->getAttributes();
	for(unsigned int i = 0; i < attributes->getLength(); i++) {
		DOMNode* attr = attributes->item(i);
		string name = XmlCommon::ToString(attr->getNodeName());
		string localPart = XmlCommon::ToString(attr->getLocalName());
		string prefix = XmlCommon::ToString(attr->getPrefix());
		if(localPart.compare("xmlns") != 0 && prefix.compare("xmlns") == 0) {
            string value = XmlCommon::ToString(attr->getNodeValue());
			XmlCommon::AddXmlns(dest, value, localPart);
		}		
	}
}

void XmlCommon::CopySchemaLocation(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* source, XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* dest) {

	string srcSchemaLocations = XmlCommon::GetAttributeByName(source->getDocumentElement(), "xsi:schemaLocation");

	// break the schema location values into sepearte strings
	// and add each of them to the destionation document
	int len =srcSchemaLocations.length();
	string currentSchemaLocation = "";
	string suffix = "";
	for(int i = 0; i < len; i++) {
		char c = srcSchemaLocations[i];
		if(c == '.') {
			suffix = suffix + c;
			currentSchemaLocation = currentSchemaLocation + c;
		} else if(c == 'x' && suffix.compare(".") == 0) {
			suffix = suffix + c;
			currentSchemaLocation = currentSchemaLocation + c;
		} else if(c == 's' && suffix.compare(".x") == 0) {
			suffix = suffix + c;
			currentSchemaLocation = currentSchemaLocation + c;
		} else if(c == 'd' && suffix.compare(".xs") == 0) {
			suffix = "";
			currentSchemaLocation = currentSchemaLocation + c;
			XmlCommon::AddSchemaLocation(dest, currentSchemaLocation);
			currentSchemaLocation = "";
		} else if(c == ' ' && currentSchemaLocation.compare("") == 0) {
			suffix = "";
		} else {
			suffix = "";
			currentSchemaLocation = currentSchemaLocation + c;
		}
	}
}

void XmlCommon::RemoveAttributes(DOMElement* elm) {

	DOMNamedNodeMap* attributes = elm->getAttributes();
	unsigned int len = attributes->getLength();
	while(len > 0) {
		DOMAttr* attr = (DOMAttr*)attributes->item(0);
		string ln = XmlCommon::ToString(attr->getLocalName());
		string p = XmlCommon::ToString(attr->getPrefix());
		string v = XmlCommon::ToString(attr->getNodeValue());
		string n = XmlCommon::ToString(attr->getName());

		DOMNode* oldAttr = attributes->removeNamedItem(attr->getName());
		oldAttr->release();
		attributes = elm->getAttributes();
		len = attributes->getLength();
	}
}

DOMElement* XmlCommon::CreateElement(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc, string nodeName, string nodeValue) {
	
	DOMText *tmpTextNode = NULL;
	DOMElement *newElem = NULL;


	newElem = doc->createElement(XMLString::transcode(nodeName.c_str()));
	if(nodeValue.compare("") != 0) {
		tmpTextNode = doc->createTextNode(XMLString::transcode(nodeValue.c_str()));
		newElem->appendChild(tmpTextNode);
	}

	return newElem;
}

ElementVector* XmlCommon::FindAllElements(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc, string nodeName, string attribute, string attValue, string xmlns) {

	ElementVector *nodes	= new ElementVector();
	DOMElement *tmpNode		= NULL;
	DOMNodeList *nodeList	= NULL;
	int listLen				= 0;
	int index				= 0;

	nodeList = doc->getElementsByTagName(XMLString::transcode(nodeName.c_str()));

	listLen = nodeList->getLength();
	while(index < listLen)
	{
		tmpNode = (DOMElement*)nodeList->item(index++);	
		if(tmpNode != NULL)
		{
			//	Check for attribute if desired
			if(attribute.compare("") == 0)
			{
				nodes->push_back(tmpNode);
				continue;
			}else if(tmpNode->hasAttribute(XMLString::transcode(attribute.c_str())))
			{
				//	Check for attribute value if desired
				if(attValue.compare("") == 0)
				{
					nodes->push_back(tmpNode);
					continue;
				}else if((GetAttributeByName(tmpNode, attribute)).compare(attValue) == 0)
				{
					nodes->push_back(tmpNode);
					continue;
				}
			}
		}
	}

	return nodes;
}

DOMElement* XmlCommon::FindElement(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc, string nodeName, string attribute, string attValue) {

	DOMElement *tmpNode		= NULL;
	DOMElement *result			= NULL;
	DOMNodeList *nodeList	= NULL;
	int listLen				= 0;
	int index				= 0;


	//	Get a list of all the nodes in the document with the nodeName and loop through them
	nodeList = doc->getElementsByTagName(XMLString::transcode(nodeName.c_str()));
	listLen = nodeList->getLength();
	while(index < listLen)
	{
		tmpNode = (DOMElement*)nodeList->item(index++);	
		if(tmpNode != NULL)
		{
			//	Check for attribute if desired
			if(attribute.compare("") == 0)
			{
				result = tmpNode;
				break;
			}else if(tmpNode->hasAttribute(XMLString::transcode(attribute.c_str())))
			{
				//	Check for attribute value if desired
				if(attValue.compare("") == 0)
				{
					result = tmpNode;
					break;
				}else if((GetAttributeByName(tmpNode, attribute)).compare(attValue) == 0)
				{
					result = tmpNode;
					break;
				}
			}
		}
	}

	return result;
}

DOMElement* XmlCommon::FindElement(DOMElement *element, string nodeName, string attribute, string attValue) {

	DOMElement *tmpNode		= NULL;
	DOMElement *result			= NULL;
	DOMNodeList *nodeList	= NULL;
	int listLen				= 0;
	int index				= 0;

	nodeList = element->getElementsByTagName(XMLString::transcode(nodeName.c_str()));
	listLen = nodeList->getLength();
	while(index < listLen)
	{
		tmpNode = (DOMElement*)nodeList->item(index++);
		if(tmpNode != NULL)
		{
			//	Check for attribute if desired
			if(attribute.compare("") == 0)
			{
				result = tmpNode;
				break;
			}else if(tmpNode->hasAttribute(XMLString::transcode(attribute.c_str())))
			{
				//	Check for attribute value if desired
				if(attValue.compare("") == 0)
				{
					result = tmpNode;
					break;
	
				}else if((GetAttributeByName(tmpNode, attribute)).compare(attValue) == 0)
				{
					result = tmpNode;
					break;
				}
			}
		}
	}

	return result;
}

DOMElement* XmlCommon::FindElementNS(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc, string nodeName, string attribute, string attValue, string xmlns) {

	DOMElement *tmpNode		= NULL;
	DOMElement *result			= NULL;
	DOMNodeList *nodeList	= NULL;
	int listLen				= 0;
	int index				= 0;


	//	Get a list of all the nodes in the document with the nodeName and loop through them
	nodeList = doc->getElementsByTagNameNS(XMLString::transcode(xmlns.c_str()), XMLString::transcode(nodeName.c_str()));
	listLen = nodeList->getLength();
	while(index < listLen)
	{
		tmpNode = (DOMElement*)nodeList->item(index++);	
		if(tmpNode != NULL)
		{
			//	Check for attribute if desired
			if(attribute.compare("") == 0)
			{
				result = tmpNode;
				break;
			}else if(tmpNode->hasAttribute(XMLString::transcode(attribute.c_str())))
			{
				//	Check for attribute value if desired
				if(attValue.compare("") == 0)
				{
					result = tmpNode;
					break;
				}else if((GetAttributeByName(tmpNode, attribute)).compare(attValue) == 0)
				{
					result = tmpNode;
					break;
				}
			}
		}
	}

	return result;
}

DOMElement* XmlCommon::FindElementNS(DOMElement *element, string nodeName, string attribute, string attValue, string xmlns) {

	DOMElement *tmpNode		= NULL;
	DOMElement *result		= NULL;
	DOMNodeList *nodeList	= NULL;
	int listLen				= 0;
	int index				= 0;


	nodeList = element->getElementsByTagNameNS(XMLString::transcode(xmlns.c_str()), XMLString::transcode(nodeName.c_str()));

	listLen = nodeList->getLength();
	while(index < listLen)
	{
		tmpNode = (DOMElement*)nodeList->item(index++);	
		if(tmpNode != NULL)
		{
			//	Check for attribute if desired
			if(attribute.compare("") == 0)
			{
				result = tmpNode;
				break;
			}else if(tmpNode->hasAttribute(XMLString::transcode(attribute.c_str())))
			{
				//	Check for attribute value if desired
				if(attValue.compare("") == 0)
				{
					result = tmpNode;
					break;
				}else if((GetAttributeByName(tmpNode, attribute)).compare(attValue) == 0)
				{
					result = tmpNode;
					break;
				}
			}
		}
	}

	return result;
}

DOMElement* XmlCommon::FindElementByAttribute(DOMElement *node, string attribute, string attValue) {
	
	DOMElement *child;
	DOMElement *found = NULL;

    if (node) {
		//	test to see if the node has attributes
		if(node->hasAttributes()) {
			// get all the attributes of the node
			DOMNamedNodeMap *pAttributes = node->getAttributes();
			int nSize = pAttributes->getLength();

			//	Loop through the attributes
		    for(int i=0;i<nSize;++i) {
				DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
				// get attribute name
				string attName = ToString(pAttributeNode->getName());

				//	Is this the attribute we are looking for
				if(attribute.compare(attName)==0) {   
					//	Get the attribute value
					string attVal = ToString(pAttributeNode->getValue());
					
					//	Is the value correct
					if(attValue.compare(attVal) == 0) {
						//	found the match we are looking for
						//	return a ptr to the node 
						found = node;
						break;
					}
				}
			}
			//	No attributes match for this node search children
			if(found == NULL) {
				//	loop through all child nodes calling this function recursively
				for (DOMNode *childNode = node->getFirstChild(); childNode != 0; childNode=childNode->getNextSibling()) {
					if(childNode->getNodeType() == DOMNode::ELEMENT_NODE) {
						child = (DOMElement*)childNode;
						found = FindElementByAttribute(child, attribute, attValue);
		
						//	Break out of the loop if found a node
						if(found != NULL)
							break;
					}
				}
			}

		//	No attributes found search its children
		} else 	{
			//	loop through all child nodes calling this function recursively
			for (DOMNode *childNode = node->getFirstChild(); childNode != 0; childNode=childNode->getNextSibling()) {
				if(childNode->getNodeType() == DOMNode::ELEMENT_NODE) {
					child = (DOMElement*)childNode;
					found = FindElementByAttribute(child, attribute, attValue);
	
					//	Break out of the loop if found a node
					if(found != NULL)
						break;
				}
			}
		}
    }

    return (found);
}

string XmlCommon::GetAttributeByName(DOMElement *node, string name) {
	
	string value = "";

	//	Check inputs
	if(node == NULL)
		throw XmlCommonException("Error: Unable to get attribute value. NULL node supplied\n");

	if(name.compare("") == 0)
		throw XmlCommonException("Error: Unable to get attribute value. NULL attribute name supplied\n");


	const XMLCh *attName = XMLString::transcode(name.c_str());
	value = ToString(((DOMElement*)node)->getAttribute(attName));

	//////////   DEBUG ////////////////////
	//	cout << "***** debug *****" <<endl;
	//	cout << "GetAttributeByName()" << endl;
	//	cout << "Name: " << name << endl;
	//	cout << "Value: " << value << endl;
	//////////   DEBUG ////////////////////

	return(value);
}

void XmlCommon::RemovetAttributeByName(DOMElement *node, string name) {

	const XMLCh *attName = XMLString::transcode(name.c_str());
	node->removeAttribute(attName);
}

string XmlCommon::GetDataNodeValue(DOMElement *node) {

	string result = "";

	//	Check input
	if(node == NULL)
		throw XmlCommonException("Error: Attempted to get the value of a NULL node.");

	if(node->hasChildNodes()) {
		DOMNodeList *childList = node->getChildNodes();
		if(childList->getLength() == 1) {
			DOMNode *child = node->getFirstChild();
			if(child->getNodeType() == DOMNode::TEXT_NODE)
				result = ToString(((DOMText*)child)->getData());
			else
				throw XmlCommonException("Error: When getting the value of a node there should be exactly one child of that node and it should be a TEXT_NODE");

		} else {
			throw XmlCommonException("Error: When getting the value of a node there should be exactly one child of that node. The specified node has more than one child.");
		}
	} else {
	//	throw XmlCommonException("Error: When getting the value of a node there should be exactly one child of that node. The specified node has no children.");
		result = "";
	}

	return result;
}

string XmlCommon::GetElementName(DOMElement *elm) {

	string name = "";

	name = XmlCommon::ToString(elm->getLocalName());
	if (name.compare("") == 0) {
		name = XmlCommon::ToString(elm->getTagName());
	}

	return name;
}

string XmlCommon::GetElementPrefix(DOMElement *elm) {

	string prefix = "";

	prefix = XmlCommon::ToString(elm->getPrefix());
	if (prefix.compare("") == 0) {
		prefix = "";
	}

	return prefix;
}

bool XmlCommon::HasChildElements(DOMNode *node) {

	bool hasChildElms = false;

	//	check for child nodes of the definition node that are ELEMENT_NODE's
	DOMNodeList *nodeChildList = node->getChildNodes();
	unsigned int index = 0;
	while(index < nodeChildList->getLength()) {
		DOMNode *child = nodeChildList->item(index++);
		if(child->getNodeType() == DOMNode::ELEMENT_NODE) {
			hasChildElms = true;
			break;
		}
	}

	return hasChildElms;
}

string XmlCommon::ToString(const XMLCh *xml) {

	string result = "";
	char *tmp;
	
	if(xml != NULL) {
		tmp = XMLString::transcode(xml);
		result = tmp;
		XMLString::release(&tmp);
	}

	return(result);
}

void XmlCommon::AddXmlns(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc, string newXmlnsUri, string newXmlnsAlias) {

	DOMElement *rootElm = doc->getDocumentElement();
	if(newXmlnsAlias.compare("") == 0) {
		XmlCommon::AddAttribute(rootElm, "xmlns", newXmlnsUri);
	} else {
		XmlCommon::AddAttribute(rootElm, "xmlns:" + newXmlnsAlias, newXmlnsUri);
	}
}

void XmlCommon::AddSchemaLocation(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc, string newSchemaLocation) {

	DOMElement *rootElm = doc->getDocumentElement();
	string currentSchemaLocation = XmlCommon::GetAttributeByName(rootElm, "xsi:schemaLocation");

	if(currentSchemaLocation.compare("") != 0) {
		size_t pos = currentSchemaLocation.find(newSchemaLocation, 0);
		if(pos == string::npos) {
			currentSchemaLocation.append(" " + newSchemaLocation);
			XmlCommon::AddAttribute(rootElm, "xsi:schemaLocation", currentSchemaLocation);
		}
	} else {
		XmlCommon::AddAttribute(rootElm, "xsi:schemaLocation", newSchemaLocation);
	}
}	

string XmlCommon::GetNamespace(DOMElement *element) {

	string xmlns = "";
	xmlns = XmlCommon::ToString(element->getTypeInfo()->getNamespace());
	if (xmlns.compare("") == 0) {
		xmlns = "";
	}
	return xmlns;
}
//****************************************************************************************//
//							XmlCommonException Class									  //	
//****************************************************************************************//
XmlCommonException::XmlCommonException(string errMsgIn, int severity) : Exception(errMsgIn, severity) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Set the error message and then set the severity to ERROR_FATAL. This is 
	//	done with the explicit call to the Exception class constructor that 
	//	takes a single string param.
	//
	// -----------------------------------------------------------------------

}

XmlCommonException::~XmlCommonException()
{
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Do nothing for now
	//
	// -----------------------------------------------------------------------

}

