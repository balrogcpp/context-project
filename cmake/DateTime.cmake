# include guard
if (_datetimes_included)
    return()
endif (_datetimes_included)
set(_datetimes_included true)

string(TIMESTAMP DATE_TIME "%Y-%m-%dT%H:%M:%SZ" UTC)
