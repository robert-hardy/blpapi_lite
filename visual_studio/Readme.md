How to build the bridge in Visual Studio 2010 Express
=======

Copy-paste from source control into `C:\Users\robert\Documents\Visual Studio
2010\Projects`, work from there and copy back to put into source control.

1. New project -> Win32 Console Application

2. Untick `Create directory for solution` (otherwise you get too much hierarchy).
Give the project a name, leave the default location (no need to create a
directory in there, it will be done automatically).

3. Click OK.

4. In the window that pops up: `Next` -> Choose DLL and `Empty project`.

5. Add the files (probably a `main.c`). Right-click `Source files` in the
   explorer side bar, and add new/existing item.

6. Project properties -> `C/C++` -> `Additional Include Directories`, add
   `C:\blp\DAPI\APIv3\CAPI\include`

7. Project properties -> `Linker` -> `Additional Library Directories`, add
   `C:\blp\DAPI\APIv3\CAPI\lib`

8. Project properties -> `C/C++` -> `Preprocessor` -> `Preprocessor Definitions`, add
   `BBG_BRIDGE_EXPORTS`.

9. Linker -> Input, add `blpapi3_32.lib` and `blpapi3_64.lib` in the `Additional Dependencies` section.

10. Project properties -> `Debugging`:
     - Command = `C:\Python27\python.exe`
     - Command Arguments = `call_twice.py`
     - Working Directory = `$(ProjectDir)`

Notes
---

This page [here](http://holowczak.com/bloomberg-api-cpp-win32-subscriptions/8/) was helpful, it gives a guide to using the BBG API from VS.

This page
[here](http://www.transmissionzero.co.uk/computing/building-dlls-with-mingw/)
gives a good overview of how to build a DLL (albeit in MinGW).

This SO answer
[here](http://stackoverflow.com/questions/3922660/which-visual-c-file-types-should-be-committed-to-version-control)
explains which file types should be put under source control.

BDP & BDH
=========
The request structure is one of two types:

    refDataService: {
      historicalDataRequest = {
        "securities": [
          security1,
          security2,
        ],
        "fields": [
          field1,
          field2
        ],
        "periodicitySelection": "DAILY",
        "startDate": "20160101",
        "endDate": "20160201"
      }
    }

or

    refDataService: {
      referenceDataRequest = {
        "securities": [
          security1,
          security2,
        ],
        "fields": [
          field1,
          field2
        ]
      }
    }

The response structure for a BDP is as follows (one main Event, each security
in the securityData contains an array of fieldData):

    response = {
      "events": [
        {
          "type": BLAPI_EVENTTYPE_PARTIAL_RESPONSE,
          "messages": [
            {
              "responseError": {},
              "securityData": {
                "value" : [
                  // security_array_item 1
                  {
                    "security": {
                      "value": "EDZ7 Comdty"
                    },
                    "sequenceNumber": {
                      "value": 1
                    },
                    "securityError": {},
                    "fieldExceptions": {},
                    "fieldData": [
                      {
                        "dataType": BLPAPI_DATATYPE_SEQUENCE,
                        "name": "foo",
                        anon: [ item1, item2 ]
                      },
                      {
                        "dataType": other,
                        "name": "PX_LAST",
                        "value": "100"
                      },
                      {
                        "dataType": other,
                        "name": "PX_MID",
                        "value": "100"
                      }
                    ]
                  },
                  // security_array_item 2
                  {
                    "security": {
                      "value": "EDZ8 Comdty"
                    },
                    "sequenceNumber": {
                      "value": 2
                    },
                    "securityError": {},
                    "fieldExceptions": {},
                    "fieldData": [
                      {
                        "dataType": other,
                        "name": "PX_LAST",
                        "value": "101"
                      },
                      {
                        "dataType": other,
                        "name": "PX_MID",
                        "value": "101"
                      }
                    ]
                  }
                  // end of securityData.value[]
                ]
              }
            }
            // end of collection of messages in this event
          ]
          // end of this event
        },
        // nextEvent
        {
          "type": BLAPI_EVENTTYPE_RESPONSE,
          // Signifies the end of the response
          "messages": [
            { },
            { }
          ]
        }
        // no more Events
      ]
    }

The response structure for a BDH is as follows (one Event for each security,
one securityData in each Event which contains an array of (date, fieldName,
fieldValue) tupes).

    bdh_response = {
      "events": [
        // Event for the first security
        {
          "type": BLAPI_EVENTTYPE_PARTIAL_RESPONSE,
          "messages": [
            {
              "responseError": {},
              "securityData": {
                "security": {
                  "value": "EDZ7 Comdty"
                },
                "securityError": {},
                "fieldExceptions": {},
                "fieldData": {
                  "value": [
                    {
                      "date": "2016-01-01",
                      "PX_LAST": {
                        "dataType": BLPAPI_DATATYPE_FLOAT32,
                        "value": 101.0
                      },
                      "PX_MID": {
                        "dataType": BLPAPI_DATATYPE_FLOAT32,
                        "value": 101.0
                      }
                    },
                    {
                      "date": "2016-01-02",
                      "PX_LAST": {
                        "dataType": BLPAPI_DATATYPE_FLOAT32,
                        "value": 101.01
                      },
                      "PX_MID": {
                        "dataType": BLPAPI_DATATYPE_FLOAT32,
                        "value": 101.01
                      }
                    }
                  ]
                }
              }
            }
          ]
        },
        // Event for the second security
        {
          "type": BLAPI_EVENTTYPE_PARTIAL_RESPONSE,
          "messages": [
            {
              "responseError": {},
              "securityData": {
                "security": {
                  "value": "EDZ8 Comdty"
                },
                "securityError": {},
                "fieldExceptions": {},
                "fieldData": {
                  "value": [
                    {
                      "date": "2016-01-01",
                      "PX_LAST": {
                        "dataType": BLPAPI_DATATYPE_FLOAT32,
                        "value": 101.0
                      },
                      "PX_MID": {
                        "dataType": BLPAPI_DATATYPE_FLOAT32,
                        "value": 101.0
                      }
                    },
                    {
                      "date": "2016-01-02",
                      "PX_LAST": {
                        "dataType": BLPAPI_DATATYPE_FLOAT32,
                        "value": 101.01
                      },
                      "PX_MID": {
                        "dataType": BLPAPI_DATATYPE_FLOAT32,
                        "value": 101.01
                      }
                    }
                  ]
                }
              }
            }
          ]
        },
        {
          "type": BLAPI_EVENTTYPE_RESPONSE,
          // Signifies the end of the response
          "messages": [
            { },
            { }
          ]
        }
        // no more Events
      ]
    }

Create the session
------------------
    sessionOptions = blpapi_SessionOptions_create();
    blpapi_SessionOptions_setServerHost(sessionOptions, d_host);
    blpapi_SessionOptions_setServerPort(sessionOptions, d_port);
    session = blpapi_Session_create(sessionOptions, 0, 0, 0);
    blpapi_SessionOptions_destroy(sessionOptions);


Build and send the request
--------------------------
    blpapi_Session_getService(session, &refDataSvc, "//blp/refdata");
    blpapi_Service_createRequest(refDataSvc, &request, "[Reference|Historical]DataRequest");
    elements = blpapi_Request_elements(request);
    blpapi_Element_getElement(elements, &securitiesElements, "securities", 0);
    while  (i < d_secCnt ) {
      security = (char *) d_secArray[i];
      blpapi_Element_setValueString(securitiesElements, security, BLPAPI_ELEMENT_INDEX_END);
    }
    blpapi_Element_getElement(elements, &fieldsElements, "fields", 0);
    while  (i < d_fldCnt ) {
      field = (char *) d_fieldArray[i];
      blpapi_Element_setValueString(fieldsElements, field, BLPAPI_ELEMENT_INDEX_END);
    }

Historical only:

    blpapi_Element_setElementString(elements, "periodicitySelection", 0, "DAILY");
    blpapi_Element_setElementString(elements, "startDate", 0, d_startDate);
    blpapi_Element_setElementString(elements, "endDate", 0, d_endDate);

Both:

    memset(&correlationId, '\0', sizeof(correlationId));
    correlationId.size = sizeof(correlationId);
    correlationId.valueType = BLPAPI_CORRELATION_TYPE_INT;
    correlationId.value.intValue = (blpapi_UInt64_t)1;
    blpapi_Session_sendRequest(session, request, &correlationId, 0, 0, 0, 0);
    blpapi_Request_destroy(request);


Unwrapping the response
-----------------------
    while (continueToLoop) {
      blpapi_Session_nextEvent(session, &event, 0);
      switch (blpapi_Event_eventType(event)) {
        case BLPAPI_EVENTTYPE_RESPONSE:
          continueToLoop = 0;
        case BLPAPI_EVENTTYPE_PARTIAL_RESPONSE:
          handleResponseEvent(event, session, results_tuples);
          break;
        default:
          // Prints the error message or checks
          // if session terminated
          // Not done in BDH code
          handleOtherEvent(event, session);
      }
      blpapi_Event_release(event);
    }
    blpapi_Session_stop(session);
    blpapi_Session_destroy(session);
    return results_tuples;


Handling each BDP response event
-------------------------------

In short, a BDP expects an array of `security` elements, each containing an
array of `fieldData` elements:

    iter = blpapi_MessageIterator_create(event);
    while (0 == blpapi_MessageIterator_next(iter, &message)) {
      messageElements = blpapi_Message_elements(message);
      if (blpapi_Element_hasElement(messageElements,"responseError",0)) {
        blpapi_MessageIterator_destroy(iter);
        blpapi_Session_destroy(session);
      }
      if (blpapi_Element_hasElement(messageElements,"securityData",0)) {
        blpapi_Element_getElement(messageElements, &securityData, "securityData", 0);
        numItems = blpapi_Element_numValues(securityData);
        for (i = 0; i < numItems; ++i) {
          blpapi_Element_getValueAsElement(securityData,&security_array_item,i);
          // security_array_item = securityData.value[i]
          blpapi_Element_getElement(security_array_item, &security, "security", 0);
          blpapi_Element_getValueAsString(security, &security_name, 0);
          blpapi_Element_getElement(security_array_item, &sequenceNumberElement, "sequenceNumber", 0);
          blpapi_Element_getValueAsInt32(sequenceNumberElement, &sequenceNumber, 0);
          if (blpapi_Element_hasElement(security_array_item, "securityError", 0)) {
            blpapi_Element_getElement(security_array_item, &securityErrorElement, "securityError", 0);
            blpapi_Element_print(securityErrorElement, &streamWriter, stdout, 0, 4);
            continue;
          }
          if (blpapi_Element_hasElement(security_array_item, "fieldData", 0)) {
            blpapi_Element_getElement(security_array_item, &fieldDataArray, "fieldData", 0);
            numElements = blpapi_Element_numElements(fieldDataArray);
              for(j=0; j<numElements; j++) {
              blpapi_Element_getElementAt(fieldDataArray, &field_array_item, j);
              // field_array_item = securityData.value[i].fieldDataArray[j]
              dataType = blpapi_Element_datatype(field_array_item);
              if(dataType == BLPAPI_DATATYPE_SEQUENCE) {
                processBulkField(field_array_item);
              } else {
                strcpy((char*) results_tuples[results_index][0], security_name);
                // Get fieldDataArray[j].name and fieldDataArray[j].value:
                processRefField((fixed_length_string*) results_tuples[results_index], field_array_item);
                blpapi_Element_getValueAsString(field_array_item, &fieldValue, 0);
                length = strlen(fieldValue) + 1;
              }
              results_index++;
            }
          }
          if (blpapi_Element_hasElement(security_array_item, "fieldExceptions", 0)) {
            blpapi_Element_getElement(security_array_item, &fieldExceptionElement, "fieldExceptions", 0);
            processFieldException(fieldExceptionElement);
          }
        }
      }
    }
