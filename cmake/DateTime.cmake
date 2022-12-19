# include guard
if (_datetimes_included)
    return()
endif (_datetimes_included)
set(_datetimes_included true)


string(TIMESTAMP DATE_ISO "%Y-%m-%d" UTC)
string(TIMESTAMP TIME_ISO "%H:%M:%SZ" UTC)
string(TIMESTAMP DATE_TIME_ISO "%Y-%m-%dT%H:%M:%SZ" UTC)

string(TIMESTAMP DATE_CUSTOM "%Y-%m-%d" UTC)
string(TIMESTAMP TIME_CUSTOM "%H-%M-%S" UTC)
string(TIMESTAMP DATE_TIME_CUSTOM "%Y-%m-%d_%H-%M-%S" UTC)

string(TIMESTAMP DATE_CPACK "%Y%m%d" UTC)
string(TIMESTAMP TIME_CPACK "%H%M%SZ" UTC)
string(TIMESTAMP DATE_TIME_CPACK "%Y%m%dT%H%M%SZ" UTC)
