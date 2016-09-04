from ctypes import cdll, c_void_p, c_char_p, c_char, cast, POINTER
from datetime import date

fixed_length_string = c_char * 64
three_strings = fixed_length_string * 3
four_strings = fixed_length_string * 4

try:
    lib = cdll.LoadLibrary("bdp_bdh.dll")
    lib.BDP_run.restype = c_void_p
    lib.BDP_run.argtype = c_char_p
    lib.BDH_run.restype = c_void_p
    lib.BDH_run.argtype = c_char_p
except:
    lib = None


def bdp(securities, fields):
    c_tickers = (c_char_p * len(securities))()
    c_tickers[:] = securities
    c_fields = (c_char_p * len(fields))()
    c_fields[:] = fields

    try:
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
        lib.freeme(ptr)
    except:
        results_list = []
    return results_list


def bdh(securities, fields, start_date, end_date):
    c_tickers = (c_char_p * len(securities))()
    c_tickers[:] = securities
    c_fields = (c_char_p * len(fields))()
    c_fields[:] = fields
    startDate = c_char_p(datetime.strftime(start_date, "%Y%m%d"))
    endDate = c_char_p(datetime.strftime(end_date, "%Y%m%d"))

    try:
        ptr = lib.BDH_run(len(securities), c_tickers, len(fields), c_fields, startDate, endDate)
        bbg_response = cast(ptr, POINTER(four_strings))
        results_list = [
            {
                "security": bbg_response[i][0].value,
                "date": bbg_response[i][1].value,
                "fieldName": bbg_response[i][2].value,
                "value": bbg_response[i][3].value
            }
            for i in range(500*len(securities)*len(fields))
            if bbg_response[i][0].value != 'no_data'
        ]
        lib.freeme(ptr)
    except:
        results_list = []
    return results_list
