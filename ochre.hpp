/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#include <string>
#include <eosiolib/eosio.hpp>


using namespace eosio;
using namespace std;

class ochre : public eosio::contract {
public:
    using contract::contract;

    ochre(account_name self)
            : contract(self),
              global_ochres(_self, _self)
    {}

    //@abi action
    void start(const std::string &description);

    //@abi action
    void stop(const uint64_t ochre_id);

    //@abi action
    void join(const uint64_t ochre_id, const account_name participant, const checksum256 &hash);

    //@abi action
    void withdraw(const uint64_t ochre_id, const account_name participant);

    //@abi action
    void reveal(const uint64_t ochre_id, const account_name participant, const checksum256 &secret);

private:
    struct participant {
        checksum256 hash;
        checksum256 secret;

        EOSLIB_SERIALIZE( participant, (hash)(secret))
    };

    //@abi table global i64
    struct global_ochre {
        uint64_t id = 0;
        uint64_t nextochreid = 0;

        uint64_t primary_key() const { return id; }

        EOSLIB_SERIALIZE(global_ochre, (id)(nextochreid))
    };

    typedef eosio::multi_index<N(global), global_ochre> global_ochre_index;

    global_ochre_index global_ochres;
};