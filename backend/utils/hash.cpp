#include "hash.h"
#include <cstdlib>
#include <cstring>
// #include <print>
#include <sodium/core.h>

namespace Crypt{
    void init(){
        sodium_init();
    }
    
    void hash_password(char hashed[crypto_pwhash_STRBYTES], const char* password){
        if (crypto_pwhash_str(hashed, password, strlen(password),
                     crypto_pwhash_OPSLIMIT_INTERACTIVE,
                      crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0) {
        std::exit(1);
    }
    }
    bool verify_hash(const char *hashed,const char* password){
        if (crypto_pwhash_str_verify(hashed, password, strlen(password)) == 0) 
            return true;
        return false;
    }
}