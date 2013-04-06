//
//
//****************************************************************************************//
// Copyright (c) 2002-2012, The MITRE Corporation
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

#include <iostream>
#include <sstream>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMNamedNodeMap.hpp>
#include <xercesc/dom/DOMText.hpp>
#include <xercesc/dom/DOMAttr.hpp>

#include "XmlCommon.h"

using namespace std;
using namespace xercesc;

const std::string XmlCommon::defNS = "http://oval.mitre.org/XMLSchema/oval-definitions-5";
const std::string XmlCommon::scNS = "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5";
const std::string XmlCommon::resNS = "http://oval.mitre.org/XMLSchema/oval-results-5";
const std::string XmlCommon::comNS = "http://oval.mitre.org/XMLSchema/oval-common-5";
const std::string XmlCommon::xsiNS = "http://www.w3.org/2001/XMLSchema-instance";

/**
 * The XML-Schema instance namespace as an
 * XMLCh string constant.
 */
static const XMLCh xsiNS[] = {
	chLatin_h, chLatin_t, chLatin_t, chLatin_p, 
	chColon, chForwardSlash, chForwardSlash,
	chLatin_w, chLatin_w, chLatin_w, chPeriod,
	chLatin_w, chDigit_3, chPeriod,
	chLatin_o, chLatin_r, chLatin_g, chForwardSlash,
	chDigit_2, chDigit_0, chDigit_0, chDigit_1, chForwardSlash,
	chLatin_X, chLatin_M, chLatin_L, 
	chLatin_S, chLatin_c, chLatin_h, chLatin_e, chLatin_m, chLatin_a,
	chDash, chLatin_i, chLatin_n, chLatin_s, chLatin_t, chLatin_a, chLatin_n, chLatin_c, chLatin_e,
	chNull
};

/**
 * An XMLCh string constant for "schemaLocation".
 */
static const XMLCh schemaLocation[] = {
	chLatin_s, chLatin_c, chLatin_h, chLatin_e, chLatin_m, chLatin_a,
	chLatin_L, chLatin_o, chLatin_c, chLatin_a, chLatin_t, chLatin_i, chLatin_o, chLatin_n,
	chNull
};

/**
 * An XMLCh string constant for "xsi:schemaLocation".
 */
static const XMLCh xsiSchemaLocation[] = {
	chLatin_x, chLatin_s, chLatin_i, chColon,
	chLatin_s, chLatin_c, chLatin_h, chLatin_e, chLatin_m, chLatin_a,
	chLatin_L, chLatin_o, chLatin_c, chLatin_a, chLatin_t, chLatin_i, chLatin_o, chLatin_n,
	chNull
};

//****************************************************************************************//
//										XmlCommon Class									  //	
//****************************************************************************************//
void XmlCommon::AddAttribute(DOMElement *node, string attName, string attValue) {

	XMLCh *name = XMLString::transcode(attName.c_str());
	XMLCh *value = XMLString::transcode(attValue.c_str());
	node->setAttribute(name, value);
	//Free memory allocated by XMLString::transcode(char*)
	XMLString::release(&name);
	XMLString::release(&value);

}

void XmlCommon::AddAttributeNS(DOMElement *node, string ns, string attName, string attValue) {

	XMLCh *xns = XMLString::transcode(ns.c_str());
	XMLCh *name = XMLString::transcode(attName.c_str());
	XMLCh *value = XMLString::transcode(attValue.c_str());
	node->setAttributeNS(xns, name, value);
	//Free memory allocated by XMLString::transcode(char*)
	XMLString::release(&xns);
	XMLString::release(&name);
	XMLString::release(&value);

}

DOMElement* XmlCommon::AddChildElement(DOMDocument *doc, DOMElement *parent, string nodeName, string nodeValue) {

	DOMElement *newElem = NULL;

	newElem = CreateElement(doc, nodeName, nodeValue);
	parent->appendChild(newElem);

	return newElem;
}

DOMElement* XmlCommon::AddChildElementNS(DOMDocument *doc, DOMElement *parent, string ns, string nodeName, string nodeValue) {
	DOMElement *newElem = CreateElementNS(doc, ns, nodeName, nodeValue);
	parent->appendChild(newElem);

	return newElem;
}

void XmlCommon::CopyNamespaces(DOMDocument* source, DOMDocument* dest) {

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

void XmlCommon::CopySchemaLocation(DOMDocument* source, DOMDocument* dest) {

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
		DOMNode* oldAttr = attributes->removeNamedItem(attr->getName());
		oldAttr->release();
		attributes = elm->getAttributes();
		len = attributes->getLength();
	}
}

DOMElement* XmlCommon::CreateElement(DOMDocument *doc, string nodeName, string nodeValue) {
	
	DOMText *tmpTextNode = NULL;
	DOMElement *newElem = NULL;

	XMLCh* name = XMLString::transcode(nodeName.c_str());
	newElem = doc->createElement(name);
	//Free memory allocated by XMLString::transcode(char*)
	XMLString::release(&name);
	if(nodeValue.compare("") != 0) {
		XMLCh* value = XMLString::transcode(nodeValue.c_str());
		tmpTextNode = doc->createTextNode(value);
		//Free memory allocated by XMLString::transcode(char*)
		XMLString::release(&value);
		newElem->appendChild(tmpTextNode);
	}

	return newElem;
}

DOMElement* XmlCommon::CreateElementNS(DOMDocument *doc, string ns, string nodeName, string nodeValue) {
	
	XMLCh* name = XMLString::transcode(nodeName.c_str());
	XMLCh *xns = XMLString::transcode(ns.c_str());
	DOMElement *newElem = doc->createElementNS(xns, name);
	//Free memory allocated by XMLString::transcode(char*)
	XMLString::release(&xns);
	XMLString::release(&name);
	if(!nodeValue.empty()) {
		XMLCh* value = XMLString::transcode(nodeValue.c_str());
		newElem->setTextContent(value);
		//Free memory allocated by XMLString::transcode(char*)
		XMLString::release(&value);
	}

	return newElem;
}

ElementVector* XmlCommon::FindAllElements(DOMDocument *doc, string nodeName, string attribute, string attValue, string xmlns) {

	ElementVector *nodes	= new ElementVector();
	DOMElement *tmpNode		= NULL;
	DOMNodeList *nodeList	= NULL;
	int listLen				= 0;
	int index				= 0;
	XMLCh* name = XMLString::transcode(nodeName.c_str());
	nodeList = doc->getElementsByTagName(name);
	//Free memory allocated by XMLString::transcode(char*)
	XMLString::release(&name);

	listLen = nodeList->getLength();
	XMLCh* attName = XMLString::transcode(attribute.c_str());
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
			}else if(tmpNode->hasAttribute(attName))
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
	//Free memory allocated by XMLString::transcode(char*)
	XMLString::release(&attName);

	return nodes;
}

DOMElement* XmlCommon::FindElement(DOMDocument *doc, string nodeName, string attribute, string attValue) {

	DOMElement *tmpNode		= NULL;
	DOMElement *result			= NULL;
	DOMNodeList *nodeList	= NULL;
	int listLen				= 0;
	int index				= 0;


	//	Get a list of all the nodes in the document with the nodeName and loop through them
	XMLCh* name = XMLString::transcode(nodeName.c_str());
	nodeList = doc->getElementsByTagName(name);
	//Free memory allocated by XMLString::transcode(char*)
	XMLString::release(&name);
	listLen = nodeList->getLength();
	XMLCh* attName = XMLString::transcode(attribute.c_str());
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
			}else if(tmpNode->hasAttribute(attName))
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
	//Free memory allocated by XMLString::transcode(char*)
	XMLString::release(&attName);
	
	return result;
}

DOMElement* XmlCommon::FindElement(DOMElement *element, string nodeName, string attribute, string attValue) {

	DOMElement *tmpNode		= NULL;
	DOMElement *result			= NULL;
	DOMNodeList *nodeList	= NULL;
	int listLen				= 0;
	int index				= 0;
	XMLCh* name = XMLString::transcode(nodeName.c_str());
	nodeList = element->getElementsByTagName(name);
	//Free memory allocated by XMLString::transcode(char*)
	XMLString::release(&name);
	listLen = nodeList->getLength();
	XMLCh* attName = XMLString::transcode(attribute.c_str());
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
			}else if(tmpNode->hasAttribute(attName))
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
	//Free memory allocated by XMLString::transcode(char*)
	XMLString::release(&attName);

	return result;
}

DOMElement* XmlCommon::FindElementNS(DOMDocument *doc, string nodeName, string attribute, string attValue, string xmlns) {
	return XmlCommon::FindElementNS(doc->getDocumentElement(), nodeName, attribute, attValue, xmlns);
}

DOMElement* XmlCommon::FindElementNS(DOMElement *element, string nodeName, string attribute, string attValue, string xmlns) {

	DOMElement *tmpNode		= NULL;
	DOMElement *result		= NULL;
	DOMNodeList *nodeList	= NULL;
	int listLen				= 0;
	int index				= 0;

	XMLCh* xmlnsValue = XMLString::transcode(xmlns.c_str());
	XMLCh* name = XMLString::transcode(nodeName.c_str());
	nodeList = element->getElementsByTagNameNS(xmlnsValue,name);
	//Free memory allocated by XMLString::transcode(char*)
	XMLString::release(&xmlnsValue);
	XMLString::release(&name);
	listLen = nodeList->getLength();

	if (attribute.empty()) {
		if (listLen > 0)
			return (DOMElement*)nodeList->item(0);
		return NULL;
	}

	XMLCh* attName = XMLString::transcode(attribute.c_str());
	while(index < listLen)
	{
		tmpNode = (DOMElement*)nodeList->item(index++);	
		if(tmpNode != NULL)
		{
			if(tmpNode->hasAttribute(attName))
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
	//Free memory allocated by XMLString::transcode(char*)
	XMLString::release(&attName);
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


	XMLCh *attName = XMLString::transcode(name.c_str());
	value = ToString(((DOMElement*)node)->getAttribute(attName));
	//Free memory allocated by XMLString::transcode(char*)
	XMLString::release(&attName);
	//////////   DEBUG ////////////////////
	//	cout << "***** debug *****" <<endl;
	//	cout << "GetAttributeByName()" << endl;
	//	cout << "Name: " << name << endl;
	//	cout << "Value: " << value << endl;
	//////////   DEBUG ////////////////////

	return(value);
}

void XmlCommon::RemovetAttributeByName(DOMElement *node, string name) {

	XMLCh *attName = XMLString::transcode(name.c_str());
	node->removeAttribute(attName);
	//Free memory allocated by XMLString::transcode(char*)
	XMLString::release(&attName);
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

void XmlCommon::AddXmlns(DOMDocument* doc, string newXmlnsUri, string newXmlnsAlias) {

	DOMElement *rootElm = doc->getDocumentElement();
	if(newXmlnsAlias.compare("") == 0) {
		XmlCommon::AddAttribute(rootElm, "xmlns", newXmlnsUri);
	} else {
		XMLCh *uri = XMLString::transcode(newXmlnsUri.c_str());
		XMLCh *attrName = XMLString::transcode(("xmlns:"+newXmlnsAlias).c_str());
		rootElm->setAttributeNS(XMLUni::fgXMLNSURIName, attrName, uri);
		XMLString::release(&uri);
		XMLString::release(&attrName);
	}
}

void XmlCommon::AddSchemaLocation(DOMDocument *doc, string newSchemaLocation) {

	DOMElement *rootElm = doc->getDocumentElement();
	string currentSchemaLocation;
	XMLCh *tmp;
	const XMLCh *ctmp;

	ctmp = rootElm->getAttributeNS(::xsiNS, schemaLocation);
	if (ctmp)
		currentSchemaLocation = ToString(ctmp);

	if(!currentSchemaLocation.empty()) {
		size_t pos = currentSchemaLocation.find(newSchemaLocation, 0);
		if(pos == string::npos) {
			currentSchemaLocation.append(" " + newSchemaLocation);
			tmp = XMLString::transcode(currentSchemaLocation.c_str());
			rootElm->setAttributeNS(::xsiNS, xsiSchemaLocation, tmp);
			XMLString::release(&tmp);
		}
	} else {
		tmp = XMLString::transcode(newSchemaLocation.c_str());
		rootElm->setAttributeNS(::xsiNS, xsiSchemaLocation, tmp);
		XMLString::release(&tmp);
	}
}	

string XmlCommon::GetNamespace(DOMElement *element) {

	string xmlns;
	const XMLCh *ns = element->lookupNamespaceURI(element->getPrefix());
	if (ns)
		xmlns = XmlCommon::ToString(ns);

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

