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

#include "OvalEnum.h"


//****************************************************************************************//
//									OvalEnum Class										  //	
//****************************************************************************************//

string OvalEnum::ArithmeticOperationToString(OvalEnum::ArithmeticOperation arithOp) {

	string arithStr = "";

	switch(arithOp) {
		case (ARITHMETIC_ADD):
			arithStr = "add";
			break;
		case (ARITHMETIC_MULTIPLY):
			arithStr = "multiply";
			break;
		default:	
			throw Exception("OvalEnum::ArithmeticOperationToString - Error unsupported arithmetic operation value.");
			break;
	}

	return arithStr;
}

OvalEnum::ArithmeticOperation OvalEnum::ToArithmeticOperation(string arithStr) {

	OvalEnum::ArithmeticOperation arithOp;
	
	if(arithStr.compare(OvalEnum::ArithmeticOperationToString(ARITHMETIC_ADD)) == 0) {
		arithOp = ARITHMETIC_ADD;
	} else if(arithStr.compare(OvalEnum::ArithmeticOperationToString(ARITHMETIC_MULTIPLY)) == 0) {
		arithOp = ARITHMETIC_MULTIPLY;
	} else {
		throw Exception("OvalEnum::ToArithmeticOperation - Error unsupported arithmetic operation value: " + arithStr);
	}

	return arithOp;
}

string OvalEnum::CheckToString(OvalEnum::Check check) {

	string checkStr = "";

	switch(check) {
		case (CHECK_ALL):
			checkStr = "all";
			break;
		case (CHECK_AT_LEAST_ONE):
			checkStr = "at least one";
			break;
		case (CHECK_NONE_EXIST):
			checkStr = "none exist";
			break;
		case (CHECK_NONE_SATISFY):
			checkStr = "none satisfy";
			break;
		case (CHECK_ONLY_ONE):
			checkStr = "only one";
			break;
		default:	
			throw Exception("OvalEnum::CheckToString - Error unsupported check value.");
			break;
	}

	return checkStr;
}

OvalEnum::Check OvalEnum::ToCheck(string checkStr) {

	OvalEnum::Check check;
	
	if(checkStr.compare(OvalEnum::CheckToString(CHECK_ALL)) == 0 || checkStr.compare("") == 0) {
		check = CHECK_ALL;
	} else if(checkStr.compare(OvalEnum::CheckToString(CHECK_AT_LEAST_ONE)) == 0) {
		check = CHECK_AT_LEAST_ONE;
	} else if(checkStr.compare(OvalEnum::CheckToString(CHECK_NONE_EXIST)) == 0) {
		check = CHECK_NONE_EXIST;
	} else if(checkStr.compare(OvalEnum::CheckToString(CHECK_NONE_SATISFY)) == 0) {
		check = CHECK_NONE_SATISFY;
	} else if(checkStr.compare(OvalEnum::CheckToString(CHECK_ONLY_ONE)) == 0) {
		check = CHECK_ONLY_ONE;
	} else {
		throw Exception("OvalEnum::ToCheck - Error unsupported check value: " + checkStr);
	}

	return check;
}

string OvalEnum::DateTimeFormatToString(OvalEnum::DateTimeFormat dateTimeFormat) {

	string dateTimeFormatStr = "";

	switch(dateTimeFormat) {
		case (DATETIME_YEAR_MONTH_DAY):
			dateTimeFormatStr = "year_month_day";
			break;
		case (DATETIME_MONTH_DAY_YEAR):
			dateTimeFormatStr = "month_day_year";
			break;
		case (DATETIME_DAY_MONTH_YEAR):
			dateTimeFormatStr = "day_month_year";
			break;
		case (DATETIME_WIN_FILETIME):
			dateTimeFormatStr = "win_filetime";
			break;
		case (DATETIME_SECONDS_SINCE_EPOCH):
			dateTimeFormatStr = "seconds_since_epoch";
			break;
		default:	
			throw Exception("OvalEnum::DateTimeFormatToString - Error unsupported data time format value.");
			break;
	}

	return dateTimeFormatStr;
}

OvalEnum::DateTimeFormat OvalEnum::ToDateTimeFormat(string dateTimeFormatStr) {

	OvalEnum::DateTimeFormat dateTimeFormat;
	
	if(dateTimeFormatStr.compare(OvalEnum::DateTimeFormatToString(DATETIME_YEAR_MONTH_DAY)) == 0 || dateTimeFormatStr.compare("") == 0) {
		dateTimeFormat = DATETIME_YEAR_MONTH_DAY;
	} else if(dateTimeFormatStr.compare(OvalEnum::DateTimeFormatToString(DATETIME_MONTH_DAY_YEAR)) == 0) {
		dateTimeFormat = DATETIME_MONTH_DAY_YEAR;
	} else if(dateTimeFormatStr.compare(OvalEnum::DateTimeFormatToString(DATETIME_DAY_MONTH_YEAR)) == 0) {
		dateTimeFormat = DATETIME_DAY_MONTH_YEAR;
	} else if(dateTimeFormatStr.compare(OvalEnum::DateTimeFormatToString(DATETIME_WIN_FILETIME)) == 0) {
		dateTimeFormat = DATETIME_WIN_FILETIME;
	} else if(dateTimeFormatStr.compare(OvalEnum::DateTimeFormatToString(DATETIME_SECONDS_SINCE_EPOCH)) == 0) {
		dateTimeFormat = DATETIME_SECONDS_SINCE_EPOCH;
	} else {
		throw Exception("OvalEnum::ToDateTimeFormat - Error unsupported date time format value: " + dateTimeFormatStr);
	}

	return dateTimeFormat;
}

string OvalEnum::ExistenceToString(OvalEnum::Existence existence) {

	string existenceStr = "";

	switch(existence) {
		case (EXISTENCE_ALL_EXIST):
			existenceStr = "all_exist";
			break;
		case (EXISTENCE_ANY_EXIST):
			existenceStr = "any_exist";
			break;
		case (EXISTENCE_AT_LEAST_ONE_EXISTS):
			existenceStr = "at_least_one_exists";
			break;
		case (EXISTENCE_NONE_EXIST):
			existenceStr = "none_exist";
			break;
		case (EXISTENCE_ONLY_ONE_EXISTS):
			existenceStr = "only_one_exists";
			break;
		default:	
			throw Exception("OvalEnum::ExistenceToString - Error unsupported existence value.");
			break;
	}

	return existenceStr;
}

OvalEnum::Existence OvalEnum::ToExistence(string existenceStr) {

	OvalEnum::Existence existence;
	
	if(existenceStr.compare(OvalEnum::ExistenceToString(EXISTENCE_ALL_EXIST)) == 0 || existenceStr.compare("") == 0) {
		existence = EXISTENCE_ALL_EXIST;
	} else if(existenceStr.compare(OvalEnum::ExistenceToString(EXISTENCE_ANY_EXIST)) == 0) {
		existence = EXISTENCE_ANY_EXIST;
	} else if(existenceStr.compare(OvalEnum::ExistenceToString(EXISTENCE_AT_LEAST_ONE_EXISTS)) == 0) {
		existence = EXISTENCE_AT_LEAST_ONE_EXISTS;
	} else if(existenceStr.compare(OvalEnum::ExistenceToString(EXISTENCE_NONE_EXIST)) == 0) {
		existence = EXISTENCE_NONE_EXIST;
	} else if(existenceStr.compare(OvalEnum::ExistenceToString(EXISTENCE_ONLY_ONE_EXISTS)) == 0) {
		existence = EXISTENCE_ONLY_ONE_EXISTS;
	} else {
		throw Exception("OvalEnum::ToCheck - Error unsupported check value: " + existenceStr);
	}

	return existence;
}

string OvalEnum::DatatypeToString(OvalEnum::Datatype datatype){
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Conver the Datatype value to a string
	// -----------------------------------------------------------------------

	string datatypeStr = "";

	switch(datatype) {
		case (DATATYPE_STRING):
			datatypeStr = "string";
			break;
		case (DATATYPE_INTEGER):
			datatypeStr = "int";
			break;
		case (DATATYPE_IOS_VERSION):
			datatypeStr = "ios_version";
			break;			
		case (DATATYPE_BINARY):
			datatypeStr = "binary";
			break;
		case (DATATYPE_BOOLEAN):
			datatypeStr = "boolean";
			break;
		case (DATATYPE_EVR_STRING):
			datatypeStr = "evr_string";
			break;
		case (DATATYPE_FLOAT):
			datatypeStr = "float";
			break;
		case (DATATYPE_VERSION):
			datatypeStr = "version";
			break;
		default:	
			throw Exception("OvalEnum::DatatypeToString - Error unsupported datatype.");
			break;
	}

	return datatypeStr;
}

OvalEnum::Datatype OvalEnum::ToDatatype(string datatypeStr){
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Convert the string to a Datatype
	//
	// -----------------------------------------------------------------------

	OvalEnum::Datatype datatype;
	
	if(datatypeStr.compare(OvalEnum::DatatypeToString(DATATYPE_STRING)) == 0 || datatypeStr.compare("") == 0) {
		datatype = DATATYPE_STRING;
	} else if(datatypeStr.compare(OvalEnum::DatatypeToString(DATATYPE_INTEGER)) == 0) {
		datatype = DATATYPE_INTEGER;
	} else if(datatypeStr.compare(OvalEnum::DatatypeToString(DATATYPE_IOS_VERSION)) == 0) {
		datatype = DATATYPE_IOS_VERSION;
	} else if(datatypeStr.compare(OvalEnum::DatatypeToString(DATATYPE_BINARY)) == 0) {
		datatype = DATATYPE_BINARY;
	} else if(datatypeStr.compare(OvalEnum::DatatypeToString(DATATYPE_BOOLEAN)) == 0) {
		datatype = DATATYPE_BOOLEAN;
	} else if(datatypeStr.compare(OvalEnum::DatatypeToString(DATATYPE_EVR_STRING)) == 0) {
		datatype = DATATYPE_EVR_STRING;
	} else if(datatypeStr.compare(OvalEnum::DatatypeToString(DATATYPE_FLOAT)) == 0) {
		datatype = DATATYPE_FLOAT;
	} else if(datatypeStr.compare(OvalEnum::DatatypeToString(DATATYPE_VERSION)) == 0) {
		datatype = DATATYPE_VERSION;
	} else {
		throw Exception("OvalEnum::ToDatatype - Error unsupported datatype value: " + datatypeStr);
	}

	return datatype;
}

string OvalEnum::FlagToString(OvalEnum::Flag flag) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Convert the Flag value to a string
	//
	// -----------------------------------------------------------------------
	string flagStr = "";

	switch(flag) {
		case (FLAG_ERROR):
			flagStr = "error";
			break;
		case (FLAG_COMPLETE):
			flagStr = "complete";
			break;
		case (FLAG_INCOMPLETE):
			flagStr = "incomplete";
			break;
		case (FLAG_DOES_NOT_EXIST):
			flagStr = "does not exist";
			break;
		case (FLAG_NOT_COLLECTED):
			flagStr = "not collected";
			break;
		case (FLAG_NOT_APPLICABLE):
			flagStr = "not applicable";
			break;
		default:
			throw Exception("OvalEnum::FlagToString - Error unsupported flag value.");
			break;
	}
	return flagStr;
}

OvalEnum::Flag OvalEnum::ToFlag(string flagStr){
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Convert the string to a CollectedObjectFlag value
	// -----------------------------------------------------------------------

	OvalEnum::Flag flag;
	
	if(flagStr.compare(OvalEnum::FlagToString(FLAG_COMPLETE)) == 0 || flagStr.compare("") == 0) {
		flag = FLAG_COMPLETE;
	} else if(flagStr.compare(OvalEnum::FlagToString(FLAG_ERROR)) == 0) {
		flag = FLAG_ERROR;
	} else if(flagStr.compare(OvalEnum::FlagToString(FLAG_INCOMPLETE)) == 0) {
		flag = FLAG_INCOMPLETE;
	} else if(flagStr.compare(OvalEnum::FlagToString(FLAG_DOES_NOT_EXIST)) == 0) {
		flag = FLAG_DOES_NOT_EXIST;
	} else if(flagStr.compare(OvalEnum::FlagToString(FLAG_NOT_COLLECTED)) == 0) {
		flag = FLAG_NOT_COLLECTED;
	} else if(flagStr.compare(OvalEnum::FlagToString(FLAG_NOT_APPLICABLE)) == 0) {
		flag = FLAG_NOT_APPLICABLE;
	} else {
		throw Exception("OvalEnum::ToFlag - Error unsupported flag value: " + flagStr);
	}

	return flag;
}

OvalEnum::Flag OvalEnum::CombineFlags(IntVector* flags) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Combine the set of flag values to a single flag value.
	// -----------------------------------------------------------------------

	OvalEnum::Flag combinedFlag = OvalEnum::FLAG_ERROR;

	// Make sure the flags vector has items. Return error flag if it is empty
	if(flags->size() == 0) {
		return combinedFlag;
	}

	// Get counts of flag values
	int errorCount = 0;
	int completeCount = 0;
	int incompleteCount = 0;
	int doesNotExistCount = 0;
	int notCollectedCount = 0;
	int notApplicableCount = 0;
	IntVector::iterator flag;
	for (flag=flags->begin(); flag!=flags->end(); flag++) {

		if((*flag) == OvalEnum::FLAG_ERROR) {
			errorCount++;
		} else if((*flag) == OvalEnum::FLAG_COMPLETE) {
			completeCount++;
		} else if((*flag) == OvalEnum::FLAG_INCOMPLETE) {
			incompleteCount++;
		} else if((*flag) == OvalEnum::FLAG_DOES_NOT_EXIST) {
			doesNotExistCount++;
		} else if((*flag) == OvalEnum::FLAG_NOT_COLLECTED) {
			notCollectedCount++;
		} else if((*flag) == OvalEnum::FLAG_NOT_APPLICABLE) {
			notApplicableCount++;
		} 
	}

	if(errorCount > 0) {
		combinedFlag = OvalEnum::FLAG_ERROR;
	} else if(errorCount == 0 
		&& completeCount > 0 
		&& incompleteCount == 0 
		&& doesNotExistCount == 0 
		&& notCollectedCount == 0 
		&& notApplicableCount == 0) {

		combinedFlag = OvalEnum::FLAG_COMPLETE;

	} else if(errorCount == 0 
		&& completeCount > 0 
		&& incompleteCount == 0 
		&& doesNotExistCount == 0 
		&& notCollectedCount == 0 
		&& notApplicableCount == 0) {

		combinedFlag = OvalEnum::FLAG_INCOMPLETE;

	} else if(errorCount == 0 
		&& completeCount >= 0 
		&&  incompleteCount >= 0 
		&& doesNotExistCount > 0 
		&& notCollectedCount == 0 
		&& notApplicableCount == 0) {

		combinedFlag = OvalEnum::FLAG_DOES_NOT_EXIST;

	} else if(errorCount == 0 
		&& completeCount >= 0 
		&&  incompleteCount >= 0 
		&& doesNotExistCount >= 0 
		&& notCollectedCount > 0 
		&& notApplicableCount == 0) {

		combinedFlag = OvalEnum::FLAG_NOT_COLLECTED;

	} else if(errorCount == 0 
		&& completeCount >= 0 
		&&  incompleteCount >= 0 
		&& doesNotExistCount >= 0 
		&& notCollectedCount >= 0 
		&& notApplicableCount > 0) {

		combinedFlag = OvalEnum::FLAG_NOT_APPLICABLE;

	}	

	return combinedFlag;
}

string OvalEnum::LevelToString(OvalEnum::Level level){
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Convert the Level value to a string
	// -----------------------------------------------------------------------

	string levelStr = "";

	switch(level) {
		case (LEVEL_DEBUG):
			levelStr = "debug";
			break;
		case (LEVEL_ERROR):
			levelStr = "error";
			break;
		case (LEVEL_FATAL):
			levelStr = "fatal";
			break;
		case (LEVEL_INFO):
			levelStr = "info";
			break;
		case (LEVEL_WARNING):
			levelStr = "warning";
			break;
		default:	
			throw Exception("OvalEnum::LevelToString - Error unsupported level.");
			break;
	};

	return levelStr;
}

OvalEnum::Level OvalEnum::ToLevel(string levelStr){
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Convert the string to a Datatype
	//
	// -----------------------------------------------------------------------

	Level level;
	
	if(levelStr.compare(OvalEnum::LevelToString(LEVEL_INFO)) == 0 || levelStr.compare("") == 0) {
		level = LEVEL_INFO;
	} else if(levelStr.compare(OvalEnum::LevelToString(LEVEL_DEBUG)) == 0) {
		level = LEVEL_DEBUG;
	} else if(levelStr.compare(OvalEnum::LevelToString(LEVEL_ERROR)) == 0) {
		level = LEVEL_ERROR;
	} else if(levelStr.compare(OvalEnum::LevelToString(LEVEL_FATAL)) == 0) {
		level = LEVEL_FATAL;
	} else if(levelStr.compare(OvalEnum::LevelToString(LEVEL_WARNING)) == 0) {
		level = LEVEL_WARNING;
	} else {
		throw Exception("OvalEnum::Tolevel - Error unsupported level value: " + levelStr);
	}

	return level;
}

string OvalEnum::OperationToString(OvalEnum::Operation operation){
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Convert the Operation value to a string
	//
	// -----------------------------------------------------------------------
	string operationStr = "";

	switch(operation) {
		case (OPERATION_EQUALS):
			operationStr = "equals";
			break;
		case (OPERATION_NOT_EQUAL):
			operationStr = "not equal";
			break;
		case (OPERATION_GREATER_THAN):
			operationStr = "greater than";
			break;
		case (OPERATION_LESS_THAN):
			operationStr = "less than";
			break;
		case (OPERATION_GREATER_THAN_OR_EQUAL):
			operationStr = "greater than or equal";
			break;
		case (OPERATION_LESS_THAN_OR_EQUAL):
			operationStr = "less than or equal";
			break;
		case (OPERATION_BITWISE_AND):
			operationStr = "bitwise and";
			break;
		case (OPERATION_BITWISE_OR):
			operationStr = "bitwise or";
			break;
		case (OPERATION_PATTERN_MATCH):
			operationStr = "pattern match";
			break;
        /* LS Start */
		case (OPERATION_CASE_INSENSITIVE_EQUALS):
			operationStr = "case insensitive equals";
			break;
		case (OPERATION_CASE_INSENSITIVE_NOT_EQUAL):
			operationStr = "case insensitive not equal";
			break;
        /* LS End */
		default:
			throw Exception("OvalEnum::OperationToString - Error unsupported operation value.");
			break;
	}
	return operationStr;
}

OvalEnum::Operation OvalEnum::ToOperation(string operationStr){
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Convert the string to a Operation value
	// -----------------------------------------------------------------------

	OvalEnum::Operation operation;
	
	if(operationStr.compare(OvalEnum::OperationToString(OPERATION_EQUALS)) == 0 || operationStr.compare("") == 0) {
		operation = OPERATION_EQUALS;
	} else if(operationStr.compare(OvalEnum::OperationToString(OPERATION_NOT_EQUAL)) == 0) {
		operation = OPERATION_NOT_EQUAL;
	} else if(operationStr.compare(OvalEnum::OperationToString(OPERATION_GREATER_THAN)) == 0) {
		operation = OPERATION_GREATER_THAN;
	} else if(operationStr.compare(OvalEnum::OperationToString(OPERATION_LESS_THAN)) == 0) {
		operation = OPERATION_LESS_THAN;
	} else if(operationStr.compare(OvalEnum::OperationToString(OPERATION_GREATER_THAN_OR_EQUAL)) == 0) {
		operation = OPERATION_GREATER_THAN_OR_EQUAL;
	} else if(operationStr.compare(OvalEnum::OperationToString(OPERATION_LESS_THAN_OR_EQUAL)) == 0) {
		operation = OPERATION_LESS_THAN_OR_EQUAL;
	} else if(operationStr.compare(OvalEnum::OperationToString(OPERATION_BITWISE_AND)) == 0) {
		operation = OPERATION_BITWISE_AND;
	} else if(operationStr.compare(OvalEnum::OperationToString(OPERATION_BITWISE_OR)) == 0) {
		operation = OPERATION_BITWISE_OR;
	} else if(operationStr.compare(OvalEnum::OperationToString(OPERATION_PATTERN_MATCH)) == 0) {
		operation = OPERATION_PATTERN_MATCH;
	} else if(operationStr.compare(OvalEnum::OperationToString(OPERATION_CASE_INSENSITIVE_EQUALS)) == 0) {
		operation = OPERATION_CASE_INSENSITIVE_EQUALS;
	} else if(operationStr.compare(OvalEnum::OperationToString(OPERATION_CASE_INSENSITIVE_NOT_EQUAL)) == 0) {
		operation = OPERATION_CASE_INSENSITIVE_NOT_EQUAL;
	} else {
		throw Exception("OvalEnum::ToOperation - Error unsupported operation value: " + operationStr);
	}

	return operation;
}

OvalEnum::Operator OvalEnum::ToOperator(string operatorStr) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Convert the string to an Operator
	//
	// -----------------------------------------------------------------------

	OvalEnum::Operator op;
	
	if(operatorStr.compare(OvalEnum::OperatorToString(OPERATOR_AND)) == 0 || operatorStr.compare("") == 0) {
		op = OPERATOR_AND;
	} else if(operatorStr.compare(OvalEnum::OperatorToString(OPERATOR_ONE)) == 0) {
		op = OPERATOR_ONE;
	} else if(operatorStr.compare(OvalEnum::OperatorToString(OPERATOR_OR)) == 0) {
		op = OPERATOR_OR;
	} else if(operatorStr.compare(OvalEnum::OperatorToString(OPERATOR_XOR)) == 0) {
		op = OPERATOR_XOR;
	} else {
		throw Exception("OvalEnum::ToOperator - Error unsupported operator value: " + operatorStr);
	}

	return op;	
}

string OvalEnum::OperatorToString(OvalEnum::Operator op) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	convert the Operator to a string
	//
	// -----------------------------------------------------------------------
	string operatorStr = "";

	switch(op) {
		case (OPERATOR_AND):
			operatorStr = "AND";
			break;
		case (OPERATOR_ONE):
			operatorStr = "ONE";
			break;
		case (OPERATOR_OR):
			operatorStr = "OR";
			break;
		case (OPERATOR_XOR):
			operatorStr = "XOR";
			break;
		default:
			throw Exception("OvalEnum::OperationToString - Error unsupported operator value.");
			break;
	}
	return operatorStr;
}

OvalEnum::ResultEnumeration OvalEnum::ToResult(string resultStr) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Convert the string to an Result
	//
	// -----------------------------------------------------------------------

	OvalEnum::ResultEnumeration result;
	
	if(resultStr.compare(OvalEnum::ResultToString(RESULT_TRUE)) == 0) {
		result = RESULT_TRUE;
	} else if(resultStr.compare(OvalEnum::ResultToString(RESULT_FALSE)) == 0) {
		result = RESULT_FALSE;
	} else if(resultStr.compare(OvalEnum::ResultToString(RESULT_UNKNOWN)) == 0) {
		result = RESULT_UNKNOWN;
	} else if(resultStr.compare(OvalEnum::ResultToString(RESULT_ERROR)) == 0) {
		result = RESULT_ERROR;
	} else if(resultStr.compare(OvalEnum::ResultToString(RESULT_NOT_EVALUATED)) == 0) {
		result = RESULT_NOT_EVALUATED;
	} else if(resultStr.compare(OvalEnum::ResultToString(RESULT_NOT_APPLICABLE)) == 0) {
		result = RESULT_NOT_APPLICABLE;
	} else {
		throw Exception("OvalEnum::ToOperator - Error unsupported result value: " + resultStr);
	}

	return result;	
}

string OvalEnum::ResultToString(OvalEnum::ResultEnumeration result) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	convert the ResultEnumeration to a string
	//
	// -----------------------------------------------------------------------
	string resultStr = "";

	switch(result) {
		case (RESULT_TRUE):
			resultStr = "true";
			break;
		case (RESULT_FALSE):
			resultStr = "false";
			break;
		case (RESULT_UNKNOWN):
			resultStr = "unknown";
			break;
		case (RESULT_ERROR):
			resultStr = "error";
			break;
		case (RESULT_NOT_EVALUATED):
			resultStr = "not evaluated";
			break;
		case (RESULT_NOT_APPLICABLE):
			resultStr = "not applicable";
			break;
		default:
			throw Exception("OvalEnum::ResultEnumeration - Error unsupported result value.");
			break;
	}
	return resultStr;
}

OvalEnum::ResultEnumeration OvalEnum::CombineResultsByCheck(IntVector* results, OvalEnum::Check check) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return a result value based on combining the results in the 
	//	provided array and considering the provided check value
	//
	//	This function assigns precidence to the non t/f result values as follows
	//		- 1 - error
	//		- 2 - unknown
	//		- 3 - not evaluated
	// -----------------------------------------------------------------------

	OvalEnum::ResultEnumeration combinedResult = RESULT_UNKNOWN;

	// Make sure the results vector has items. Return unknown_result if it is empty
	if(results->size() == 0) {
		return RESULT_UNKNOWN;
	}

	// Get counts of result values
	int trueCount = 0;
	int falseCount = 0;
	int unknownCount = 0;
	int errorCount = 0;
	int notEvaluatedCount = 0;
	int notApplicableCount = 0;
	IntVector::iterator result;
	for (result=results->begin(); result!=results->end(); result++) {

		if((*result) == OvalEnum::RESULT_TRUE) {
			trueCount++;
		} else if((*result) == OvalEnum::RESULT_FALSE) {
			falseCount++;
		} else if((*result) == OvalEnum::RESULT_UNKNOWN) {
			unknownCount++;
		} else if((*result) == OvalEnum::RESULT_ERROR) {
			errorCount++;
		} else if((*result) == OvalEnum::RESULT_NOT_EVALUATED) {
			notEvaluatedCount++;
		} else if((*result) == OvalEnum::RESULT_NOT_APPLICABLE) {
			notApplicableCount++;
		} 
	}

	// first check for a possible Not Applicable result
	if(notApplicableCount > 0 && notEvaluatedCount == 0 && falseCount == 0 && errorCount == 0 && unknownCount == 0 && trueCount == 0) {
		return OvalEnum::RESULT_NOT_APPLICABLE;
	}

	// Set the combined result
	if(check == OvalEnum::CHECK_ALL) {
		if(trueCount > 0 && falseCount == 0 && errorCount == 0 && unknownCount == 0 && notEvaluatedCount == 0) {
			combinedResult = OvalEnum::RESULT_TRUE;
		} else if(falseCount > 0) {
			combinedResult = OvalEnum::RESULT_FALSE;
		} else if(falseCount == 0 && errorCount > 0) {
			combinedResult = OvalEnum::RESULT_ERROR;
		} else if(unknownCount > 0 && falseCount == 0 && errorCount == 0) {
			combinedResult = OvalEnum::RESULT_UNKNOWN;
		} else if(notEvaluatedCount > 0 && falseCount == 0 && errorCount == 0 && unknownCount == 0) {
			combinedResult = OvalEnum::RESULT_NOT_EVALUATED;
		}
	} else if(check == OvalEnum::CHECK_AT_LEAST_ONE) {
		if(trueCount > 0) {
			combinedResult = OvalEnum::RESULT_TRUE;
		} else if(falseCount > 0 && trueCount == 0 && unknownCount == 0 &&  errorCount == 0 &&  notEvaluatedCount == 0) {
			combinedResult = OvalEnum::RESULT_FALSE;
		} else if(errorCount > 0 && trueCount == 0) {
			combinedResult = OvalEnum::RESULT_ERROR;
		} else if(unknownCount > 0 && trueCount == 0 && errorCount == 0) {
			combinedResult = OvalEnum::RESULT_UNKNOWN;
		} else if(notEvaluatedCount > 0 && unknownCount == 0 && trueCount == 0 && errorCount == 0) {
			combinedResult = OvalEnum::RESULT_NOT_EVALUATED;
		}
	} else if(check == OvalEnum::CHECK_NONE_SATISFY) {
		if(trueCount > 0) {
			combinedResult = OvalEnum::RESULT_FALSE;
		} else if(errorCount > 0 && trueCount == 0) {	
			combinedResult = OvalEnum::RESULT_ERROR;
		} else if(unknownCount > 0 && errorCount == 0 && trueCount == 0) {	
			combinedResult = OvalEnum::RESULT_UNKNOWN;
		} else if(notEvaluatedCount > 0 && unknownCount == 0 && errorCount == 0 && trueCount == 0) {	
			combinedResult = OvalEnum::RESULT_NOT_EVALUATED;
		} else if(falseCount > 0 && notEvaluatedCount == 0 && unknownCount == 0 && errorCount == 0 && trueCount == 0) {	
			combinedResult = OvalEnum::RESULT_TRUE;
		}
	} else if(check == OvalEnum::CHECK_ONLY_ONE) {
		if(trueCount == 1 && unknownCount == 0 && errorCount == 0 && notEvaluatedCount == 0) {
			combinedResult = OvalEnum::RESULT_TRUE;
		} else if(trueCount > 1) {
			combinedResult = OvalEnum::RESULT_FALSE;
		} else if(errorCount > 0 && trueCount < 2) {
			combinedResult = OvalEnum::RESULT_ERROR;
		} else if(unknownCount > 0 && errorCount == 0 && trueCount < 2) {
			combinedResult = OvalEnum::RESULT_UNKNOWN;
		} else if(notEvaluatedCount > 0 && unknownCount == 0 && errorCount == 0 && trueCount < 2) {
			combinedResult = OvalEnum::RESULT_NOT_EVALUATED;
		} else if(falseCount > 0 && trueCount != 1) {
            combinedResult = OvalEnum::RESULT_FALSE;
		}
	}

	return combinedResult;
}

OvalEnum::ResultEnumeration OvalEnum::CombineResultsByOperator(IntVector* results, OvalEnum::Operator op) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Return a result value based on combining the results in the 
	//	provided array and considering the provided operation value
	//
	//	This function assigns precidence to the non t/f result values as follows
	//		- 1 - error
	//		- 2 - unknown
	//		- 3 - not evaluated
	// -----------------------------------------------------------------------

	OvalEnum::ResultEnumeration combinedResult = RESULT_UNKNOWN;

	// Make sure the results vector has items. Return unknown_result if it is empty
	if(results->size() == 0) {
		return RESULT_UNKNOWN;
	}

	// Get counts of result values
	int trueCount = 0;
	int falseCount = 0;
	int unknownCount = 0;
	int errorCount = 0;
	int notEvaluatedCount = 0;
	int notApplicableCount = 0;
	IntVector::iterator result;
	for (result=results->begin(); result!=results->end(); result++) {

		if((*result) == OvalEnum::RESULT_TRUE) {
			trueCount++;
		} else if((*result) == OvalEnum::RESULT_FALSE) {
			falseCount++;
		} else if((*result) == OvalEnum::RESULT_UNKNOWN) {
			unknownCount++;
		} else if((*result) == OvalEnum::RESULT_ERROR) {
			errorCount++;
		} else if((*result) == OvalEnum::RESULT_NOT_EVALUATED) {
			notEvaluatedCount++;
		} else if((*result) == OvalEnum::RESULT_NOT_APPLICABLE) {
			notApplicableCount++;
		} 
	}

	// first check for a possible Not Applicable result
	if(notApplicableCount > 0 && notEvaluatedCount == 0 && falseCount == 0 && errorCount == 0 && unknownCount == 0 && trueCount == 0) {
		return OvalEnum::RESULT_NOT_APPLICABLE;
	}

	// Set the combined result
	if(op == OvalEnum::OPERATOR_AND) {
		if(trueCount > 0 && falseCount == 0 && errorCount == 0 && unknownCount == 0 && notEvaluatedCount == 0) {
			combinedResult = OvalEnum::RESULT_TRUE;
		} else if(falseCount > 0) {
			combinedResult = OvalEnum::RESULT_FALSE;
		} else if(falseCount == 0 && errorCount > 0) {
			combinedResult = OvalEnum::RESULT_ERROR;
		} else if(unknownCount > 0 && falseCount == 0 && errorCount == 0) {
			combinedResult = OvalEnum::RESULT_UNKNOWN;
		} else if(notEvaluatedCount > 0 && falseCount == 0 && errorCount == 0 && unknownCount == 0) {
			combinedResult = OvalEnum::RESULT_NOT_EVALUATED;
		}
	} else if(op == OvalEnum::OPERATOR_ONE) {

		if(trueCount == 1 && falseCount >= 0 && errorCount == 0 && unknownCount == 0 && notEvaluatedCount == 0 && notApplicableCount >= 0) {
			combinedResult = OvalEnum::RESULT_TRUE;
		} else if(trueCount >= 2 && falseCount >= 0 && errorCount >= 0 && unknownCount >= 0 && notEvaluatedCount >= 0 && notApplicableCount >= 0) {
			combinedResult = OvalEnum::RESULT_FALSE;
		} else if(trueCount == 0 && falseCount >= 0 && errorCount == 0 && unknownCount == 0 && notEvaluatedCount == 0 && notApplicableCount >= 0) {
			combinedResult = OvalEnum::RESULT_FALSE;
		} else if((trueCount == 0 || trueCount == 1) && falseCount >= 0 && errorCount >= 1 && unknownCount >= 0 && notEvaluatedCount >= 0 && notApplicableCount >= 0) {
			combinedResult = OvalEnum::RESULT_ERROR;
		} else if((trueCount == 0 || trueCount == 1) && falseCount >= 0 && errorCount == 0 && unknownCount >= 1 && notEvaluatedCount >= 0 && notApplicableCount >= 0) {
			combinedResult = OvalEnum::RESULT_UNKNOWN;
		} else if((trueCount == 0 || trueCount == 1) && falseCount >= 0 && errorCount == 0 && unknownCount == 0&& notEvaluatedCount >= 1 && notApplicableCount >= 0) {
			combinedResult = OvalEnum::RESULT_NOT_EVALUATED;
		}
	} else if(op == OvalEnum::OPERATOR_OR) {
		if(trueCount > 0) {
			combinedResult = OvalEnum::RESULT_TRUE;
		} else if(falseCount > 0 && trueCount == 0 && unknownCount == 0 &&  errorCount == 0 &&  notEvaluatedCount == 0) {
			combinedResult = OvalEnum::RESULT_FALSE;
		} else if(errorCount > 0 && trueCount == 0) {
			combinedResult = OvalEnum::RESULT_ERROR;
		} else if(unknownCount > 0 && trueCount == 0 && errorCount == 0) {
			combinedResult = OvalEnum::RESULT_UNKNOWN;
		} else if(notEvaluatedCount > 0 && unknownCount == 0 && trueCount == 0 && errorCount == 0) {
			combinedResult = OvalEnum::RESULT_NOT_EVALUATED;
		}
	} else if(op == OvalEnum::OPERATOR_XOR) {
		if(trueCount%2 == 1 && notEvaluatedCount == 0 && unknownCount == 0 && errorCount == 0) {
			combinedResult = OvalEnum::RESULT_TRUE;
		} else if(trueCount%2 == 0 && notEvaluatedCount == 0 && unknownCount == 0 && errorCount == 0) {	
			combinedResult = OvalEnum::RESULT_FALSE;
		} else if(errorCount > 0) {	
			combinedResult = OvalEnum::RESULT_ERROR;
		} else if(unknownCount > 0 && errorCount == 0) {	
			combinedResult = OvalEnum::RESULT_UNKNOWN;
		} else if(notEvaluatedCount > 0 && unknownCount == 0 && errorCount == 00) {	
			combinedResult = OvalEnum::RESULT_NOT_EVALUATED;
		}
	}

	return combinedResult;
}

OvalEnum::ResultEnumeration OvalEnum::NegateResult(OvalEnum::ResultEnumeration result) {
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	negate the result value and return it
	//
	// -----------------------------------------------------------------------

	if(result == OvalEnum::RESULT_TRUE) {
		return OvalEnum::RESULT_FALSE;
	} else if(result == OvalEnum::RESULT_FALSE) {
			return OvalEnum::RESULT_TRUE;
	} else {
		return result;
	}

}

string OvalEnum::SCStatusToString(OvalEnum::SCStatus status){
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Convert the SCStatus value to a string
	//
	// -----------------------------------------------------------------------
	string statusStr = "";

	switch(status) {
		case (OvalEnum::STATUS_DOES_NOT_EXIST):
			statusStr = "does not exist";
			break;
		case (OvalEnum::STATUS_ERROR):
			statusStr = "error";
			break;
		case (OvalEnum::STATUS_EXISTS):
			statusStr = "exists";
			break;
		case (OvalEnum::STATUS_NOT_COLLECTED):
			statusStr = "not collected";
			break;
	}
	return statusStr;
}

OvalEnum::SCStatus OvalEnum::ToSCStatus(string statusStr){
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Convert the string to a SCStatus value
	// -----------------------------------------------------------------------

	OvalEnum::SCStatus scStatus = OvalEnum::STATUS_ERROR;
	
	if(statusStr.compare(OvalEnum::SCStatusToString(OvalEnum::STATUS_DOES_NOT_EXIST)) == 0) {
		scStatus = OvalEnum::STATUS_DOES_NOT_EXIST;
	} else if(statusStr.compare(OvalEnum::SCStatusToString(OvalEnum::STATUS_ERROR)) == 0) {
		scStatus = OvalEnum::STATUS_ERROR;
	} else if(statusStr.compare(OvalEnum::SCStatusToString(OvalEnum::STATUS_EXISTS)) == 0 || statusStr.compare("") == 0) {
		scStatus = OvalEnum::STATUS_EXISTS;
	} else if(statusStr.compare(OvalEnum::SCStatusToString(OvalEnum::STATUS_NOT_COLLECTED)) == 0) {
		scStatus = OvalEnum::STATUS_NOT_COLLECTED;
	}

	return scStatus;
}

string OvalEnum::SetOperatorToString(OvalEnum::SetOperator setOperator){
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Convert the SetOperator value to a string
	//
	// -----------------------------------------------------------------------
	string setOperatorStr = "";

	switch(setOperator) {
		case (OvalEnum::SET_OPERATOR_COMPLEMENT):
			setOperatorStr = "COMPLEMENT";
			break;
		case (OvalEnum::SET_OPERATOR_INTERSECTION):
			setOperatorStr = "INTERSECTION";
			break;
		case (OvalEnum::SET_OPERATOR_UNION):
			setOperatorStr = "UNION";
			break;
		default:
			throw Exception("Set::SetOperatorToString - Error unsupported setOperator value.");
			break;
	}
	return setOperatorStr;
}

OvalEnum::SetOperator OvalEnum::ToSetOperator(string setOperatorStr){
	// -----------------------------------------------------------------------
	//	Abstract
	//
	//	Convert the string to a SetOperator value
	// -----------------------------------------------------------------------

	SetOperator setOperator;
	
	if(setOperatorStr.compare(OvalEnum::SetOperatorToString(SET_OPERATOR_UNION)) == 0 || setOperatorStr.compare("") == 0) {
		setOperator = OvalEnum::SET_OPERATOR_UNION;
	} else if(setOperatorStr.compare(OvalEnum::SetOperatorToString(SET_OPERATOR_COMPLEMENT)) == 0) {
		setOperator = OvalEnum::SET_OPERATOR_COMPLEMENT;
	} else if(setOperatorStr.compare(OvalEnum::SetOperatorToString(SET_OPERATOR_INTERSECTION)) == 0) {
		setOperator = OvalEnum::SET_OPERATOR_INTERSECTION;
	} else {
		throw Exception("OvalEnum::ToSetOperator - Error unsupported setOperator value: " + setOperatorStr);
	}

	return setOperator;
}
