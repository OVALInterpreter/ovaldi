//
//
//****************************************************************************************//
// Copyright (c) 2002-2010, The MITRE Corporation
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

#ifndef ABSENTITYVALUE_H
#define ABSENTITYVALUE_H

#include "OvalEnum.h"
using namespace std;
XERCES_CPP_NAMESPACE_USE
/**
	This class represents an entity value in an entity as defined in the oval system characteristics schema.
*/
class AbsEntityValue{
public:
	/** AbsEntityValue constructor. */
	AbsEntityValue();

	/** AbsEntityValue constructor. */
	AbsEntityValue(string value);

	/** AbsEntityValue virtual destructor. */
	virtual ~AbsEntityValue();

	/** Return the value of the entity.
	 *  @return A string representing the value of the entity.
	 */
	string GetValue();

	/** Set the value of the entity.
	 *  @param value A string representation of the value of the entity.
	 *  @return Void.
	 */
	void SetValue(string value);

	/** Determine if this entity value is equal to the specified entity value. 
	 *  @param entityValue The AbsEntityValue that you would like to compare to this entity value.
	 *  @return A boolean value indicating whether or not the specified value is equal to this entity value.
	 */
	virtual bool Equals(AbsEntityValue *entityValue)=0;

protected:
	string value;
};

/**	
	A vector for storing AbsEntityValue objects. 
	Stores only pointers to the objects. 
*/
typedef vector < AbsEntityValue*, allocator<AbsEntityValue*> > AbsEntityValueVector;

#endif
