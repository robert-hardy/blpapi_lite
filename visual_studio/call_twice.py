from ctypes import cdll, c_void_p, c_char_p, c_char, cast, POINTER

fixed_length_string = c_char * 64
three_strings = fixed_length_string * 3
four_strings = fixed_length_string * 4

lib = cdll.LoadLibrary("Debug/bdp_bdh.dll")
lib.BDP_run.restype = c_void_p
lib.BDP_run.argtype = c_char_p
lib.BDH_run.restype = c_void_p
lib.BDH_run.argtype = c_char_p

def blpapi_bdp(securities, fields):
    c_tickers = (c_char_p * len(securities))()
    c_tickers[:] = securities
    c_fields = (c_char_p * len(fields))()
    c_fields[:] = fields
    ptr = lib.BDP_run(len(securities), c_tickers, len(fields), c_fields)
    bbg_response = cast(ptr, POINTER(three_strings))
    results_list = [
        {
            "security": bbg_response[i][0].value,
            "fieldName": bbg_response[i][1].value,
            "fieldValue": bbg_response[i][2].value
        }
        for i in range(len(securities)*len(fields))
        if bbg_response[i][0].value != 'no_data'
    ]
    for d in results_list:
        print d
    lib.freeme(ptr)

def blpapi_bdh(securities, fields):
    c_tickers = (c_char_p * len(securities))()
    c_tickers[:] = securities
    c_fields = (c_char_p * len(fields))()
    c_fields[:] = fields
    startDate = c_char_p("20160401")
    endDate = c_char_p("20160501")
    ptr = lib.BDH_run(len(securities), c_tickers, len(fields), c_fields, startDate, endDate)
    bbg_response = cast(ptr, POINTER(four_strings))
    results_list = [
        {
            "security": bbg_response[i][0].value,
            "date": bbg_response[i][1].value,
            "fieldName": bbg_response[i][2].value,
            "fieldValue": bbg_response[i][3].value
        }
        for i in range(100*len(securities)*len(fields))
        if bbg_response[i][0].value != 'no_data'
    ]
    for d in results_list:
        print d
    lib.freeme(ptr)


if __name__ == "__main__":
    securities = [
        "ERM6 Comdty",
        "EDZ7 Comdty",
        "FOO Comdty",
        "EDZ6 Comdty",
        "EDZ7 Comdty"
    ]
    fields = [
        "PX_LAST",
        "PX_BID",
        "PX_ASK"
    ]
    blpapi_bdp(securities, fields)
    blpapi_bdh(securities, fields)
