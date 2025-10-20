#include<sodium.h>

namespace Crypt{
    void init();
    void key_derivation();
    void hash_password(char[crypto_pwhash_STRBYTES], const char*);
    bool verify_hash(const char *hashed,const char* password);
}