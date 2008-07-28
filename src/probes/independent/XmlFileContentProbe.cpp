//
// $Id: XmlFileContentProbe.cpp 4579 2008-01-02 17:39:07Z bakerj $
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

#include "XmlFileContentProbe.h"

XmlFileContentProbe* XmlFileContentProbe::instance = NULL;

XmlFileContentProbe::XmlFileContentProbe() {

}

XmlFileContentProbe::~XmlFileContentProbe() {

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* XmlFileContentProbe::Instance() {

	// Use lazy initialization
	if(instance == NULL) 
		instance = new XmlFileContentProbe();

	return instance;	
}

ItemVector* XmlFileContentProbe::CollectItems(Object* object) {

	// get the hive, key, and name from the provided object
	ObjectEntity* path = object->GetElementByName("path");
	ObjectEntity* fileName = object->GetElementByName("filename");
	ObjectEntity* xpath = object->GetElementByName("xpath");
	
	// check datatypes - only allow string
	if(path->GetDatatype() != OvalEnum::DATATYPE_STRING) {
		throw ProbeException("Error: invalid data type specified on path. Found: " + OvalEnum::DatatypeToString(path->GetDatatype()));
	}
	if(fileName->GetDatatype() != OvalEnum::DATATYPE_STRING) {
		throw ProbeException("Error: invalid data type specified on fileName. Found: " + OvalEnum::DatatypeToString(fileName->GetDatatype()));
	}
	if(xpath->GetDatatype() != OvalEnum::DATATYPE_STRING) {
		throw ProbeException("Error: invalid data type specified on xpath. Found: " + OvalEnum::DatatypeToString(xpath->GetDatatype()));
	}

	// check operation 
	if(path->GetOperation() != OvalEnum::OPERATION_EQUALS 
		&& path->GetOperation() != OvalEnum::OPERATION_PATTERN_MATCH 
		&& path->GetOperation() != OvalEnum::OPERATION_NOT_EQUAL) {
		throw ProbeException("Error: invalid operation specified on path. Found: " + OvalEnum::OperationToString(path->GetOperation()));
	}
	if(fileName->GetOperation() != OvalEnum::OPERATION_EQUALS 
		&& fileName->GetOperation() != OvalEnum::OPERATION_PATTERN_MATCH 
		&& fileName->GetOperation() != OvalEnum::OPERATION_NOT_EQUAL) {
		throw ProbeException("Error: invalid operation specified on fileName. Found: " + OvalEnum::OperationToString(fileName->GetOperation()));
	}
	if(xpath->GetOperation() != OvalEnum::OPERATION_EQUALS) {
		throw ProbeException("Error: invalid operation specified on xpath. Found: " + OvalEnum::OperationToString(xpath->GetOperation()));
	}

	ItemVector *collectedItems = new ItemVector();


	FileFinder fileFinder;
	StringPairVector* filePaths = fileFinder.SearchFiles(path, fileName, object->GetBehaviors());

	//StringPairVector* filePaths = this->GetFiles(path, fileName, object->GetBehaviors());

	if(filePaths->size() > 0) {
		// Loop through all file paths
		StringPairVector::iterator iterator;
		for(iterator = filePaths->begin(); iterator != filePaths->end(); iterator++) {

			StringPair* fp = (*iterator);

			if(fp->second.compare("") == 0) {

				Item* item = NULL;

				// check if the code should report that the filename does not exist.
				StringVector fileNames;
				if(fileFinder.ReportFileNameDoesNotExist(fp->first, fileName, &fileNames)) {
					StringVector::iterator iterator;
					for(iterator = fileNames.begin(); iterator != fileNames.end(); iterator++) {

						item = this->CreateItem();
						item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
						item->AppendElement(new ItemEntity("path", fp->first, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
						item->AppendElement(new ItemEntity("filename", (*iterator), OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_DOES_NOT_EXIST));
						collectedItems->push_back(item);
					}
					
				} else {

					item = this->CreateItem();
					item->SetStatus(OvalEnum::STATUS_EXISTS);
					item->AppendElement(new ItemEntity("path", fp->first, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
					collectedItems->push_back(item);

				}

			} else {

				Item* item = this->EvaluateXpath((*iterator)->first, (*iterator)->second, xpath->GetValue());
				if(item != NULL) {
					collectedItems->push_back(item);
				}
				item = NULL;

			}

			delete fp;
		}

	} else {
		// if no filepaths check if the code should report that the path does not exist
		StringVector paths;
		if(fileFinder.ReportPathDoesNotExist(path, &paths)) {

			Item* item = NULL;
			StringVector::iterator iterator;
			for(iterator = paths.begin(); iterator != paths.end(); iterator++) {

				item = this->CreateItem();
				item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
				item->AppendElement(new ItemEntity("path", (*iterator), OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_DOES_NOT_EXIST));
				collectedItems->push_back(item);
			}
		}
	}
	delete filePaths;

	return collectedItems;
}

Item* XmlFileContentProbe::EvaluateXpath(string path, string fileName, string xpath) {

	Item* item = NULL;

	string contextNode = "/";

	string filePath = path;
	if(path[path.length()-1] != Common::fileSeperator)
		filePath.append(1, Common::fileSeperator);

	if(fileName[0] != Common::fileSeperator) {
		filePath.append(fileName);
	} else {
		filePath.append(fileName.substr(1, fileName.length()-2));
	}

	XALAN_USING_XALAN(XSLException)

	//XALAN_USING_XERCES(XMLPlatformUtils)
	XALAN_USING_XALAN(XPathEvaluator)
	XMLPlatformUtils::Initialize();
	XPathEvaluator::initialize();

	XALAN_USING_XERCES(LocalFileInputSource)
	XALAN_USING_XALAN(XalanDocument)
	XALAN_USING_XALAN(XalanDocumentPrefixResolver)
	XALAN_USING_XALAN(XalanDOMString)
	XALAN_USING_XALAN(XalanNode)
	XALAN_USING_XALAN(XalanSourceTreeInit)
	XALAN_USING_XALAN(XalanSourceTreeDOMSupport)
	XALAN_USING_XALAN(XalanSourceTreeParserLiaison)
	XALAN_USING_XALAN(XObjectPtr)
	XALAN_USING_XALAN(NodeRefList)
	XALAN_USING_XALAN(CharVectorType)

	// Initialize the XalanSourceTree subsystem...
	XalanSourceTreeInit	theSourceTreeInit;

	// We'll use these to parse the XML file.
	XalanSourceTreeDOMSupport theDOMSupport;
	XalanSourceTreeParserLiaison theLiaison(theDOMSupport);

	// Hook the two together...
	theDOMSupport.setParserLiaison(&theLiaison);
		
	try {

		const XalanDOMString theFileName(filePath.c_str());

		// Create an input source that represents a local file...
		const LocalFileInputSource theInputSource(theFileName.c_str());

		// Parse the document...
		XalanDocument* theDocument = NULL;
		try {
			theDocument = theLiaison.parseXMLStream(theInputSource);

		} catch(...) {
			theDocument = NULL;
			// this should never happen at this point only documents that exist should get here
			//string errMsg = "Error: The specified document does not exist: " + filePath;
		}

		if(theDocument == NULL) {
			throw ProbeException("Error: Unable to parse the current document: " + filePath);
		} else {

			XalanDocumentPrefixResolver	thePrefixResolver(theDocument);

			XPathEvaluator theEvaluator;

			// find the context node...
			XalanNode* const theContextNode =
					theEvaluator.selectSingleNode(	theDOMSupport,
													theDocument,
													XalanDOMString(contextNode.c_str()).c_str(),
													thePrefixResolver);

			if (theContextNode == 0) {
				throw ProbeException("Error the specified context node, \'" + contextNode + "\' was not found.");
			} else {

				// evaluate the expression...
				/*const XObjectPtr theResult = XObjectPtr(
				theEvaluator.evaluate(
						theDOMSupport,
						theContextNode,
						XalanDOMString(xpath.c_str()).c_str(),
						thePrefixResolver));*/

				NodeRefList nodeList;
				theEvaluator.selectNodeList(nodeList,
											theDOMSupport,
											theContextNode,
											XalanDOMString(xpath.c_str()).c_str(),
											thePrefixResolver);


				if(nodeList.getLength() <= 0) {
					// no nodes were found
					item = this->CreateItem();
					item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
					item->AppendElement(new ItemEntity("path", path, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
					item->AppendElement(new ItemEntity("filename", fileName, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
					item->AppendElement(new ItemEntity("xpath", xpath, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
					item->AppendElement(new ItemEntity("value_of", "", OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_DOES_NOT_EXIST));

				} else {

					item = this->CreateItem();
					item->SetStatus(OvalEnum::STATUS_EXISTS);
					item->AppendElement(new ItemEntity("path", path, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
					item->AppendElement(new ItemEntity("filename", fileName, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));
					item->AppendElement(new ItemEntity("xpath", xpath, OvalEnum::DATATYPE_STRING, true, OvalEnum::STATUS_EXISTS));

					// add each value returned.
					int length = nodeList.getLength();
					for(int i = 0; i < length; i++) {
						XalanNode* node = nodeList.item(i);

						if(node->getNodeType() == XalanNode::TEXT_NODE) {
							
							const CharVectorType chVec = TranscodeToLocalCodePage(node->getNodeValue());
							string value;
							for( int i=0; chVec[i] !='\0'; i++)
								value += chVec[i];

							item->AppendElement(new ItemEntity("value_of", value, OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));

						} else if(node->getNodeType() == XalanNode::ATTRIBUTE_NODE) {

							const CharVectorType chVec = TranscodeToLocalCodePage(node->getNodeValue());
							string value;
							for( int i=0; chVec[i] !='\0'; i++)
								value += chVec[i];

							item->AppendElement(new ItemEntity("value_of", value, OvalEnum::DATATYPE_STRING, false, OvalEnum::STATUS_EXISTS));

						} else {

							throw ProbeException("Error: invalid xpath specified. An xpath is only allowed to select text nodes or attribute nodes from a document.");
						}
					}
				}
			}
		}

		XPathEvaluator::terminate();
		XMLPlatformUtils::Terminate();

	} catch(const XSLException& theException) {
		// Convert the XSLException message to a string
		ostringstream m;
		m << (theException.getMessage());
		string errMsg = m.str();

		XPathEvaluator::terminate();
		XMLPlatformUtils::Terminate();

		throw ProbeException("Error while evaluating an xpath. " + errMsg);

	} catch(ProbeException ex) {

		XPathEvaluator::terminate();
		XMLPlatformUtils::Terminate();

		throw ex;

	} catch(Exception ex) {

		XPathEvaluator::terminate();
		XMLPlatformUtils::Terminate();

		throw ex;

	} catch(...) {

		XPathEvaluator::terminate();
		XMLPlatformUtils::Terminate();

		throw ProbeException("Error: XmlFileContentProbe() An unknown error occured while collecting data.");
	}

	return item;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
Item* XmlFileContentProbe::CreateItem() {

	Item* item = new Item(0, 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#independent", 
						"ind-sc", 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#independent independent-system-characteristics-schema.xsd", 
						OvalEnum::STATUS_ERROR, 
						"xmlfilecontent_item");

	return item;
}
