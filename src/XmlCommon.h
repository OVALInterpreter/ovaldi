//
// $Id: XmlCommon.h 4579 2008-01-02 17:39:07Z bakerj $
//
//****************************************************************************************//
// Copyright (c) 2002-2008, The MITRE Corporation
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

#ifndef XMLCOMMON_H
#define XMLCOMMON_H

/**
  msxml.h defines its own DOMDocument so we have to safegaurd against this.
  If msxml.h was included and the XERCES headers have not been included yet, 
  undefine DOMDocument so that XERCES can redefine it.
**/

#ifdef	__msxml_h__
#ifndef	DOMDocument_HEADER_GUARD_
#undef	DOMDocument
#endif  /* DOMDocument_HEADER_GUARD_ */
#endif  /* __msxml_h__ */

//	required xerces includes
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMAttr.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/parsers/AbstractDOMParser.hpp>


//	other includes
#include <string>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <vector>

#include "Exception.h"

//	namespaces
XERCES_CPP_NAMESPACE_USE
using namespace std;

/**	
	A vector for storing DOMElement objects. 
	Stores only pointers to the objects. 
*/
typedef vector < DOMElement*, allocator<DOMElement*> > ElementVector;

/**
	This class encapsulates a set of static methods for manipulating XML
*/
class XmlCommon {
public:
	/** Add an attribute to the specified DOMElement. */
	static void AddAttribute(DOMElement *node, string attName, string attValue); 
	/** Add a new DOMElement node to the parent node.  
		Use nodeName and nodeValue to construct the new node. Only attempt
		to add a value if a value is specified. Return a ptr to the new DOMElement. 
	*/
	static DOMElement* AddChildElement(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc, DOMElement *parent, string nodeName, string nodeValue = "");
	/** Create a new DOMElement node with the specified value. 
		Use nodeName and nodeValue to construct the new node. Only attempt
		to add a value if a value is specified. 
		Return a ptr to the new DOMElement. 
	*/
	static DOMElement* CreateElement(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument*, string, string nodeValue = "");
	/** Return the all nodes in the specified document that match the node name and attribute value.
		Attribute name and value are optional as well as xmlns.
	*/
	static ElementVector* FindAllElements(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *node, string nodeName, string attribute = "", string attValue = "", string xmlns = "*");
	/** Return the first node found that has the corresponding name with the attribute and attribute value specified.
		Requires input of at least a node name and a DOMDocument to search. 
		The attribute and attribute value parameters are optional.
	*/
	static DOMElement* FindElement(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc, string nodeName, string attribute = "", string attValue ="");
	/** Return the first node found that has the corresponding name with the attribute and attribute value specified.
		Requires input of at least a DOMElement name and a DOMElement.
		The attribute and attribute value parameters are optional.
	*/
	static DOMElement* FindElement(DOMElement *node, string nodeName, string attribute = "", string attValue ="");
	/**	Return the first node found that has the corresponding name with the attribute and attribute value specified and the specified xmlns.
		Requires input of at least a DOMElement name and a DOMDocument.
		Attribute name and value are optional as well as xmlns.
	*/
	static DOMElement* FindElementNS(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc, string nodeName, string attribute = "", string attValue ="", string xmlns = "*");
	/**	Return the first node found that has the corresponding name with the attribute and attribute value specified and the specified xmlns.
		Requires input of at least a DOMElement name and a DOMElement.
		Attribute name and value are optional as well as xmlns.
	*/
	static DOMElement* FindElementNS(DOMElement *node, string nodeName, string attribute = "", string attValue ="", string xmlns = "*");
	/** Recursively search the specified  DOMElement for an element with a corresponding attribute and attribute value.
	*/
	static DOMElement* FindElementByAttribute(DOMElement *node, string attribute, string attValue);
	/**	Get the name of the specified attribute.
		Return empty string if the attribute is not found. 
	*/
	static string GetAttributeByName(DOMElement *node, string name);
	/**	Get the text value of the specified node. 
		Return an empty string if there is no value. 
		Throws an exception if the specifeid DOMElement has child elements.
	*/
	static string GetDataNodeValue(DOMElement*);
	/** Get the name of the specified element. */
	static string GetElementName(DOMElement*);
	/** Get the prefix of the specified element. */
	static string GetElementPrefix(DOMElement*);
	/** Return true if the specified node has child elements. */
	static bool HasChildElements(DOMNode*);
	/**	Remove the specified attribute. */
	static void RemovetAttributeByName(DOMElement*, string);
	/** Convert the XMLCh* to a string and handle memory allocation. */
	static string ToString(const XMLCh*);
	/** Add the specifed namespace to the root element in the specified document. */
	static void AddXmlns(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc, string newXmlnsUri, string newXmlnsAlias = "");
	/** Add the specified schema location to the document. 
		Ensures that schema locations are unique. 
	*/
	static void AddSchemaLocation(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc, string newSchemaLocation);
	//static void SplitnNSPrefixandElmenetName(string nameAndPrefix
	/** Remove all the attributes from the specified element. */
	static void RemoveAttributes(DOMElement* elm);
	/** Copy the schema location from the source document to the destionation document. */
	static void CopySchemaLocation(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* source, XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* dest);
	/** Copy the namespace on the source elmement to the destination element.
		Do not overwrite the destination element's default xmlns. 
	*/
	static void CopyNamespaces(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* source, XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* dest); 
	
	/** Return the namespace of the specifed element or null if no namespace is present. */
	static string GetNamespace(DOMElement*);
};

/** 
	This class represents an Exception that occured using one of the XmlCommon functions.
*/
class XmlCommonException : public Exception {
	public:
		XmlCommonException(string errMsgIn = "", int severity = ERROR_FATAL);
		~XmlCommonException();
};

#endif
