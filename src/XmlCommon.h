//
//
//****************************************************************************************//
// Copyright (c) 2002-2013, The MITRE Corporation
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

//	other includes
#include <string>
#include <vector>

//	required xerces includes
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMDocument.hpp>

#include "Exception.h"

/**	
	A vector for storing DOMElement objects. 
	Stores only pointers to the objects. 
*/
typedef std::vector < xercesc::DOMElement* > ElementVector;

/**
	This class encapsulates a set of static methods for manipulating XML
*/
class XmlCommon {
public:
	/** Add an attribute to the specified DOMElement. */
	static void AddAttribute(xercesc::DOMElement *node, std::string attName, std::string attValue); 
	/** Add a namespaced attribute to the specified DOMElement. */
	static void AddAttributeNS(xercesc::DOMElement *node, std::string ns, std::string attName, std::string attValue);
	/** Add a new DOMElement node to the parent node.  
		Use nodeName and nodeValue to construct the new node. Only attempt
		to add a value if a value is specified. Return a ptr to the new DOMElement. 
	*/
	static xercesc::DOMElement* AddChildElement(xercesc::DOMDocument *doc, xercesc::DOMElement *parent, std::string nodeName, std::string nodeValue = "");
	/** Add a new DOMElement node to the parent node.  
	* Return a ptr to the new DOMElement. 
	*/
	static xercesc::DOMElement* AddChildElementNS(xercesc::DOMDocument *doc, xercesc::DOMElement *parent, std::string ns, std::string nodeName, std::string nodeValue = "");
	/** Create a new DOMElement node with the specified value. 
		Use nodeName and nodeValue to construct the new node. Only attempt
		to add a value if a value is specified. 
		Return a ptr to the new DOMElement. 
	*/
	static xercesc::DOMElement* CreateElement(xercesc::DOMDocument*, std::string nodeName, std::string nodeValue = "");
	/** Create a new namespaced DOMElement node with the specified value.
		Return a ptr to the new DOMElement. 
	*/
	static xercesc::DOMElement* CreateElementNS(xercesc::DOMDocument*, std::string ns, std::string nodeName, std::string nodeValue = "");
	/** Return the all nodes in the specified document that match the node name and attribute value.
		Attribute name and value are optional as well as xmlns.
	*/
	static ElementVector* FindAllElements(xercesc::DOMDocument *node, std::string nodeName, std::string attribute = "", std::string attValue = "", std::string xmlns = "*");
	/** Return the first node found that has the corresponding name with the attribute and attribute value specified.
		Requires input of at least a node name and a DOMDocument to search. 
		The attribute and attribute value parameters are optional.
	*/
	static xercesc::DOMElement* FindElement(xercesc::DOMDocument *doc, std::string nodeName, std::string attribute = "", std::string attValue ="");
	/** Return the first node found that has the corresponding name with the attribute and attribute value specified.
		Requires input of at least a DOMElement name and a DOMElement.
		The attribute and attribute value parameters are optional.
	*/
	static xercesc::DOMElement* FindElement(xercesc::DOMElement *node, std::string nodeName, std::string attribute = "", std::string attValue ="");
	/**	Return the first node found that has the corresponding name with the attribute and attribute value specified and the specified xmlns.
		Requires input of at least a DOMElement name and a DOMDocument.
		Attribute name and value are optional as well as xmlns.
	*/
	static xercesc::DOMElement* FindElementNS(xercesc::DOMDocument *doc, std::string nodeName, std::string attribute = "", std::string attValue ="", std::string xmlns = "*");
	/**	Return the first node found that has the corresponding name with the attribute and attribute value specified and the specified xmlns.
		Requires input of at least a DOMElement name and a DOMElement.
		Attribute name and value are optional as well as xmlns.
	*/
	static xercesc::DOMElement* FindElementNS(xercesc::DOMElement *node, std::string nodeName, std::string attribute = "", std::string attValue ="", std::string xmlns = "*");
	/** Recursively search the specified  DOMElement for an element with a corresponding attribute and attribute value.
	*/
	static xercesc::DOMElement* FindElementByAttribute(xercesc::DOMElement *node, std::string attribute, std::string attValue);
	/**	Get the name of the specified attribute.
		Return empty string if the attribute is not found. 
	*/
	static std::string GetAttributeByName(xercesc::DOMElement *node, std::string name);
	/**	Get the text value of the specified node. 
		Return an empty string if there is no value. 
		Throws an exception if the specifeid DOMElement has child elements.
	*/
	static std::string GetDataNodeValue(xercesc::DOMElement*);
	/** Get the name of the specified element. */
	static std::string GetElementName(xercesc::DOMElement*);
	/** Get the prefix of the specified element. */
	static std::string GetElementPrefix(xercesc::DOMElement*);
	/** Return true if the specified node has child elements. */
	static bool HasChildElements(xercesc::DOMNode*);
	/**	Remove the specified attribute. */
	static void RemovetAttributeByName(xercesc::DOMElement*, std::string);
	/** Convert the XMLCh* to a string and handle memory allocation. */
	static std::string ToString(const XMLCh*);
	/** Add the specified namespace to the root element in the specified document. */
	static void AddXmlns(xercesc::DOMDocument *doc, std::string newXmlnsUri, std::string newXmlnsAlias = "");
	/** Add the specified schema location to the document. 
		Ensures that schema locations are unique. 
	*/
	static void AddSchemaLocation(xercesc::DOMDocument *doc, std::string newSchemaLocation);
	//static void SplitnNSPrefixandElmenetName(std::string nameAndPrefix
	/** Remove all the attributes from the specified element. */
	static void RemoveAttributes(xercesc::DOMElement* elm);
	/** Copy the schema location from the source document to the destionation document. */
	static void CopySchemaLocation(xercesc::DOMDocument* source, xercesc::DOMDocument* dest);
	/** Copy the namespace on the source elmement to the destination element.
		Do not overwrite the destination element's default xmlns. 
	*/
	static void CopyNamespaces(xercesc::DOMDocument* source, xercesc::DOMDocument* dest); 
	
	/** Return the namespace of the specified element or null if no namespace is present. */
	static std::string GetNamespace(xercesc::DOMElement*);

	/** The oval definitions XML namespace */
	static const std::string defNS;
	/** The oval system characteristics XML namespace */
	static const std::string scNS;
	/** The oval results XML namespace */
	static const std::string resNS;
	/** The oval common XML namespace */
	static const std::string comNS;
	/**
	 * The XML-Schema instance namespace:
	 * http://www.w3.org/2001/XMLSchema-instance
	 * It seems xerces doesn't have a predefined
	 * constant for this...
	 */
	static const std::string xsiNS;
};

/** 
	This class represents an Exception that occured using one of the XmlCommon functions.
*/
class XmlCommonException : public Exception {
	public:
		XmlCommonException(std::string errMsgIn = "", int severity = ERROR_FATAL);
		~XmlCommonException();
};

#endif
