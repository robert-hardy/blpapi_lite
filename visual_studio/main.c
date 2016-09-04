#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifdef BBG_BRIDGE_EXPORTS
#define BBGBRIDGE_API __declspec(dllexport)
#else
#define BBGBRIDGE_API __declspec(dllimport)
#endif

#define BBGCLIENT_CALL __cdecl

/* Copyright 2012. Bloomberg Finance L.P.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:  The above copyright notice and this
 * permission notice shall be included in all copies or substantial portions of
 * the Software.  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO
 * EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <blpapi_event.h>
#include <blpapi_element.h>
#include <blpapi_message.h>
#include <blpapi_name.h>
#include <blpapi_request.h>
#include <blpapi_session.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* for strcmp(3C) */

#define SEC_MAX			256
#define FIELD_MAX		256

typedef char fixed_length_string[64];
typedef fixed_length_string three_strings[3];
typedef fixed_length_string four_strings[4];

#define REFDATA_SVC "//blp/refdata"
#define SECURITY_DATA "securityData"
#define SECURITY_ERROR "securityError"
#define SECURITY_NAME "security"
#define DATE "date"
#define FIELD_ID "fieldId"
#define FIELD_DATA "fieldData"
#define FIELD_EXCEPTIONS "fieldExceptions"
#define ERROR_INFO "errorInfo"
#define MESSAGE "message"
#define FIELD_ID "fieldId"

static int streamWriter(const char* data, int length, void *stream)
{
	assert(data);
	assert(stream);
	return (int) fwrite(data, length, 1, (FILE *)stream);
}

/**************************************
 * Convert the fieldName to UpperCase *
 **************************************/
void  ConvertToUpperCase(char * str)
{
	int ch, i;

	for(i=0;i < strlen(str); i++)
	{
		ch = toupper(str[i]);
		str[i] = ch;
	}
}

int BDH_Process_Exceptions(blpapi_Message_t *message)
{
	int i = 0;
	blpapi_Element_t *exception_Element = NULL;
	blpapi_Element_t *errorInfo_Element = NULL;
	blpapi_Element_t *securityData = NULL;
	blpapi_Element_t *messageElements = NULL;
	blpapi_Element_t *fieldExceptions = NULL;
	blpapi_Element_t *numFieldExceptions = NULL;
	int num_of_Exceptions = NULL;

	messageElements = blpapi_Message_elements(message);
	blpapi_Element_getElement(messageElements, &securityData, SECURITY_DATA, 0);
	if(blpapi_Element_hasElement(securityData, FIELD_EXCEPTIONS, 0))
	{
		blpapi_Element_getElement(securityData, &fieldExceptions, FIELD_EXCEPTIONS, 0);
		num_of_Exceptions = blpapi_Element_numValues(fieldExceptions);

		if(num_of_Exceptions > 0)
		{
			for(i = 0; i < num_of_Exceptions; i++)
			{
				blpapi_Element_t *field_id = NULL;
				blpapi_Element_t *error_info = NULL;
				blpapi_Element_t *error_message = NULL;
				const char *_field_id = NULL;
				const char *_error_info = NULL;
				const char *_error_message = NULL;

				blpapi_Element_getValueAsElement(fieldExceptions, &exception_Element, i);
				assert(exception_Element);
				blpapi_Element_getElement(exception_Element, &errorInfo_Element, ERROR_INFO, 0);
				assert(errorInfo_Element);

				blpapi_Element_getElement(exception_Element, &field_id, FIELD_ID, 0);
				assert(field_id);
				blpapi_Element_getValueAsString(field_id, &_field_id, 0);
				printf("%s  ",_field_id);

				blpapi_Element_getElement(errorInfo_Element, &error_message, MESSAGE, 0);
				assert(error_message);
				blpapi_Element_getValueAsString(error_message, &_error_message, 0);
				printf("%s  \n", _error_message);
			}
			return 0;
		}
		return -1;
	}
}

int BDH_Process_Errors(blpapi_Message_t *message)
{
	blpapi_Element_t *securityData = NULL;
	blpapi_Element_t *securityError = NULL;
	blpapi_Element_t *messageElements = NULL;
	blpapi_Element_t *errorMessage = NULL;

	const char *_error_message = NULL;

	messageElements = blpapi_Message_elements(message);
	blpapi_Element_getElement(messageElements, &securityData, SECURITY_DATA, 0);

	if(blpapi_Element_hasElement(securityData, SECURITY_ERROR, 0))
	{
		blpapi_Element_getElement(securityData, &securityError, SECURITY_ERROR, 0);
		blpapi_Element_getElement(securityError, &errorMessage, MESSAGE, 0);
		assert(errorMessage);
		blpapi_Element_getValueAsString(errorMessage, &_error_message, 0);
		printf("%s  \n", _error_message);
		return 0;
	}
	return -1;
}

void BDH_Process_Fields(four_strings *four_string_tuple, blpapi_Message_t *message, int d_fldCnt, char d_fieldArray[][64])
{
	blpapi_Element_t *messageElements = NULL;
	blpapi_Element_t *securityData = NULL;
	blpapi_Element_t *securityName = NULL;
	const char *sec_name;
	blpapi_Element_t *_field1 = NULL;
	blpapi_Element_t *_datatype = NULL;
	blpapi_Element_t *_date = NULL;
	blpapi_Element_t *fieldData = NULL;
	blpapi_Element_t *fieldData2 = NULL;
	int numValues = NULL;
	const char *date;
	char  * field;
	int i = 0;
	int j = 0;
	int datatype;
	int results_index = 0;

	const char *delimiter = "\t\t";

	// print out date column heading
	printf("DATE");
	printf(delimiter);
	i = 0;
	// Set fields specified on command line
	while  (i < d_fldCnt ) {
		field = (char *) d_fieldArray[i];
		printf("%s", field);
		printf(delimiter);
		i++;
	}
	printf("\n");

	messageElements = blpapi_Message_elements(message);

	blpapi_Element_getElement(messageElements, &securityData, "securityData", 0);
	blpapi_Element_getElement(securityData, &securityName, "security", 0);
	blpapi_Element_getValueAsString(securityName, &sec_name, 0);
	blpapi_Element_getElement(securityData, &fieldData, "fieldData", 0);

	// need to get the individual field data elements out first

	numValues = blpapi_Element_numValues(fieldData);
	if(numValues > 0)
	{
		for(j = 0; j < numValues; j++)
		{

			blpapi_Element_getValueAsElement(fieldData, &fieldData2, j);
			blpapi_Element_getElement(fieldData2, &_date, "date", 0);
			blpapi_Element_getValueAsString(_date, &date, 0);
			printf("%s ", date);
			printf("\t");

			i = 0;
			// Set fields specified on command line
			while  (i < d_fldCnt ) {
				field = (char *) d_fieldArray[i];
				ConvertToUpperCase(field);
				if(blpapi_Element_hasElement(fieldData2, field, 0))
				{
					strcpy((char*) four_string_tuple[results_index][0], sec_name);
					strcpy(four_string_tuple[results_index][1], date);
					strcpy(four_string_tuple[results_index][2], field);
					blpapi_Element_getElement(fieldData2, &_field1, field, 0);
					datatype = blpapi_Element_datatype(_field1);
					switch(datatype)
					{
						case BLPAPI_DATATYPE_BOOL://Bool
							{
								blpapi_Bool_t field1;
								blpapi_Element_getValueAsBool(_field1, &field1, 0);
								printf("%i	", field1);
								break;
							}
						case BLPAPI_DATATYPE_CHAR://Char
							{
								char field1;
								blpapi_Element_getValueAsChar(_field1, &field1, 0);
								printf("%s	", field1);
								break;
							}
						case BLPAPI_DATATYPE_INT32://Int32
							{
								blpapi_Int32_t field1;
								blpapi_Element_getValueAsInt32(_field1, &field1, 0);
								printf("%i	", field1);
								break;
							}
						case BLPAPI_DATATYPE_INT64://Int64
							{
								blpapi_Int64_t field1;
								blpapi_Element_getValueAsInt64(_field1, &field1, 0);
								sprintf(four_string_tuple[results_index][3], "%g", field1);
								printf("%i	", field1);
								break;
							}
						case BLPAPI_DATATYPE_FLOAT32://Float32
							{
								blpapi_Float32_t field1;
								blpapi_Element_getValueAsFloat32(_field1, &field1, 0);
								sprintf(four_string_tuple[results_index][3], "%g", field1);
								printf("%i	", field1);
								break;
							}
						case BLPAPI_DATATYPE_FLOAT64://Float64
							{
								blpapi_Float64_t field1;
								blpapi_Element_getValueAsFloat64(_field1, &field1, 0);
								sprintf(four_string_tuple[results_index][3], "%g", field1);
								printf("%f	", field1);
								break;
							}
						case BLPAPI_DATATYPE_STRING://String
							{
								const char *field1;
								blpapi_Element_getValueAsString(_field1, &field1, 0);
								strcpy(four_string_tuple[results_index][3], field1);
								printf("%s	", field1);
								break;
							}
						case BLPAPI_DATATYPE_DATE://Date
							{
								blpapi_Datetime_t field1;
								blpapi_Element_getValueAsDatetime(_field1, &field1, 0);
								printf("%s	", field1);
								break;
							}
						case BLPAPI_DATATYPE_TIME://Time
							{
								blpapi_Datetime_t field1;
								blpapi_Element_getValueAsDatetime(_field1, &field1, 0);
								printf("%s	", field1);
								break;
							}
						case BLPAPI_DATATYPE_DATETIME://Datetime
							{
								blpapi_Datetime_t field1;
								blpapi_Element_getValueAsDatetime(_field1, &field1, 0);
								printf("%s	", field1);
								break;
							}
						default:
							{
								const char *field1;
								blpapi_Element_getValueAsString(_field1, &field1, 0);
								printf("%s", field1);
								break;
							}
					}
				}
				i++;
				results_index++;
			}
			printf("\n");
		}
		printf("\n\n");
	}

}

// BDP things

void BDP_processFieldException(blpapi_Element_t *fieldExceptionElement)
{
	int i = 0;
	int numExceptions = 0;
	const char *fieldId = NULL;
	const char *category = NULL;
	const char *message = NULL;
	blpapi_Element_t *exception_Element = NULL;
	blpapi_Element_t *errorInfo_Element = NULL;
	blpapi_Element_t *fieldId_Element = NULL;
	blpapi_Element_t *category_ele = NULL;
	blpapi_Element_t *message_ele = NULL;

	// Get the number of fieldExceptions received in message
	numExceptions = blpapi_Element_numValues(fieldExceptionElement);
	if(numExceptions > 0){
		printf("FIELD\t\tEXCEPTION\n");
		printf("-----\t\t---------\n");
		for(i=0; i<numExceptions; i++){

			blpapi_Element_getValueAsElement(fieldExceptionElement, &exception_Element, i);
			assert(exception_Element);
			blpapi_Element_getElement(exception_Element, &errorInfo_Element, "errorInfo", 0);
			assert(errorInfo_Element);

			// read fieldId - (Calcroute ID or Mnemonic)
			// Typically contains invalid field for the request.
			blpapi_Element_getElement(exception_Element, &fieldId_Element, "fieldId", 0);
			assert(fieldId_Element);
			blpapi_Element_getValueAsString(fieldId_Element, &fieldId, 0);

			// read error category
			blpapi_Element_getElement(errorInfo_Element, &category_ele, "category", 0);
			assert(category_ele);
			blpapi_Element_getValueAsString(category_ele, &category, 0);

			// read error message
			blpapi_Element_getElement(errorInfo_Element, &message_ele, "message", 0);
			assert(message_ele);
			blpapi_Element_getValueAsString(message_ele, &message, 0);

			printf("%s\t\t%s(%s)\n", fieldId, category, message);
		}
	}

}

void BDP_processRefField(fixed_length_string *three_string_tuple, blpapi_Element_t *field_Element)
{
	const char *fieldName = NULL;
	const char *fieldValue = NULL;

	fieldName = blpapi_Element_nameString (field_Element);
	blpapi_Element_getValueAsString(field_Element, &fieldValue, 0);

	printf("\t%s = %s\n", fieldName, fieldValue);
	strcpy(three_string_tuple[1], fieldName);
	strcpy(three_string_tuple[2], fieldValue);
}

void BDP_processBulkField(blpapi_Element_t *field_Element)
{
	int bvCtr = 0;
	int beCtr = 0;
	int numOfBulkValues = 0;
	int numOfBulkEle = 0;
	const char *fieldName = NULL;
	const char *bulkFieldName = NULL;
	const char *bulkFieldValue = NULL;
	blpapi_Element_t *bulkElement = NULL;
	blpapi_Element_t *elem = NULL;

	fieldName = blpapi_Element_nameString (field_Element);
	printf("\t%s = \n", fieldName);

	// Get the count of values for the field.
	numOfBulkValues = blpapi_Element_numValues(field_Element);
	for(bvCtr = 0; bvCtr < numOfBulkValues; bvCtr++){
		bulkElement = 0;
		blpapi_Element_getValueAsElement(field_Element, &bulkElement, bvCtr);
		assert(bulkElement);
		numOfBulkEle = blpapi_Element_numElements(bulkElement);
		for(beCtr = 0; beCtr < numOfBulkEle; beCtr++){
			elem = 0;
			blpapi_Element_getElementAt(bulkElement, &elem, beCtr);
			assert(elem);
			bulkFieldName = blpapi_Element_nameString (elem);
			blpapi_Element_getValueAsString(elem, &bulkFieldValue, 0);
			printf("\t%s = %s\n", bulkFieldName, bulkFieldValue);
		}
		printf("\n");
	}
}

void BDP_sendRefDataRequest(
	blpapi_Session_t *session,
	int d_secCnt,
	char d_secArray[][64],
	int d_fldCnt,
	char d_fieldArray[][64]
)
{
	blpapi_Service_t *refDataSvc = NULL;
	blpapi_Request_t *request = NULL;
	blpapi_Element_t *elements = NULL;
	blpapi_Element_t *securitiesElements = NULL;
	blpapi_Element_t *fieldsElements = NULL;
	blpapi_CorrelationId_t correlationId;
    char  * security;
    char  * field;
	int i = 0;

	assert(session);
	blpapi_Session_getService(session, &refDataSvc, "//blp/refdata");
	// Create Reference Data Request using //blp/refdata service
	blpapi_Service_createRequest(refDataSvc, &request, "ReferenceDataRequest");
	assert(request);

	elements = blpapi_Request_elements(request);
	assert(elements);

	// Get "securities" element
	blpapi_Element_getElement(elements,	&securitiesElements, "securities", 0);
	assert(securitiesElements);
	i = 0;
	// Set securities specified on command line
	while  (i < d_secCnt ) {
        security = (char *) d_secArray[i];
		blpapi_Element_setValueString(securitiesElements, security, BLPAPI_ELEMENT_INDEX_END);
		i++;
    }

	// Get "fields" element
	blpapi_Element_getElement(elements, &fieldsElements, "fields", 0);

	i = 0;
	// Set fields specified on command line
	while  (i < d_fldCnt ) {
        field = (char *) d_fieldArray[i];
		blpapi_Element_setValueString(fieldsElements, field, BLPAPI_ELEMENT_INDEX_END);
		i++;
    }

	// Print the request on the console.
	blpapi_Element_print(elements, &streamWriter, stdout, 0, 4);

	// Init Correlation ID object
	memset(&correlationId, '\0', sizeof(correlationId));
	correlationId.size = sizeof(correlationId);
	correlationId.valueType = BLPAPI_CORRELATION_TYPE_INT;
	correlationId.value.intValue = (blpapi_UInt64_t)1;
	// Sending request
	blpapi_Session_sendRequest(session, request, &correlationId, 0, 0, 0, 0);

	blpapi_Request_destroy(request);

	return;
}

static void BDP_handleResponseEvent(const blpapi_Event_t *event,
									blpapi_Session_t *session,
									three_strings *results_tuples)
{
	blpapi_MessageIterator_t *iter = NULL;
	blpapi_Message_t *message = NULL;
	int i = 0;
	int results_index = 0;
	const char *fieldValue = NULL;
	size_t length = 0;
	assert(event);
	assert(session);
	iter = blpapi_MessageIterator_create(event);
	assert(iter);
	// Iterate through messages received
	while (0 == blpapi_MessageIterator_next(iter, &message)) {
		blpapi_Element_t *referenceDataResponse = NULL;
		blpapi_Element_t *securityDataArray = NULL;
		int numItems = 0;

		assert(message);

		referenceDataResponse = blpapi_Message_elements(message);
		assert(referenceDataResponse);

		// If a request cannot be completed for any reason, the responseError
		// element is returned in the response. responseError contains detailed
		// information regarding the failure.
		// Printing the responseError on the console, release the allocated
		// resources and exiting the program
		if (blpapi_Element_hasElement(referenceDataResponse,"responseError",0)){
			fprintf(stderr, "has responseError\n");
			blpapi_Element_print(referenceDataResponse, &streamWriter, stdout,
								0, 4);
            blpapi_MessageIterator_destroy(iter);
			blpapi_Session_destroy(session);
			exit(1);
		}

		// securityData Element contains Array of ReferenceSecurityData
		// containing Response data for each security specified in the request.
		blpapi_Element_getElement(referenceDataResponse, &securityDataArray,
									"securityData", 0);
		// Get the number of securities received in message
		numItems = blpapi_Element_numValues(securityDataArray);
		printf("\nProcessing %d security(s)\n", numItems);

		for (i = 0; i < numItems; ++i) {
			blpapi_Element_t *securityData = NULL;
			blpapi_Element_t *securityElement = NULL;
			const char *security = NULL;
			blpapi_Element_t *sequenceNumberElement = NULL;
			int sequenceNumber = -1;
			blpapi_Element_getValueAsElement(securityDataArray,&securityData,i);
			assert(securityData);
			blpapi_Element_getElement(securityData, &securityElement,
										"security", 0);
			assert(securityElement);
			// Read the security specified
			blpapi_Element_getValueAsString(securityElement, &security, 0);
			assert(security);
			// reading the sequenceNumber element
			blpapi_Element_getElement(securityData, &sequenceNumberElement,
									"sequenceNumber", 0);
			assert(sequenceNumberElement);
			blpapi_Element_getValueAsInt32(sequenceNumberElement,
									&sequenceNumber, 0);

			// Checking if there is any Security Error
			if (blpapi_Element_hasElement(securityData, "securityError", 0)){
				//If present, this indicates that the specified security could
				// not be processed. This element contains a detailed reason for
				// the failure.
				blpapi_Element_t *securityErrorElement = NULL;
				printf("\tSecurity = %s\n", security);
				blpapi_Element_getElement(securityData, &securityErrorElement,
											"securityError", 0);
				assert(securityErrorElement);
				blpapi_Element_print(securityErrorElement, &streamWriter,
										stdout, 0, 4);
				continue;
			}

			if (blpapi_Element_hasElement(securityData, "fieldData", 0)){
				int j = 0;
				int numElements = 0;
				blpapi_Element_t *fieldDataElement = NULL;
				blpapi_Element_t *field_Element = NULL;

				printf("\tSecurity = %s\n", security);
				printf("\tsequenceNumber = %d\n", sequenceNumber);
				// Get fieldData Element
				blpapi_Element_getElement(securityData, &fieldDataElement,
										"fieldData", 0);
				assert(fieldDataElement);
				// Get the number of securities received in message
				numElements = blpapi_Element_numElements(fieldDataElement);
				printf("Processing %d field(s)\n", numElements);
				for(j=0; j<numElements; j++){
					int dataType = 0;
					blpapi_Element_getElementAt(fieldDataElement,
											&field_Element, j);
					assert(field_Element);
					dataType = blpapi_Element_datatype(field_Element);
					if(dataType == BLPAPI_DATATYPE_SEQUENCE){
						// read the data for bulk field
						BDP_processBulkField(field_Element);
					}else{
						// Write the security into the tuple
						strcpy((char*) results_tuples[results_index][0], security);
						// Write the fieldName and fieldValue into the tuple
						BDP_processRefField((fixed_length_string*) results_tuples[results_index], field_Element);


						blpapi_Element_getValueAsString(field_Element, &fieldValue, 0);

						length = strlen(fieldValue) + 1;
					}
					results_index++;
				}
				printf("\n");
			}

			if (blpapi_Element_hasElement(securityData, "fieldExceptions", 0)){
				blpapi_Element_t *fieldExceptionElement = NULL;
				// Get fieldException Element
				blpapi_Element_getElement(securityData, &fieldExceptionElement,
													"fieldExceptions", 0);
				assert(fieldExceptionElement);
				// read the field exception errors for invalid fields
				BDP_processFieldException(fieldExceptionElement);
			}
		}
	}
	blpapi_MessageIterator_destroy(iter);
}

/*****************************************************************************
Function    : handleOtherEvent
Description : This function handles events other than response and partial
			  response event. This function gets the messages from the event
			  and print them on the console. If the event is session terminate,
			  then release allocated resources and exit the program.
Arguments   : Pointer to blpapi_Event_t
			  Pointer to session
Returns     : void
*****************************************************************************/
static void BDP_handleOtherEvent(const blpapi_Event_t *event,
							 blpapi_Session_t *session)
{
	blpapi_MessageIterator_t *iter = NULL;
	blpapi_Message_t *message = NULL;

	assert(event);
	assert(session);
	// Event has one or more messages. Create message iterator for event
	iter = blpapi_MessageIterator_create(event);
	assert(iter);

	// Iterate through messages received
	while (0 == blpapi_MessageIterator_next(iter, &message)) {
		blpapi_Element_t *messageElements = NULL;
		assert(message);

		printf("messageType=%s\n", blpapi_Message_typeString(message));

		// Get the message element and print it on console.
		messageElements = blpapi_Message_elements(message);
		assert(messageElements);
		blpapi_Element_print(messageElements, &streamWriter, stdout, 0, 4);

		// If session status is sessionTerminated, release allocated resource
		// and exit the program.
		if (BLPAPI_EVENTTYPE_SESSION_STATUS == blpapi_Event_eventType(event)
			&& 0 == strcmp("SessionTerminated",
			blpapi_Message_typeString(message))){
			fprintf(stdout, "Terminating: %s\n",
							blpapi_Message_typeString(message));
            blpapi_MessageIterator_destroy(iter);
			blpapi_Session_destroy(session);
			exit(1);
		}
	}
	// Destroy the message iterator.
	blpapi_MessageIterator_destroy(iter);
}

BBGBRIDGE_API const three_strings* BBGCLIENT_CALL BDP_run(int nb_tickers, const char **ticker_list, int nb_fields, const char **field_list)
{
	char*			d_host = "localhost";
	int				d_port = 8194;
	char 			d_secArray[SEC_MAX][64];
	char 			d_fieldArray[FIELD_MAX][64];
	int				d_secCnt = 0;
	int				d_fldCnt = 0;
	three_strings	*results_tuples = NULL;

	blpapi_SessionOptions_t *sessionOptions = NULL;
	blpapi_Session_t *session = NULL;
	int continueToLoop = 1;
	int i;

	for(i=0; i<nb_tickers; i++) {
		strcpy(d_secArray[d_secCnt++], ticker_list[i]);
	}
	for(i=0; i<nb_fields; i++) {
		strcpy(d_fieldArray[d_fldCnt++], field_list[i]);
	}

	results_tuples = (three_strings*) malloc(d_secCnt * d_fldCnt * sizeof(three_strings));
	for(i=0; i<d_secCnt*d_fldCnt; i++) {
		strcpy(results_tuples[i][0], "no_data");
		strcpy(results_tuples[i][1], "no_data");
		strcpy(results_tuples[i][2], "no_data");
	}

	// create sessionOptions instance.
	sessionOptions = blpapi_SessionOptions_create();
	assert(sessionOptions);

	// Set the host and port for the session.
	blpapi_SessionOptions_setServerHost(sessionOptions, d_host);
	blpapi_SessionOptions_setServerPort(sessionOptions, d_port);

	// Create the session
	session = blpapi_Session_create(sessionOptions, 0, 0, 0);
	assert(session);

	blpapi_SessionOptions_destroy(sessionOptions);
	printf("Connecting to %s:%d\n", d_host, d_port);
	// Start a Session
	if (0 != blpapi_Session_start(session)) {
		fprintf(stderr, "Failed to start session.\n");
		blpapi_Session_destroy(session);
	}
	// Open Reference Data Service
	if (0 != blpapi_Session_openService(session, "//blp/refdata")){
		fprintf(stderr, "Failed to open service //blp/refdata.\n");
		blpapi_Session_destroy(session);
	}

	// create and send reference data request to session
	BDP_sendRefDataRequest(session, d_secCnt, d_secArray, d_fldCnt, d_fieldArray);

	// Poll for the events from the session until complete response for
	// request is received. For each event received, do the desired processing.
	while (continueToLoop) {
		blpapi_Event_t *event = NULL;
		blpapi_Session_nextEvent(session, &event, 0);
		assert(event);
		switch (blpapi_Event_eventType(event)) {
			case BLPAPI_EVENTTYPE_PARTIAL_RESPONSE:
				// Process the partial response event to get data. This event
				// indicates that request has not been fully satisfied.
				printf("Processing Partial Response\n");
				BDP_handleResponseEvent(event, session, results_tuples);
				break;
			case BLPAPI_EVENTTYPE_RESPONSE: /* final event */
		        // Process the response event. This event indicates that
                // request has been fully satisfied, and that no additional
                // events should be expected.
				printf("Processing Response\n");
				BDP_handleResponseEvent(event, session, results_tuples);
				continueToLoop = 0; /* fall through */
				break;
			default:
				// Process events other than PARTIAL_RESPONSE or RESPONSE.
				BDP_handleOtherEvent(event, session);
				break;
		}

		blpapi_Event_release(event);
	}
	// Destory/release allocated resources.
	blpapi_Session_stop(session);
	blpapi_Session_destroy(session);

	return results_tuples;
}

BBGBRIDGE_API const four_strings* BBGCLIENT_CALL BDH_run(int nb_tickers, const char **ticker_list, int nb_fields, const char **field_list, const char* startDate, const char* endDate)
{
	char  * security;
	char  * field;
	int i = 0;
	int continueToLoop;
	blpapi_SessionOptions_t *sessionOptions;
	blpapi_Session_t *session;
	blpapi_Service_t *refDataService;
	blpapi_Request_t *request;
	blpapi_Element_t *elements;
	blpapi_Element_t *securitiesElements;
	blpapi_Element_t *fieldsElements;
	blpapi_CorrelationId_t correlationId;
	blpapi_MessageIterator_t *msgIter;
	blpapi_Message_t *message;
	blpapi_Element_t *messageElements;

	char*			d_host = "localhost";
	int				d_port = 8194;
	char			d_secArray[SEC_MAX][64];
	char 			d_fieldArray[FIELD_MAX][64];
	int				d_secCnt = 0;
	int				d_fldCnt = 0;

	four_strings *results_tuples = NULL;

	for(i=0; i<nb_tickers; i++) {
		strcpy(d_secArray[d_secCnt++], ticker_list[i]);
	}
	for(i=0; i<nb_fields; i++) {
		strcpy(d_fieldArray[d_fldCnt++], field_list[i]);
	}

	results_tuples = (four_strings*) malloc(500 * d_secCnt * d_fldCnt * sizeof(four_strings));
	for(i=0; i<500*d_secCnt*d_fldCnt; i++) {
		strcpy(results_tuples[i][0], "no_data");
		strcpy(results_tuples[i][1], "no_data");
		strcpy(results_tuples[i][2], "no_data");
		strcpy(results_tuples[i][3], "no_data");
	}

	sessionOptions = blpapi_SessionOptions_create();

	blpapi_SessionOptions_setServerHost(sessionOptions, "localhost");
	blpapi_SessionOptions_setServerPort(sessionOptions, d_port);

	printf("Connecting to %s:%d\n",
			blpapi_SessionOptions_serverHost(sessionOptions),
			blpapi_SessionOptions_serverPort(sessionOptions));

	session = blpapi_Session_create(sessionOptions, 0, 0, 0);

	if (blpapi_Session_start(session) != 0)
	{
		fprintf(stderr, "Failed to start session.\n");
		blpapi_Session_destroy(session);
		return results_tuples;
	}

	if (blpapi_Session_openService(session, REFDATA_SVC) != 0)
	{
		fprintf(stderr, "Failed to open service %s.\n", REFDATA_SVC);
		blpapi_Session_destroy(session);
		return results_tuples;
	}

	refDataService = NULL;
	blpapi_Session_getService(session, &refDataService, REFDATA_SVC);

	request = NULL;
	blpapi_Service_createRequest(refDataService, &request, "HistoricalDataRequest");

	elements = NULL;
	elements = blpapi_Request_elements(request);
	// Get securities element
	blpapi_Element_getElement(elements,	&securitiesElements, "securities", 0);
	assert(securitiesElements);

	i = 0;
	// Set securities specified on command line
	while  (i < d_secCnt ) {
		security = (char *) d_secArray[i];
		blpapi_Element_setValueString(securitiesElements, security, BLPAPI_ELEMENT_INDEX_END);
		i++;
	}
	// Get Field element
	blpapi_Element_getElement(elements, &fieldsElements, "fields", 0);

	i = 0;
	// Set fields specified on command line
	while  (i < d_fldCnt ) {
		field = (char *) d_fieldArray[i];
		blpapi_Element_setValueString(fieldsElements, field, BLPAPI_ELEMENT_INDEX_END);
		i++;
	}


	blpapi_Element_setElementString(elements, "periodicitySelection", 0, "DAILY");
	blpapi_Element_setElementString(elements, "startDate", 0, startDate);
	blpapi_Element_setElementString(elements, "endDate", 0, endDate);

	blpapi_Element_print(elements, &streamWriter, stdout, 0, 4);
	printf("\n");

	memset(&correlationId, '\0', sizeof(correlationId));
	correlationId.size = sizeof(correlationId);
	correlationId.valueType = BLPAPI_CORRELATION_TYPE_INT;
	correlationId.value.intValue = 0;

	blpapi_Session_sendRequest(session, request, &correlationId, 0, 0, 0, 0);

	blpapi_Request_destroy(request);

	continueToLoop = 1;
	while (continueToLoop)
	{
		blpapi_Event_t *event = NULL;
		blpapi_Session_nextEvent(session, &event, 0);

		if ( (blpapi_Event_eventType(event) != BLPAPI_EVENTTYPE_RESPONSE) &&
				(blpapi_Event_eventType(event) != BLPAPI_EVENTTYPE_PARTIAL_RESPONSE) )
		{
			blpapi_Event_release(event);
			continue;
		}
		else
		{
			if ( (blpapi_Event_eventType(event) == BLPAPI_EVENTTYPE_RESPONSE))
			{
				continueToLoop = 0;
			}

			msgIter = blpapi_MessageIterator_create(event);
			message = NULL;
			messageElements = NULL;

			while (blpapi_MessageIterator_next(msgIter, &message) == 0)
			{
				// Get the message element and print it on console.
				messageElements = blpapi_Message_elements(message);
				if(blpapi_Element_hasElement(messageElements, "securityData", 0))
				{
					// check the message type
					// if it's a history response, extract the appropriate data
					blpapi_Element_t *securityName = NULL;
					blpapi_Element_t *securityData = NULL;
					const char *sec_name;

					blpapi_Element_getElement(messageElements, &securityData, "securityData", 0);
					blpapi_Element_getElement(securityData, &securityName, "security", 0);
					blpapi_Element_getValueAsString(securityName, &sec_name, 0);
					printf("%s\n\n", sec_name);

					if(BDH_Process_Exceptions(message) == -1)
					{
						if(BDH_Process_Errors(message) == -1)
						{
							BDH_Process_Fields(results_tuples, message, d_fldCnt, d_fieldArray);
						}
					}
				}
			}
			blpapi_Event_release(event);
		}
	}// end of while 0
	// Destory/release allocated resources.
	blpapi_Session_stop(session);
	blpapi_Session_destroy(session);

	return results_tuples;
}
BBGBRIDGE_API void BBGCLIENT_CALL freeme(char *ptr)
{
    printf("freeing address: %p\n", ptr);
    free(ptr);
}
