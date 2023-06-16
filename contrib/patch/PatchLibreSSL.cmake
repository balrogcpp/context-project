# include guard
if (_patch_libressl_included)
    return()
endif (_patch_libressl_included)
set(_patch_libressl_included true)

if (EXISTS crypto/CMakeLists.txt)
    file(READ crypto/CMakeLists.txt TEXT)
    string(REPLACE "set(CRYPTO_POSTFIX" "set(_CRYPTO_POSTFIX" TEXT "${TEXT}")
    file(WRITE crypto/CMakeLists.txt "${TEXT}")
endif ()

if (EXISTS ssl/CMakeLists.txt)
    file(READ ssl/CMakeLists.txt TEXT)
    string(REPLACE "set(SSL_POSTFIX" "set(_SSL_POSTFIX" TEXT "${TEXT}")
    file(WRITE ssl/CMakeLists.txt "${TEXT}")
endif ()

if (EXISTS tls/CMakeLists.txt)
    file(READ tls/CMakeLists.txt TEXT)
    string(REPLACE "set(TLS_POSTFIX" "set(_TLS_POSTFIX" TEXT "${TEXT}")
    file(WRITE tls/CMakeLists.txt "${TEXT}")
endif ()
