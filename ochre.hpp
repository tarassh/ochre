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
              global_ochres(_self, _self),
              ochres(_self, _self)
    {}

    //@abi action
    void start(const account_name owner, const uint64_t participant_limit, const std::string &description);

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
        uint64_t next_id = 0;

        uint64_t primary_key() const { return id; }

        EOSLIB_SERIALIZE(global_ochre, (id)(next_id))
    };

    typedef eosio::multi_index<N(global), global_ochre> global_ochre_index;

    //@abi table ochre_event
    struct ochre_event {
        uint64_t id;
        account_name owner;
        uint64_t participant_limit;
        std::string description;

        uint64_t primary_key() const { return id; }

        EOSLIB_SERIALIZE(ochre_event, (id)(owner)(participant_limit)(description))
    };

    typedef eosio::multi_index<N(ochre_event), ochre_event> ochres_index;

    global_ochre_index global_ochres;
    ochres_index ochres;
};