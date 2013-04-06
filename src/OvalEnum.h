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

#ifndef OVALENUM_H
#define OVALENUM_H

#include <string>
#include <vector>

/**
	This class stores all the enumerations that are common to more than one oval schema.
	Along with each enumeration are the methods need for translating that enumeration tothe oval 
	schema equivalent values.
*/
class OvalEnum {

public:
	/** An enum to define the acceptable arithmetic operations. */
	enum ArithmeticOperation	{ARITHMETIC_ADD,
				                ARITHMETIC_MULTIPLY};
	static std::string ArithmeticOperationToString(OvalEnum::ArithmeticOperation arithOp);
	static OvalEnum::ArithmeticOperation ToArithmeticOperation(std::string arithStr);

    /** An enum to define the acceptable check values. */
	enum Check	{CHECK_ALL,
				CHECK_AT_LEAST_ONE,
				CHECK_NONE_EXIST, // DEPRECATED in version 5.3
				CHECK_NONE_SATISFY,
				CHECK_ONLY_ONE};
	static std::string CheckToString(OvalEnum::Check check);
	static OvalEnum::Check ToCheck(std::string checkStr);

	/** An enum to define the acceptable check_existence values. */
	enum Existence	{EXISTENCE_ALL_EXIST,
				EXISTENCE_ANY_EXIST,
				EXISTENCE_AT_LEAST_ONE_EXISTS,
				EXISTENCE_NONE_EXIST,
				EXISTENCE_ONLY_ONE_EXISTS};
	static std::string ExistenceToString(OvalEnum::Existence existence);
	/** Convert the string to an Existence. */
	static OvalEnum::Existence ToExistence(std::string existenceStr);

	/** An enum to define the acceptable data types. */
	enum Datatype	{DATATYPE_STRING,
					DATATYPE_INTEGER,
					DATATYPE_IOS_VERSION,
					DATATYPE_BINARY,
					DATATYPE_BOOLEAN,
					DATATYPE_EVR_STRING,
					DATATYPE_FLOAT,
					DATATYPE_VERSION,
					DATATYPE_RECORD,
					DATATYPE_IPV4_ADDRESS,
					DATATYPE_IPV6_ADDRESS};
	static std::string DatatypeToString(OvalEnum::Datatype datatype);
	static OvalEnum::Datatype ToDatatype(std::string datatypeStr);

    /** An enum to define the acceptable date time formats. */
	enum DateTimeFormat	{DATETIME_YEAR_MONTH_DAY,
					DATETIME_MONTH_DAY_YEAR,
					DATETIME_DAY_MONTH_YEAR,
					DATETIME_WIN_FILETIME,
					DATETIME_SECONDS_SINCE_EPOCH};
	static std::string DateTimeFormatToString(OvalEnum::DateTimeFormat dateTimeFormat);
	static OvalEnum::DateTimeFormat ToDateTimeFormat(std::string dateTimeFormatStr);

	/** enumeration of possible CollectedObject Flag values. */
	enum Flag	{FLAG_ERROR,
				FLAG_COMPLETE,
				FLAG_INCOMPLETE,
				FLAG_DOES_NOT_EXIST,
				FLAG_NOT_COLLECTED,
				FLAG_NOT_APPLICABLE};
	static std::string FlagToString(OvalEnum::Flag flag);
	static OvalEnum::Flag ToFlag(std::string flagStr);
	static OvalEnum::Flag CombineFlags(std::vector<int>* flags);

	/** An enum to define the acceptable levels. */
	enum Level	{LEVEL_DEBUG,
				LEVEL_ERROR,
				LEVEL_FATAL,
				LEVEL_INFO,
				LEVEL_WARNING};
	static std::string LevelToString(OvalEnum::Level level);
	static OvalEnum::Level ToLevel(std::string levelStr);

	/** enumeration of Operations and supporting methods. */
	enum Operation	{OPERATION_EQUALS, 
					OPERATION_NOT_EQUAL, 
					OPERATION_GREATER_THAN, 
					OPERATION_LESS_THAN, 
					OPERATION_GREATER_THAN_OR_EQUAL, 
					OPERATION_LESS_THAN_OR_EQUAL, 
					OPERATION_BITWISE_AND, 
					OPERATION_BITWISE_OR, 
					OPERATION_PATTERN_MATCH,
					OPERATION_CASE_INSENSITIVE_EQUALS,
					OPERATION_CASE_INSENSITIVE_NOT_EQUAL,
					OPERATION_SUBSET_OF,
					OPERATION_SUPERSET_OF
	};
	static std::string OperationToString(OvalEnum::Operation operation);
	static OvalEnum::Operation ToOperation(std::string operationStr);

	/** enumeration of Operators and supporting methods. */
	enum Operator	{OPERATOR_AND,
					OPERATOR_ONE,
					OPERATOR_OR,
					OPERATOR_XOR};
	static OvalEnum::Operator ToOperator(std::string operatorStr);
	static std::string OperatorToString(OvalEnum::Operator op);

	/** enumeration of Result Values and supporting methods. */
	enum ResultEnumeration	{RESULT_TRUE,
							RESULT_FALSE,
							RESULT_UNKNOWN,
							RESULT_ERROR,
							RESULT_NOT_EVALUATED,
							RESULT_NOT_APPLICABLE};
	static OvalEnum::ResultEnumeration ToResult(std::string resultStr);
	static std::string ResultToString(OvalEnum::ResultEnumeration result);
	static std::string ResultToDirectiveString(OvalEnum::ResultEnumeration result);

	static OvalEnum::ResultEnumeration CombineResultsByOperator(std::vector<int>* results, OvalEnum::Operator op);    
	static OvalEnum::ResultEnumeration CombineResultsByCheck(std::vector<int>* results, OvalEnum::Check check);
	static OvalEnum::ResultEnumeration NegateResult(OvalEnum::ResultEnumeration);

	/** An enum to define result content.  Start at 1 and continue using powers of two so that
	they can be combined using binary logic. */
	enum ResultContent {RESULT_CONTENT_FULL=0x1,
						RESULT_CONTENT_THIN=0x2};
	static OvalEnum::ResultContent ToResultContent(std::string resultContentStr);
	static std::string ResultContentToString(OvalEnum::ResultContent resultContent);

	/** Combine the given result content levels.  The combined level is returned.  Currently 
	this simply returns the highest level, but if additional levels are added in the future, this may change. */
	static OvalEnum::ResultContent CombineResultContent(OvalEnum::ResultContent rc1, OvalEnum::ResultContent rc2);

	/** An enum to define the acceptable status values. */
	enum SCStatus	{STATUS_ERROR,
					STATUS_EXISTS,
					STATUS_DOES_NOT_EXIST,
					STATUS_NOT_COLLECTED};
	static std::string SCStatusToString(OvalEnum::SCStatus status);
	static OvalEnum::SCStatus ToSCStatus(std::string statusStr);

	/** An eumeration of possible SetOperator values in the definition schema. */
	enum SetOperator	{SET_OPERATOR_COMPLEMENT, 
						SET_OPERATOR_INTERSECTION,
						SET_OPERATOR_UNION};
	static std::string SetOperatorToString(OvalEnum::SetOperator operation);
	static OvalEnum::SetOperator ToSetOperator(std::string operationStr);

	/** An enumeration of definition classes.  This is defined in the oval-common-schema. **/
	enum ClassEnumeration	{CLASS_DEFAULT,
							CLASS_COMPLIANCE,
							CLASS_INVENTORY,
							CLASS_MISCELLANEOUS,
							CLASS_PATCH,
							CLASS_VULNERABILITY};

	/**
	 * Convert a string into a ClassEnumeration
	 *
	 * @param std::string classStr
	 * @return OvalEnum::ClassEnumeration
	 **/
	static OvalEnum::ClassEnumeration ToClass(std::string classStr);

	/**
	 * Convert a ClassEnumeration into a string
	 *
	 * @param OvalEnum::ClassEnumeration classEnum
	 * @return std::string
	 **/
	static std::string ClassToString(OvalEnum::ClassEnumeration classEnum);
};

#endif
