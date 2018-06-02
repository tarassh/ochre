#include <eosiolib/eosio.hpp>
#include <ochre.hpp>

void ochre::start(const std::string &description) {

}

void ochre::stop(const uint64_t ochre_id) {

}

void ochre::join(const uint64_t ochre_id, const account_name participant, const checksum256 &hash) {

}

void ochre::withdraw(const uint64_t ochre_id, const account_name participant) {

}

void ochre::reveal(const uint64_t ochre_id, const account_name participant, const checksum256 &secret) {

}

EOSIO_ABI(ochre, (start)(stop)(join)(withdraw)(reveal))
