//
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

#ifndef TIMEDIFFERENCEFUNCTION_H
#define TIMEDIFFERENCEFUNCTION_H

#include "AbsFunctionComponent.h"
#include "ComponentFactory.h"

XERCES_CPP_NAMESPACE_USE
using namespace std;

/**
	This class represents a TimeDifferenceFunction component in a local_variable in the oval definition schema.

    The time_difference function calculates the difference in seconds between date-time 
    values. If one component is specified, the values of that component are subtracted
    from the current time (UTC). If two components are specified, the value of the second
    component is subtracted from the value of the first component. If the component(s) 
    contain multiple values, the operation is performed multiple times and the result is
    an array of time differences.

    The date-time format of each component is determined by the two format attributes. The
    format1 attribute applies to the first component, and the format2 attribute applies to
    the second component. Valid values for the attributes are 'win_filetime', 'seconds_since_epoch',
    'day_month_year', 'year_month_day', and 'month_day_year'. Please see the DateTimeFormatEnumeration
    for more information about each of these values. If an input value is not understood, the result
    is an error. If only one input is specified, specify the format with the format2 attribute, as the
    first input is considered to be the implied 'current time' input.

    Note that the datatype associated with the components should be 'string' or 'int' depending
    on which date time format is specified.  The result of this function though is always an integer.
            
*/
class TimeDifferenceFunction : public AbsFunctionComponent {
public:

	/** Create a complete TimeDifferenceFunction object. */
    TimeDifferenceFunction(OvalEnum::DateTimeFormat format1 = OvalEnum::DATETIME_YEAR_MONTH_DAY, OvalEnum::DateTimeFormat format2 = OvalEnum::DATETIME_YEAR_MONTH_DAY);
	~TimeDifferenceFunction();

	/** Parse the substring element and its child component element. */
	void Parse(DOMElement* componentElm); 

	/** Compute the desired substring and return the value. */
	ComponentValue* ComputeValue();

	/** Return the variable values used to compute this function's value. */
	VariableValueVector* GetVariableValues();

	/** Get the format1 field's value. */
	OvalEnum::DateTimeFormat GetFormat1();
	/** Set the format1 field's value. */
	void SetFormat1(OvalEnum::DateTimeFormat format1);

    /** Get the format2 field's value. */
	OvalEnum::DateTimeFormat GetFormat2();
	/** Set the format2 field's value. */
	void SetFormat2(OvalEnum::DateTimeFormat format2);


private:
    
	OvalEnum::DateTimeFormat format1;
    OvalEnum::DateTimeFormat format2;

};

#endif
