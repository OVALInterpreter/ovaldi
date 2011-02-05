//
//
//****************************************************************************************//
// Copyright (c) 2002-2011, The MITRE Corporation
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

#ifndef OBJECTCOMPONENT_H
#define OBJECTCOMPONENT_H

#include "AbsComponent.h"
#include "AbsObjectCollector.h"
#include "ObjectReader.h"


XERCES_CPP_NAMESPACE_USE

/**
	This class represents a ObjectComponent in a local_variable in the oval definition schema.
*/
class ObjectComponent : public AbsComponent {
public:

    /** Create a complete ObjectComponent object/ */
	ObjectComponent(std::string objectId = "", std::string itemField = "", std::string recordField = "");
	
	/** ObjectComponent destructor */
	~ObjectComponent();

    /** Parse the ObjectComponent element and populate the current ObjectComponent. */
	void Parse(DOMElement* componentElm);


    /** Calculate the value of this ObjectComponent
        <ul>
            <li>Make a call to AbsObjectCollector to collect the data</li>
            <li>Get the resulting collected object and then get the matching Item(s)</li>
            <li>Then look for the Item with the Entity named the same as this->itemField. If no matching items were found report it and return and empty set of values.</li>
            <li>Then get that value and return it as a string.</li>
        </ul>
    */
	ComponentValue* ComputeValue();

    /** Return the variable values used to compute this component's value. */
	VariableValueVector* GetVariableValues();

    /** Return the objectId field's value. */
	std::string GetObjectId();
    /** Set the objectId field's value. */
	void SetObjectId(std::string objectId);

    /** Return the itemField field's value. */
	std::string GetItemField();
    /** Set the itemField field's value. */
	void SetItemField(std::string itemField);

	/** Return the recordField field's value. */
	std::string GetRecordField();

    /** Set the recordField field's value. */
	void SetRecordField(std::string recordField);

private:
	std::string objectId;
	std::string itemField;
	std::string recordField;
};

#endif
