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

#include <iostream>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMNodeList.hpp>

#include "Log.h"
#include "DocumentManager.h"
#include "Common.h"
#include "XmlCommon.h"
#include "Version.h"
#include "AbsVariable.h"
#include "CollectedObject.h"

#include "AbsDataCollector.h"

using namespace std;
using namespace xercesc;

//****************************************************************************************//
//								AbsDataCollector Class									  //	
//****************************************************************************************//
AbsDataCollector* AbsDataCollector::instance = NULL;
bool AbsDataCollector::isRunning = false;

AbsDataCollector::AbsDataCollector() {
	this->collectedObjectsElm = NULL;
	this->systemDataElm = NULL;	
}

AbsDataCollector::~AbsDataCollector() {

	delete(this->objectCollector);
}

// ***************************************************************************************	//
//									Public members											//
// ***************************************************************************************	//
AbsDataCollector* AbsDataCollector::Instance() {

	return AbsDataCollector::instance;
}

DOMElement* AbsDataCollector::GetSCCollectedObjectsElm() {

	if(this->collectedObjectsElm == NULL) {
		//	Create the collected_objects element and add it as a child of the sc element
		DOMElement* scNode = XmlCommon::FindElement(DocumentManager::GetSystemCharacteristicsDocument(), "oval_system_characteristics");
		DOMElement *elm = XmlCommon::AddChildElementNS(DocumentManager::GetSystemCharacteristicsDocument(), scNode, XmlCommon::scNS, "collected_objects"); 
		this->collectedObjectsElm = elm;
	}
	return this->collectedObjectsElm;
}

DOMElement* AbsDataCollector::GetSCSystemDataElm() {

	if(this->systemDataElm == NULL) {
		//	Create the system_data element and add it as a child of the sc element
		DOMElement* scNode = XmlCommon::FindElement(DocumentManager::GetSystemCharacteristicsDocument(), "oval_system_characteristics");
		DOMElement *elm = XmlCommon::AddChildElementNS(DocumentManager::GetSystemCharacteristicsDocument(), scNode, XmlCommon::scNS, "system_data"); 
		this->systemDataElm = elm;
	}
	return this->systemDataElm;
}

void AbsDataCollector::InitBase(AbsObjectCollector* objectCollector) {

	string errMsg;

	if(objectCollector != NULL) 
		this->objectCollector = objectCollector;
	else
		throw AbsDataCollectorException("Error: The specified objectCollector is NULL. Unable to initialize the data collector.");

	//	Get the system_characteristics node
	DOMElement* scNode = XmlCommon::FindElement(DocumentManager::GetSystemCharacteristicsDocument(), "oval_system_characteristics");

	//	Add the namespace info to the oval_system_characteristics node
	XmlCommon::AddXmlns(DocumentManager::GetSystemCharacteristicsDocument(), "http://oval.mitre.org/XMLSchema/oval-common-5", "oval");
	XmlCommon::AddXmlns(DocumentManager::GetSystemCharacteristicsDocument(), "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5", "oval-sc");
	XmlCommon::AddXmlns(DocumentManager::GetSystemCharacteristicsDocument(), "http://www.w3.org/2001/XMLSchema-instance", "xsi");
	XmlCommon::AddSchemaLocation(DocumentManager::GetSystemCharacteristicsDocument(), "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5 oval-system-characteristics-schema.xsd");
	XmlCommon::AddSchemaLocation(DocumentManager::GetSystemCharacteristicsDocument(), "http://oval.mitre.org/XMLSchema/oval-common-5 oval-common-schema.xsd");

	// Add Generator Info
	this->WriteGenerator();

	//	Create the system_info element and add it as a child of the sc element
	DOMElement *sysInfoElm = XmlCommon::CreateElementNS(DocumentManager::GetSystemCharacteristicsDocument(), XmlCommon::scNS, "system_info"); 
	scNode->appendChild(sysInfoElm);

	// Write system info
	this->WriteSystemInfo();
}

void AbsDataCollector::AddXmlns(string newXmlnsAlias, string newXmlnsUri) {

	XmlCommon::AddXmlns(DocumentManager::GetSystemCharacteristicsDocument(), newXmlnsUri, newXmlnsAlias);
}

void AbsDataCollector::AddSchemaLocation(string newSchemaLocation) {

	XmlCommon::AddSchemaLocation(DocumentManager::GetSystemCharacteristicsDocument(), newSchemaLocation);
}

void AbsDataCollector::Run() {

	AbsDataCollector::isRunning = true;
	//////////////////////////////////////////////////////
	////////////////  Process OVAL objects  //////////////
	//////////////////////////////////////////////////////
	//	get a ptr to the objects node in the oval document.
	DOMElement* objectsNode = XmlCommon::FindElementNS(DocumentManager::GetDefinitionDocument(), "objects");
	if(objectsNode != NULL) {
		//	get a list of the child nodes
		DOMNodeList* ovalObjectsChildren = objectsNode->getChildNodes();

		if(!Log::WriteToScreen())
			cout << "      Collecting object:  "; 

		//	Loop through all the nodes in objects children
		int prevIdLength = 1;
		int curIdLength = 1;
		unsigned int index = 0;
		while(index < ovalObjectsChildren->getLength()) {
			DOMNode *tmpNode = ovalObjectsChildren->item(index);

			//	only concerned with ELEMENT_NODEs
			if (tmpNode->getNodeType() == DOMNode::ELEMENT_NODE) {
				DOMElement *object = (DOMElement*)tmpNode;

				string objectId = XmlCommon::GetAttributeByName(object, "id");
				
				Log::Debug("Collecting object id: " + objectId);

				if(!Log::WriteToScreen()) {
					curIdLength = objectId.length();
					string blankSpaces = "";
					if(prevIdLength > curIdLength)
						blankSpaces = Common::PadStringWithChar(blankSpaces, ' ', prevIdLength-curIdLength);

					string backSpaces = "";
					backSpaces = Common::PadStringWithChar(backSpaces, '\b', prevIdLength);
					string endBackSpaces = "";
					endBackSpaces = Common::PadStringWithChar(endBackSpaces, '\b', blankSpaces.length());
					cout << backSpaces << objectId << blankSpaces << endBackSpaces;
				}

				this->objectCollector->Run(objectId);

				prevIdLength = curIdLength;
			}

			index ++;
		}

		if(!Log::WriteToScreen()) {
			string fin = " FINISHED ";
			int curLen = fin.length();
			string blankSpaces = "";
			if(prevIdLength > curLen)
				blankSpaces = Common::PadStringWithChar(blankSpaces, ' ', prevIdLength-curLen);
			string backSpaces = "";
			backSpaces = Common::PadStringWithChar(backSpaces, '\b', prevIdLength);
			
			cout << backSpaces << fin << blankSpaces << endl;
		}

		// Once finished running call write method on all collected objects
		CollectedObject::WriteCollectedObjects();

		// clean up after the run completes
		State::ClearCache();
		AbsVariable::ClearCache();
		AbsProbe::ClearGlobalCache();
        Item::ClearCache();
		VariableValue::ClearCache();
	} 

	AbsDataCollector::isRunning = false;
}

bool AbsDataCollector::GetIsRunning(){

	return AbsDataCollector::isRunning;
}

void AbsDataCollector::WriteGenerator() {

	DOMElement *scNode = XmlCommon::FindElement(DocumentManager::GetSystemCharacteristicsDocument(), "oval_system_characteristics");
	DOMElement *generatorElm = XmlCommon::AddChildElementNS(DocumentManager::GetSystemCharacteristicsDocument(), scNode, XmlCommon::scNS, "generator");
	XmlCommon::AddChildElementNS(DocumentManager::GetSystemCharacteristicsDocument(), generatorElm, XmlCommon::comNS, "oval:product_name", "cpe:/a:mitre:ovaldi:" + Version::GetVersion() + "." + Version::GetBuild());
	XmlCommon::AddChildElementNS(DocumentManager::GetSystemCharacteristicsDocument(), generatorElm, XmlCommon::comNS, "oval:product_version", Version::GetVersion() + " Build: " + Version::GetBuild());
	XmlCommon::AddChildElementNS(DocumentManager::GetSystemCharacteristicsDocument(), generatorElm, XmlCommon::comNS, "oval:schema_version", Version::GetSchemaVersion());
	XmlCommon::AddChildElementNS(DocumentManager::GetSystemCharacteristicsDocument(), generatorElm, XmlCommon::comNS, "oval:timestamp", Common::GetTimeStamp());
	XmlCommon::AddChildElement(DocumentManager::GetSystemCharacteristicsDocument(), generatorElm, "vendor", Version::GetVendor());
}

//****************************************************************************************//
//						AbsDataCollectorException Class									  //	
//****************************************************************************************//
AbsDataCollectorException::AbsDataCollectorException(string errMsgIn, int severity, Exception* ex) : Exception(errMsgIn, severity, ex) {

}

AbsDataCollectorException::~AbsDataCollectorException() {

}

