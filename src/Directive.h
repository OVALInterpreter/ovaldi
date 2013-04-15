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

#ifndef DIRECTIVE_H
#define DIRECTIVE_H

#include <map>
#include "Common.h"
#include "OvalEnum.h"
#include "XmlCommon.h"

class Directive;

typedef std::map<OvalEnum::ResultEnumeration, Directive *> DirectiveMap;

/**
 * The Directive class is used to apply Directives to an OVAL results document.
 **/
class Directive
{
public:
	//
	// Static members
	//

	/**
	 * Returns a directive matching the specified type of result
	 *
	 * @param OvalEnum::ResultEnumeration
	 * @return Directive
	 **/
	static Directive GetDirective(OvalEnum::ClassEnumeration classEnum, OvalEnum::ResultEnumeration);

	/**
	 * Use the given directive for the specified type of result
	 *
	 * @param OvalEnum::ResultEnumeration
	 * @param Directive
	 * @return void
	 **/
	static void SetDirective(OvalEnum::ClassEnumeration classEnum, OvalEnum::ResultEnumeration, Directive);

	/**
	 * Load directives from the ovaldi configuration file.
	 *
	 * If the configuration file is not present, default values will be used.
	 *
	 * @return void
	 **/
	static void LoadDirectives();

	/**
	 * Apply all directives to the given results XML document
	 *
	 * @param DOMDocument*
	 * @return void
	 **/
	static void ApplyAll(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* resultsDoc);

	//
	// Public members
	//
	/**
	 * Constructor
	 *
	 * @param OvalEnum::ResultEnumeration type of result that this directive describes
	 * @param bool True if results matching this directive should be reported in the results file (default=true)
	 * @param OvalEnum::ResultContext Controls how much context information is provided for results matching this directive (default=OvalEnum::RESULT_CONTENT_FULL)
	 **/
	Directive(OvalEnum::ResultEnumeration result, bool reported=true, OvalEnum::ResultContent content=OvalEnum::RESULT_CONTENT_FULL);

	/**
	 * Get the type of result that this directive describes
	 *
	 * @return OvalEnum::ResultEnumeration
	 **/
	OvalEnum::ResultEnumeration GetResult();

	/**
	 * Value indicating if this type of result is reported in the results file
	 *
	 * @return bool true if reported
	 **/
	bool GetReported();

	/**
	 * Get a value indicating how much context information will be included about results matching this directive
	 *
	 * @return OvalEnum::ResultContent
	 **/
	OvalEnum::ResultContent GetResultContent();

	/**
	 * Sets the type of result that this directive describes
	 *
	 * @param OvalEnum::ResultEnumeration
	 * @return void
	 **/
	void SetResult(OvalEnum::ResultEnumeration value);

	/**
	 * Sets the value indicating if this result type will be reported in the results file
	 *
	 * @param bool true if reported
	 * @return void
	 **/
	void SetReported(bool value);

	/**
	 * Sets the value that determines how much context information will be included about the results matching this directive
	 *
	 * @param OvalEnum::ResultContent
	 * @return void
	 **/
	void SetResultContent(OvalEnum::ResultContent value);

private:
	/**
	 * Enumeration of the different types of elements in a results document that 
	 * are relevant to the Directive object
	 **/
	enum ElementType {ELEMENT_OBJECT,
						ELEMENT_VARIABLE,
						ELEMENT_DEFINITION,
						ELEMENT_STATE,
						ELEMENT_TEST,
						ELEMENT_ITEM
	};

	/**
	 * Static map of directives.
	 **/
	static std::map<OvalEnum::ClassEnumeration, DirectiveMap> directives;

	/**
	 * False if the user wants to exclude the source definitions from the results document
	 **/
	static bool includeSource;

	/**
	 * Build a list of references to other elements contained within the given DOMElement*.
	 *
	 * @param DOMElement* XML element to search for references within
	 * @param map<string,map<string, Directive::ElementType>>* Map to store the results in
	 * @param Directive::ElementType Type of element that is being searched
	 * @param string (default="id") Optional name of the attribute that contains the id of the given XML elements.
	 * @param string Optional id of the given element.  This should almost always be omitted.
	 * @return void
	 **/
	static void BuildReferences(DOMElement* parent, std::map<std::string,std::map<std::string, Directive::ElementType> >* references, Directive::ElementType type, std::string idAttr="id", std::string id="");

	/**
	 * Walk through a list of references generated by the BuildReferences function and generate a map
	 * associating each element with it's appropriate ResultContent value based on the ResultContent
	 * values of any elements that reference it.
	 *
	 * @param string id of the element to use as a starting point.  Usually the id of a definition element.
	 * @param map<string,map<string, Directive::ElementType>> References map generated by the BuildReferences function
	 * @param map<string, OvalEnum::ResultContent>* Pointer to a map in which to place the result
	 * @param OvalEnum::ResultContent Content type for the starting item.  This content type will be applied to child items.  If more than one element references the same child element, the OvalEnum::CombineResultContent function will be used to determine the appropriate value.
	 * @return void
	 **/
	static void WalkReferences(std::string id, std::map<std::string,std::map<std::string, Directive::ElementType> > references, std::map<std::string, OvalEnum::ResultContent>* included, OvalEnum::ResultContent type);

	/**
	 * Remove unwanted elements from the given XML container
	 *
	 * @param DOMElement* XML container to remove children from
	 * @param string Name of the attribute that contains the ID of the given elements
	 * @param OvalEnum::ResultContent Type of content that IS desired.  Other content will be discarded.  Content types can be combined using the bitwise OR operator.
	 * @param map<string, OvalEnum::ResultContent> Map containing the type of content associated with each element id.  This is generated using the WalkReferences function.
	 * @return void
	 **/
	static void RemoveUnwanted(DOMElement* parent, std::string idAttr, OvalEnum::ResultContent wantedTypes, std::map<std::string, OvalEnum::ResultContent> included);

	OvalEnum::ResultEnumeration result;
	bool reported;
	OvalEnum::ResultContent content;
};

#endif
