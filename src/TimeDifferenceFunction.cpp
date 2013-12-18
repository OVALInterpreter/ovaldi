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

#include <algorithm>
#include <iterator>
//#include <math.h>
#include <time.h>
#include <cstdlib>
#include <cstring>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMNodeList.hpp>

#include "ComponentFactory.h"
#include "Common.h"
#include "XmlCommon.h"
#include "Exception.h"

#include "TimeDifferenceFunction.h"

using namespace std;
using namespace xercesc;

//****************************************************************************************//
//								Component Class											  //	
//****************************************************************************************//
// ***************************************************************************************	//
//								 Public members												//
// ***************************************************************************************	//
ComponentValue* TimeDifferenceFunction::ComputeValue() {
	
	AbsComponentVector * components = this->GetComponents();
	AbsComponent * component1 = NULL;
	AbsComponent * component2 = NULL;
	ComponentValue* component1Value = NULL;
	ComponentValue* component2Value = NULL;
	ComponentValue* result = new ComponentValue();
	string value1;
	string value2;
	time_t firstTime;
	time_t secondTime;
	string format1 = OvalEnum::DateTimeFormatToString(this->GetFormat1());
	string format2 = OvalEnum::DateTimeFormatToString(this->GetFormat2());
	
	// One component was specified
	if ( components->size() == 1 ){
		time_t currentTime = time(NULL);
		component2 = components->at(0);	
		component1Value = new ComponentValue();
		component1Value->SetFlag(OvalEnum::FLAG_COMPLETE);
		component1Value->AppendValue(Common::ToString((long int)(currentTime)));
		component2Value = component2->ComputeValue();
		value2 = component2Value->GetValues()->at(0);
	}

	// Two components were specified
	else if ( components->size() == 2 ){
		component1 = components->at(0);	
		component2 = components->at(1);
		
		component1Value = component1->ComputeValue();
		component2Value = component2->ComputeValue();

		value1 = component1Value->GetValues()->at(0);
		value2 = component2Value->GetValues()->at(0);
	}

	if ( component1Value != NULL && component2Value != NULL ){
		// Combine the flag values of components 1 and 2, and set the new value
		IntVector* flags = new IntVector();
		flags->push_back(component1Value->GetFlag());
		flags->push_back(component2Value->GetFlag());
		result->SetFlag(OvalEnum::CombineFlags(flags));
		delete flags;
		
		result->AppendMessages(component1Value->GetMessages());
		result->AppendMessages(component2Value->GetMessages());

		if(component2Value->GetFlag() == OvalEnum::FLAG_COMPLETE) {

			unsigned int component1Size = component1Value->GetValues()->size();
			unsigned int component2Size = component2Value->GetValues()->size();

			// Perform the timedifference operation on the Cartesian product of the two components.
			// For example assume a local_variable specifies the timedifference function and has two sub-components 
			// under this function: the first component returns multiple values "04/02/2009" and "04/03/2009", and the second component
			// returns multiple values "02/02/2005" and "02/03/2005" and "02/04/2005". The local_variable element would be evaluated to have six
			// values:  (ToSeconds("04/02/2009") - ToSeconds("02/02/2005")), (ToSeconds("04/02/2009") - ToSeconds("02/03/2005")),
			// (ToSeconds("04/02/2009") - ToSeconds("02/04/2005")), (ToSeconds("04/03/2009") - ToSeconds("02/02/2005")), 
			// (ToSeconds("04/03/2009") - ToSeconds("02/03/2005")), and (ToSeconds("04/03/2009") - ToSeconds("02/04/2005")).
			
			for(unsigned int counter1 = 0 ; counter1 < component1Size ; counter1++){
				for(unsigned int counter2 = 0 ; counter2 < component2Size ; counter2++){
					string currentComponent2Value = component2Value->GetValues()->at(counter2);
					string currentComponent1Value ="";

					if ( component1 == NULL ){
						firstTime = time(NULL);
					}
					
					// Get time in seconds for component1
					else{
						// Get corresponding value in component1
						currentComponent1Value = component1Value->GetValues()->at(counter1);

						// Get the number of seconds since the epoch using the function that corresponds to specified date-time format
						if(format1.compare(OvalEnum::DateTimeFormatToString(OvalEnum::DATETIME_YEAR_MONTH_DAY)) == 0 ) {
							firstTime = TimeDifferenceFunction::YearMonthDayValueToSeconds(currentComponent1Value);
						} else if(format1.compare(OvalEnum::DateTimeFormatToString(OvalEnum::DATETIME_MONTH_DAY_YEAR)) == 0) {
							firstTime = TimeDifferenceFunction::MonthDayYearValueToSeconds(currentComponent1Value);
						} else if(format1.compare(OvalEnum::DateTimeFormatToString(OvalEnum::DATETIME_DAY_MONTH_YEAR)) == 0) {
							firstTime = TimeDifferenceFunction::DayMonthYearValueToSeconds(currentComponent1Value);
						} else if(format1.compare(OvalEnum::DateTimeFormatToString(OvalEnum::DATETIME_WIN_FILETIME)) == 0) {
							firstTime = TimeDifferenceFunction::WinFiletimeValueToSeconds(currentComponent1Value);
						} else if(format1.compare(OvalEnum::DateTimeFormatToString(OvalEnum::DATETIME_SECONDS_SINCE_EPOCH)) == 0) {
							firstTime = TimeDifferenceFunction::SecondsSinceEpochValueToSeconds(currentComponent1Value);
						} else {
							if ( result != NULL ){
								delete result;
								result = NULL;
							}
							throw Exception("Error: An unsupported date-time format value \'" + format1 + "\' was found.");
						}
					}

					// Get time in seconds for component2
					if(format2.compare(OvalEnum::DateTimeFormatToString(OvalEnum::DATETIME_YEAR_MONTH_DAY)) == 0 ) {
						secondTime = TimeDifferenceFunction::YearMonthDayValueToSeconds(currentComponent2Value);
					} else if(format2.compare(OvalEnum::DateTimeFormatToString(OvalEnum::DATETIME_MONTH_DAY_YEAR)) == 0) {
						secondTime = TimeDifferenceFunction::MonthDayYearValueToSeconds(currentComponent2Value);
					} else if(format2.compare(OvalEnum::DateTimeFormatToString(OvalEnum::DATETIME_DAY_MONTH_YEAR)) == 0) {
						secondTime = TimeDifferenceFunction::DayMonthYearValueToSeconds(currentComponent2Value);
					} else if(format2.compare(OvalEnum::DateTimeFormatToString(OvalEnum::DATETIME_WIN_FILETIME)) == 0) {
						secondTime = TimeDifferenceFunction::WinFiletimeValueToSeconds(currentComponent2Value);
					} else if(format2.compare(OvalEnum::DateTimeFormatToString(OvalEnum::DATETIME_SECONDS_SINCE_EPOCH)) == 0) {
						secondTime = TimeDifferenceFunction::SecondsSinceEpochValueToSeconds(currentComponent2Value);
					} else {
						if ( result != NULL ){
							delete result;
							result = NULL;
						}
						throw Exception("Error: An unsupported date-time format value \'" + format2 + "\' was found.");
					}
				
					// Calculate the time difference between corresponding date-time values					
					result->AppendValue(Common::ToString((long int)(firstTime-secondTime)));
				}
			}
		}
	}

	if ( components->size() == 1 && component1Value != NULL ){
		delete component1Value;
		component1Value = NULL;
	}

	return result;	
}

void TimeDifferenceFunction::Parse(DOMElement* componentElm) {

	// Get the start and length attrs
	string format_1Str = XmlCommon::GetAttributeByName(componentElm, "format_1");
    this->SetFormat1(OvalEnum::ToDateTimeFormat(format_1Str));
	string format_2Str = XmlCommon::GetAttributeByName(componentElm, "format_2");
	this->SetFormat2(OvalEnum::ToDateTimeFormat(format_2Str));

	// Loop through all child elements
	DOMNodeList *componentElms = componentElm->getChildNodes();
	unsigned int index = 0;

	while(index < componentElms->getLength()) {
		DOMNode *tmpNode = componentElms->item(index);
		if (tmpNode->getNodeType() == DOMNode::ELEMENT_NODE) {
			DOMElement *childElm = (DOMElement*)tmpNode;

			// Call the ComponentFactory
			AbsComponent* absComponent = ComponentFactory::GetComponent(childElm);
			
			// store the returned component
			this->AppendComponent(absComponent);			
		}
		index ++;
	}
}

VariableValueVector TimeDifferenceFunction::GetVariableValues() {
	
	VariableValueVector values;
	AbsComponentVector* components = this->GetComponents();
	
	AbsComponentVector::iterator iterator;
	for(iterator = components->begin(); iterator != components->end(); iterator++) {
		AbsComponent* component = (AbsComponent*)(*iterator);
		VariableValueVector tmp = component->GetVariableValues();
		copy(tmp.begin(), tmp.end(), back_inserter(values));
	}

	return values;
}

// Determine the number of seconds since the epoch for date-time values of type year_month_day
time_t TimeDifferenceFunction::YearMonthDayValueToSeconds(string dateTimeValue){
	
	string dateTimeFormatStr = OvalEnum::DateTimeFormatToString(OvalEnum::DATETIME_YEAR_MONTH_DAY);
	time_t calculatedSeconds;
	size_t index;
	string year;
	string month;
	string day;
	string hours;
	string minutes;
	string seconds;
	
		// Found date-time value of format yyyymmddThhmmss
		if ( (index = dateTimeValue.find_first_of("T")) != string::npos ){
			year = dateTimeValue.substr(0,4);
			month = dateTimeValue.substr(4,2);
			day = dateTimeValue.substr(6,2);
			hours = dateTimeValue.substr(index+1,2);
			minutes = dateTimeValue.substr(index+3,2);
			seconds = dateTimeValue.substr(index+5,2);
			
			// Make sure the values are numerical
			if ( year.find_first_not_of("0123456789") != string::npos ||
				 month.find_first_not_of("0123456789") != string::npos ||
				 day.find_first_not_of("0123456789") != string::npos ||
				 hours.find_first_not_of("0123456789") != string::npos ||
				 minutes.find_first_not_of("0123456789") != string::npos ||
				 seconds.find_first_not_of("0123456789") != string::npos ){

					 throw Exception("Error: There was an error parsing the date-time value string: \'" + dateTimeValue + "\'. Please refer to the \'" + dateTimeFormatStr + "\' format specification under the DateTimeFormatEnumeration section in the OVAL Element Dictionary.");	
			
			}
			
			// Retrieve the number of seconds since the epoch for this date-time value
			calculatedSeconds = TimeDifferenceFunction::buildTimeStructure(atoi(year.c_str()),atoi(month.c_str()),atoi(day.c_str()),atoi(hours.c_str()),atoi(minutes.c_str()),atoi(seconds.c_str()));
			
		}

		// Found date-time value of format yyyy/mm/dd hh:mm:ss or yyyy-mm-dd hh:mm:ss
		else if ( (index = dateTimeValue.find_first_of(" ")) != string::npos){
			StringVector* value = new StringVector();
			char * dateTimeValueToken;
			char * dateTimeValueCStr = (char*)malloc(sizeof(char)*(dateTimeValue.length()+1));
			
			strcpy(dateTimeValueCStr,dateTimeValue.c_str());
			dateTimeValueToken = strtok(dateTimeValueCStr,"/ -:");

			while ( dateTimeValueToken != NULL ){
				value->push_back(dateTimeValueToken);
				dateTimeValueToken = strtok(NULL,"/ -:");
			}

			year = value->at(0);
			month = value->at(1);
			day = value->at(2);
			hours = value->at(3);
			minutes = value->at(4);
			seconds = value->at(5);

			// Make sure the values are numerical
			if ( year.find_first_not_of("0123456789") != string::npos ||
				 month.find_first_not_of("0123456789") != string::npos ||
				 day.find_first_not_of("0123456789") != string::npos ||
				 hours.find_first_not_of("0123456789") != string::npos ||
				 minutes.find_first_not_of("0123456789") != string::npos ||
				 seconds.find_first_not_of("0123456789") != string::npos ){
					if ( dateTimeValueCStr != NULL ){
						free(dateTimeValueCStr);
						dateTimeValueCStr = NULL;
					}
					if ( value != NULL ){
						delete value;
						value = NULL;
					}
					throw Exception("Error: There was an error parsing the date-time value string: \'" + dateTimeValue + "\'. Please refer to the \'" + dateTimeFormatStr + "\' format specification under the DateTimeFormatEnumeration section in the OVAL Element Dictionary.");	
			
			}

		
			if ( dateTimeValueCStr != NULL ){
				free(dateTimeValueCStr);
				dateTimeValueCStr = NULL;
			}
			if ( value != NULL ){
				delete value;
				value = NULL;
			}
			
			// Retrieve the number of seconds since the epoch for this date-time value
			calculatedSeconds = TimeDifferenceFunction::buildTimeStructure(atoi(year.c_str()),atoi(month.c_str()),atoi(day.c_str()),atoi(hours.c_str()),atoi(minutes.c_str()),atoi(seconds.c_str()));
		}

		// Found date-time value of format yyyy/mm/dd or yyyy-mm-dd
		else if ( (index = dateTimeValue.find_first_of("/-")) != string::npos){
			StringVector* value = new StringVector();
			char * dateTimeValueToken;
			char * dateTimeValueCStr = (char*)malloc(sizeof(char)*(dateTimeValue.length()+1));
			strcpy(dateTimeValueCStr,dateTimeValue.c_str());
			dateTimeValueToken = strtok(dateTimeValueCStr,"/-");
			while ( dateTimeValueToken != NULL ){
				value->push_back(dateTimeValueToken);
				dateTimeValueToken = strtok(NULL,"/-");
			}
			year = value->at(0);
			month = value->at(1);
			day = value->at(2);

			// Make sure the values are numerical
			if ( year.find_first_not_of("0123456789") != string::npos ||
				 month.find_first_not_of("0123456789") != string::npos ||
				 day.find_first_not_of("0123456789") != string::npos ){
					if ( dateTimeValueCStr != NULL ){
						free(dateTimeValueCStr);
						dateTimeValueCStr = NULL;
					}
					if ( value != NULL ){
						delete value;
						value = NULL;
					}
					throw Exception("Error: There was an error parsing the date-time value string: \'" + dateTimeValue + "\'. Please refer to the \'" + dateTimeFormatStr + "\' format specification under the DateTimeFormatEnumeration section in the OVAL Element Dictionary.");	
			
			}

			if ( dateTimeValueCStr != NULL ){
				free(dateTimeValueCStr);
				dateTimeValueCStr = NULL;
			}
			if ( value != NULL ){
				delete value;
				value = NULL;
			}
	
		// Retrieve the number of seconds since the epoch for this date-time value
		calculatedSeconds = TimeDifferenceFunction::buildTimeStructure(atoi(year.c_str()),atoi(month.c_str()),atoi(day.c_str()));
		
		}
		// Found date-time value of format yyyymmdd or an unknown format
		else{
			// If a non-numerical value is found then the format is unrecognized
			if ( dateTimeValue.find_first_not_of("0123456789") != string::npos ){
					throw Exception("Error: There was an error parsing the date-time value string: \'" + dateTimeValue + "\'. Please refer to the \'" + dateTimeFormatStr + "\' format specification under the DateTimeFormatEnumeration section in the OVAL Element Dictionary.");	
			}else{
				year = dateTimeValue.substr(0,4);
				month = dateTimeValue.substr(4,2);
				day = dateTimeValue.substr(6,2);

				// Make sure the values are numerical
				if ( year.find_first_not_of("0123456789") != string::npos ||
					 month.find_first_not_of("0123456789") != string::npos ||
					 day.find_first_not_of("0123456789") != string::npos ){
					
						throw Exception("Error: There was an error parsing the date-time value string: \'" + dateTimeValue + "\'. Please refer to the \'" + dateTimeFormatStr + "\' format specification under the DateTimeFormatEnumeration section in the OVAL Element Dictionary.");	
			
				}

				// Retrieve the number of seconds since the epoch for this date-time value
				calculatedSeconds = TimeDifferenceFunction::buildTimeStructure(atoi(year.c_str()),atoi(month.c_str()),atoi(day.c_str()));
		
			}
		}

		return calculatedSeconds;
}

// Determine the number of seconds since the epoch for date-time values of type month_day_year
time_t TimeDifferenceFunction::MonthDayYearValueToSeconds(string dateTimeValue){

	string dateTimeFormatStr = OvalEnum::DateTimeFormatToString(OvalEnum::DATETIME_MONTH_DAY_YEAR);
	time_t calculatedSeconds;
	string year;
	string month;
	string day;
	string hours;
	string minutes;
	string seconds;
	StringVector* value = new StringVector();
	char * dateTimeValueToken;
	char * dateTimeValueCStr = (char*)malloc(sizeof(char)*(dateTimeValue.length()+1));

	strcpy(dateTimeValueCStr,dateTimeValue.c_str());
	dateTimeValueToken = strtok(dateTimeValueCStr,",/ -:");
	
	while ( dateTimeValueToken != NULL ){
		value->push_back(dateTimeValueToken);
		dateTimeValueToken = strtok(NULL,",/ -:");
	}
	
	// Found date-time value of format mm/dd/yyyy or mm-dd-yyyy (first item is a number and doesn't contain a time component)
	if ( value->front().find_first_not_of("0123456789") == string::npos ){
		if ( value->size() == 3 ){
			month = value->at(0);
			day = value->at(1);
			year = value->at(2);

			// Make sure the values are numerical
			if ( year.find_first_not_of("0123456789") != string::npos ||
				 month.find_first_not_of("0123456789") != string::npos ||
				 day.find_first_not_of("0123456789") != string::npos ){
					if ( dateTimeValueCStr != NULL ){
						free(dateTimeValueCStr);
						dateTimeValueCStr = NULL;
					}
					if ( value != NULL ){
						delete value;
						value = NULL;
					}
					throw Exception("Error: There was an error parsing the date-time value string: \'" + dateTimeValue + "\'. Please refer to the \'" + dateTimeFormatStr + "\' format specification under the DateTimeFormatEnumeration section in the OVAL Element Dictionary.");
			}
			
			// Retrieve the number of seconds since the epoch for this date-time value
			calculatedSeconds = TimeDifferenceFunction::buildTimeStructure(atoi(year.c_str()),atoi(month.c_str()),atoi(day.c_str()));
		}
		// Found date-time value of format mm/dd/yyyy hh:mm:ss or mm-dd-yyyy hh:mm:ss (first item is a number and contains a time component)
		else{
			month = value->at(0);
			day = value->at(1);
			year = value->at(2);
			hours = value->at(3);
			minutes = value->at(4);
			seconds = value->at(5);

			// Make sure the values are numerical
			if ( year.find_first_not_of("0123456789") != string::npos ||
				 month.find_first_not_of("0123456789") != string::npos ||
			     day.find_first_not_of("0123456789") != string::npos ||
			     hours.find_first_not_of("0123456789") != string::npos ||
			     minutes.find_first_not_of("0123456789") != string::npos ||
			     seconds.find_first_not_of("0123456789") != string::npos ){
					if ( dateTimeValueCStr != NULL ){
						free(dateTimeValueCStr);
						dateTimeValueCStr = NULL;
					}
					if ( value != NULL ){
						delete value;
						value = NULL;
					}
	      			throw Exception("Error: There was an error parsing the date-time value string: \'" + dateTimeValue + "\'. Please refer to the \'" + dateTimeFormatStr + "\' format specification under the DateTimeFormatEnumeration section in the OVAL Element Dictionary.");
			}

			// Retrieve the number of seconds since the epoch for this date-time value
			calculatedSeconds = TimeDifferenceFunction::buildTimeStructure(atoi(year.c_str()),atoi(month.c_str()),atoi(day.c_str()),atoi(hours.c_str()),atoi(minutes.c_str()),atoi(seconds.c_str()));
		}
	}
	// Found date-time value of NameOfMonth, dd yyyy hh:mm:ss or NameOfMonth, dd yyyy or AbreviatedNameOfMonth, dd yyyy hh:mm:ss, or AbreviatedNameOfMonth, dd yyyy (first item is a month)
	else{
		int monthValue;
		// Found date-time value of NameOfMonth, dd yyyy or AbreviatedNameOfMonth, dd yyyy (first item is a month and doesn't contain a time component)
		if ( value->size() == 3 ){
			month = value->at(0);
			day = value->at(1);
			year = value->at(2);

			//Make sure the values are numerical
			if ( year.find_first_not_of("0123456789") != string::npos ||
				 day.find_first_not_of("0123456789") != string::npos ){
					if ( dateTimeValueCStr != NULL ){
						free(dateTimeValueCStr);
						dateTimeValueCStr = NULL;
					}
					if ( value != NULL ){
						delete value;
						value = NULL;
					}
					throw Exception("Error: There was an error parsing the date-time value string: \'" + dateTimeValue + "\'. Please refer to the \'" + dateTimeFormatStr + "\' format specification under the DateTimeFormatEnumeration section in the OVAL Element Dictionary.");
			}

			// Convert the name of the month to its corresponding numerical value
			monthValue = TimeDifferenceFunction::MonthStrToInt(month);

			if ( monthValue >= 0 ){
				// Retrieve the number of seconds since the epoch for this date-time value 
				calculatedSeconds = TimeDifferenceFunction::buildTimeStructure(atoi(year.c_str()),monthValue,atoi(day.c_str()),atoi(hours.c_str()),atoi(minutes.c_str()),atoi(seconds.c_str()));
			}else{
				if ( dateTimeValueCStr != NULL ){
						free(dateTimeValueCStr);
						dateTimeValueCStr = NULL;
				}
				if ( value != NULL ){
					delete value;
					value = NULL;
				}
				throw Exception("Error: There was an error parsing the date-time value string: \'" + dateTimeValue + "\'. The month: \'"+month+"\' is an invalid month. Please refer to the \'" + dateTimeFormatStr + "\' format specification under the DateTimeFormatEnumeration section in the OVAL Element Dictionary.");
			}
		}
		// Found date-time value of NameOfMonth, dd yyyy hh:mm:ss or AbreviatedNameOfMonth, dd yyyy hh:mm:ss (first item is a month and contains a time component)
		else{
			month = value->at(0);
			day = value->at(1);
			year = value->at(2);
			hours = value->at(3);
			minutes = value->at(4);
			seconds = value->at(5);

			// Make sure the values are numerical
			if ( year.find_first_not_of("0123456789") != string::npos ||
			     day.find_first_not_of("0123456789") != string::npos ||
			     hours.find_first_not_of("0123456789") != string::npos ||
			     minutes.find_first_not_of("0123456789") != string::npos ||
			     seconds.find_first_not_of("0123456789") != string::npos ){
	      			 if ( dateTimeValueCStr != NULL ){
						free(dateTimeValueCStr);
						dateTimeValueCStr = NULL;
					 }
					 if ( value != NULL ){
						delete value;
						value = NULL;
					 }
					 throw Exception("Error: There was an error parsing the date-time value string: \'" + dateTimeValue + "\'. Please refer to the \'" + dateTimeFormatStr + "\' format specification under the DateTimeFormatEnumeration section in the OVAL Element Dictionary.");
			}
			// Convert the name of the month to its corresponding numerical value
			if ( (monthValue = TimeDifferenceFunction::MonthStrToInt(month)) >= 0 ){
				// Retrieve the number of seconds since the epoch for this date-time value 
				calculatedSeconds = TimeDifferenceFunction::buildTimeStructure(atoi(year.c_str()),monthValue,atoi(day.c_str()),atoi(hours.c_str()),atoi(minutes.c_str()),atoi(seconds.c_str()));
			}else{
				if ( dateTimeValueCStr != NULL ){
					free(dateTimeValueCStr);
					dateTimeValueCStr = NULL;
				}
				if ( value != NULL ){
					delete value;
					value = NULL;
				}
				throw Exception("Error: There was an error parsing the date-time value string: \'" + dateTimeValue + "\'. The month: \'"+month+"\' is an invalid month. Please refer to the \'" + dateTimeFormatStr + "\' format specification under the DateTimeFormatEnumeration section in the OVAL Element Dictionary.");
			}
		}
	}
	if ( dateTimeValueCStr != NULL ){
		free(dateTimeValueCStr);
		dateTimeValueCStr = NULL;
	}
	if ( value != NULL ){
		delete value;
		value = NULL;
	}
	return calculatedSeconds;
}

// Determine the number of seconds since the epoch for date-time values of type day_month_year
time_t TimeDifferenceFunction::DayMonthYearValueToSeconds(string dateTimeValue){
	string dateTimeFormatStr = OvalEnum::DateTimeFormatToString(OvalEnum::DATETIME_DAY_MONTH_YEAR);
	time_t calculatedSeconds;
	string year;
	string month;
	string day;
	string hours;
	string minutes;
	string seconds;
	StringVector* value = new StringVector();
	char * dateTimeValueToken;
	char * dateTimeValueCStr = (char*)malloc(sizeof(char)*(dateTimeValue.length()+1));

	strcpy(dateTimeValueCStr,dateTimeValue.c_str());
	dateTimeValueToken = strtok(dateTimeValueCStr,"/ -:");
	
	while ( dateTimeValueToken != NULL ){
		value->push_back(dateTimeValueToken);
		dateTimeValueToken = strtok(NULL,"/ -:");
	}
	
	// Found date-time value of format dd/mm/yyyy or dd-mm-yyyy (first item is a day and doesn't contain a time component)
	if ( value->size() == 3 ){
		day = value->at(0);
		month = value->at(1);
		year = value->at(2);

		//Make sure the values are numerical
		if ( year.find_first_not_of("0123456789") != string::npos ||
			 month.find_first_not_of("0123456789") != string::npos ||
			 day.find_first_not_of("0123456789") != string::npos ){
			    if ( dateTimeValueCStr != NULL ){
					free(dateTimeValueCStr);
					dateTimeValueCStr = NULL;
				}
				if ( value != NULL ){
					delete value;
					value = NULL;
				}
				throw Exception("Error: There was an error parsing the date-time value string: \'" + dateTimeValue + "\'. Please refer to the \'" + dateTimeFormatStr + "\' format specification under the DateTimeFormatEnumeration section in the OVAL Element Dictionary.");
		}
		// Retrieve the number of seconds since the epoch for this date-time value 
		calculatedSeconds = TimeDifferenceFunction::buildTimeStructure(atoi(year.c_str()),atoi(month.c_str()),atoi(day.c_str()));
	
	// Found date-time value of format dd/mm/yyyy hh:mm:ss or dd-mm-yyyy hh:mm:ss (first item is a day and contains a time component)
	}else{
		day = value->at(0);
		month = value->at(1);
		year = value->at(2);
		hours = value->at(3);
		minutes = value->at(4);
		seconds = value->at(5);

		//Make sure the values are numerical
		if ( year.find_first_not_of("0123456789") != string::npos ||
			 month.find_first_not_of("0123456789") != string::npos ||
		     day.find_first_not_of("0123456789") != string::npos ||
		     hours.find_first_not_of("0123456789") != string::npos ||
		     minutes.find_first_not_of("0123456789") != string::npos ||
		     seconds.find_first_not_of("0123456789") != string::npos ){
      			 if ( dateTimeValueCStr != NULL ){
					free(dateTimeValueCStr);
					dateTimeValueCStr = NULL;
				 }
				 if ( value != NULL ){
					delete value;
					value = NULL;
				 }
				 throw Exception("Error: There was an error parsing the date-time value string: \'" + dateTimeValue + "\'. Please refer to the \'" + dateTimeFormatStr + "\' format specification under the DateTimeFormatEnumeration section in the OVAL Element Dictionary.");
		}
		// Retrieve the number of seconds since the epoch for this date-time value 
		calculatedSeconds = TimeDifferenceFunction::buildTimeStructure(atoi(year.c_str()),atoi(month.c_str()),atoi(day.c_str()),atoi(hours.c_str()),atoi(minutes.c_str()),atoi(seconds.c_str()));
		
	}
	if ( dateTimeValueCStr != NULL ){
		free(dateTimeValueCStr);
		dateTimeValueCStr = NULL;
	}
	if ( value != NULL ){
		delete value;
		value = NULL;
	}
	return calculatedSeconds;
}

// Determine the number of seconds since the epoch for date-time values of type win_filetime
time_t TimeDifferenceFunction::WinFiletimeValueToSeconds(string dateTimeValue){
	
	string dateTimeFormatStr = OvalEnum::DateTimeFormatToString(OvalEnum::DATETIME_WIN_FILETIME);
	time_t calculatedSeconds = time(NULL);
	struct tm * buildTime = localtime(&calculatedSeconds);

	// Retrieve the number of seconds since the epoch for this date-time value 
	calculatedSeconds = (time_t)((TimeDifferenceFunction::HexToDecimal(dateTimeValue)-116444736000000000LL)/10000000);
	
	buildTime = localtime(&calculatedSeconds);

	if ( buildTime == NULL ){
		throw Exception("Error: There was an error processing the date-time value.  Please make sure that the date-time value does not precede the UNIX epoch.");
	}

	// Account for daylight savings time
	if ( buildTime->tm_isdst == 1 ){
		calculatedSeconds = calculatedSeconds - 3600;
		buildTime->tm_isdst = 0;
	}

	buildTime = gmtime(&calculatedSeconds);
	
	if ( buildTime == NULL ){
		throw Exception("Error: There was an error processing the date-time value.  Please make sure that the date-time value does not precede the UNIX epoch.");
	}
	
	calculatedSeconds = mktime(buildTime);
	return calculatedSeconds;
}


// Determine the number of seconds since the epoch for date-time values of type seconds_since_epoch
time_t TimeDifferenceFunction::SecondsSinceEpochValueToSeconds(string dateTimeValue){
	
	string dateTimeFormatStr = OvalEnum::DateTimeFormatToString(OvalEnum::DATETIME_SECONDS_SINCE_EPOCH);
	time_t calculatedSeconds = time(NULL);
	
	//Make sure the value is numerical
	if ( dateTimeValue.find_first_not_of("0123456789") != string::npos ){ 
		throw Exception("Error: There was an error parsing the date-time value string: \'" + dateTimeValue + "\'. Please refer to the \'" + dateTimeFormatStr + "\' format specification under the DateTimeFormatEnumeration section in the OVAL Element Dictionary.");
	}
	
	struct tm * buildTime = localtime(&calculatedSeconds);
	calculatedSeconds = (time_t)atoi(dateTimeValue.c_str());
	buildTime = localtime(&calculatedSeconds);
	
	if ( buildTime == NULL ){
		throw Exception("Error: There was an error processing the date-time value.  Please make sure that the date-time value does not precede the UNIX epoch.");
	}

	// Account for daylight savings time
	if ( buildTime->tm_isdst == 1 ){
		calculatedSeconds = calculatedSeconds - 3600;
		buildTime->tm_isdst = 0;
	}
	
	buildTime = gmtime(&calculatedSeconds);

	if ( buildTime == NULL ){
		throw Exception("Error: There was an error processing the date-time value.  Please make sure that the date-time value does not precede the UNIX epoch.");
	}

	calculatedSeconds = mktime(buildTime);

	return calculatedSeconds;
}

// Determine the number of seconds since the epoch for a given date-time value
time_t TimeDifferenceFunction::buildTimeStructure(int year, int month, int day, int hour, int minute, int second){
	time_t value = time(NULL);
	struct tm * buildTime = localtime(&value);

	if ( buildTime == NULL ){
		throw Exception("Error: There was an error processing the date-time value.  Please make sure that the date-time value does not precede the UNIX epoch.");
	}

	buildTime->tm_year = year - 1900;
	buildTime->tm_mon = month - 1;
	buildTime->tm_mday = day;
	buildTime->tm_hour = hour;
	buildTime->tm_min = minute;
	buildTime->tm_sec = second;

	buildTime->tm_isdst = -1;
	time_t t = mktime(buildTime);
	buildTime = localtime(&t);
	
	if ( buildTime == NULL ){
		throw Exception("Error: There was an error processing the date-time value.  Please make sure that the date-time value does not precede the UNIX epoch.");
	}

	// Account for daylight savings time
	if ( buildTime->tm_isdst == 1 ){
		buildTime->tm_hour = buildTime->tm_hour - 1;	
		buildTime->tm_isdst = 0;
	}

	value = mktime(buildTime);
	return value;
}

// Convert a month into its corresponding numerical value
int TimeDifferenceFunction::MonthStrToInt(string monthValueStr){
	string lowerMonthValueStr;
	char * lowerMonthValueCStr = (char*)malloc(sizeof(char)*(monthValueStr.length()+1));
	int length = (int)monthValueStr.length();
	
	// Convert the string to all lowercase characters
	for(int i=0;i<length;i++){
		lowerMonthValueCStr[i] = toascii(tolower(monthValueStr.at(i)));
	}

	lowerMonthValueCStr[monthValueStr.length()]='\0';
	lowerMonthValueStr = lowerMonthValueCStr;
	
	if ( lowerMonthValueCStr != NULL ){
		free(lowerMonthValueCStr);
		lowerMonthValueCStr = NULL;
	}
		
	if ( lowerMonthValueStr.compare("january") == 0 || lowerMonthValueStr.compare("jan") == 0 ){
		return 1;
	}else if ( lowerMonthValueStr.compare("february") == 0 || lowerMonthValueStr.compare("feb") == 0 ){
		return 2;
	}else if ( lowerMonthValueStr.compare("march") == 0 || lowerMonthValueStr.compare("mar") == 0 ){
		return 3;
	}else if ( lowerMonthValueStr.compare("april") == 0 || lowerMonthValueStr.compare("apr") == 0 ){
		return 4;
	}else if ( lowerMonthValueStr.compare("may") == 0 ){
		return 5;
	}else if ( lowerMonthValueStr.compare("june") == 0 || lowerMonthValueStr.compare("jun") == 0 ){
		return 6;
	}else if ( lowerMonthValueStr.compare("july") == 0 || lowerMonthValueStr.compare("jul") == 0 ){
		return 7;
	}else if ( lowerMonthValueStr.compare("august") == 0 || lowerMonthValueStr.compare("aug") == 0 ){
		return 8;
	}else if ( lowerMonthValueStr.compare("september") == 0 || lowerMonthValueStr.compare("sep") == 0 ){
		return 9;
	}else if ( lowerMonthValueStr.compare("october") == 0 || lowerMonthValueStr.compare("oct") == 0 ){
		return 10;
	}else if ( lowerMonthValueStr.compare("november") == 0 || lowerMonthValueStr.compare("nov") == 0 ){
		return 11;
	}else if ( lowerMonthValueStr.compare("december") == 0 || lowerMonthValueStr.compare("dec") == 0 ){
		return 12;
	}else{
		return -1;
	}
	
}

// Convert a hex string to its corresponding decimal value
long long TimeDifferenceFunction::HexToDecimal(string hexTimeDateStr){
	char * hexCStr = (char*) malloc (sizeof(char)*(hexTimeDateStr.length()+1));
	long long value = 0;
	int length = hexTimeDateStr.length();
	int bitpos = length-1;
	double base = 16;
	strcpy(hexCStr,hexTimeDateStr.c_str());
	for(int i=0;i<length;i++){
		if ( hexCStr[i] >= 48 && hexCStr[i] <= 57 ){
			value = value + (long long)((hexCStr[i]-48)*(pow(base,bitpos)));	
		}else if ( hexCStr[i] == 'a' || hexCStr[i] == 'A' ){
			value = value + (long long)(10*(pow(base,bitpos)));
		}else if ( hexCStr[i] == 'b' || hexCStr[i] == 'B' ){
			value = value + (long long)(11*(pow(base,bitpos)));
		}else if ( hexCStr[i] == 'c' || hexCStr[i] == 'C' ){
			value = value + (long long)(12*(pow(base,bitpos)));
		}else if ( hexCStr[i] == 'd' || hexCStr[i] == 'D' ){
			value = value + (long long)(13*(pow(base,bitpos)));
		}else if ( hexCStr[i] == 'e' || hexCStr[i] == 'E' ){
			value = value + (long long)(14*(pow(base,bitpos)));
		}else if ( hexCStr[i] == 'f' || hexCStr[i] == 'F' ){
			value = value + (long long)(15*(pow(base,bitpos)));
		}else{
			value = -1;
			break;
		}
		bitpos = bitpos-1;
	}
	if ( hexCStr != NULL ){
		free(hexCStr);
		hexCStr = NULL;
	}
	return value;
}


