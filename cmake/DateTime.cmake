# include guard
if (_datetimes_included)
    return()
endif (_datetimes_included)
set(_datetimes_included true)


string(TIMESTAMP DATE "%Y-%m-%d" UTC)
string(TIMESTAMP TIME "%H:%M:%S" UTC)
string(TIMESTAMP DATE_TIME "%Y-%m-%d:%H:%M:%S" UTC)
