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
              ochres(_self, _self),
              participants(_self, _self)
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
    //@abi table participant i64
    struct participant {
        uint64_t        id;
        account_name    account;
        uint64_t        ochre_id;
        checksum256     hash;
        checksum256     secret;

        uint64_t primary_key() const { return id; }

        uint64_t by_event() const { return ochre_id; }

        EOSLIB_SERIALIZE( participant, (id)(account)(ochre_id)(hash)(secret))
    };

    typedef eosio::multi_index<N(participant), participant,
            indexed_by<N(event), const_mem_fun<participant, uint64_t, &participant::by_event> >
    > participant_index;

    //@abi table global i64
    struct global_ochre {
        uint64_t id = 0;
        uint64_t next_id = 0;

        uint64_t primary_key() const { return id; }

        EOSLIB_SERIALIZE(global_ochre, (id)(next_id))
    };

    typedef eosio::multi_index<N(global), global_ochre> global_ochre_index;

    //@abi table events i64
    struct ochre_event {
        uint64_t     id;
        account_name owner;
        uint64_t     participant_limit;
        uint64_t     participant_number;
        std::string  description;
        bool         hash_registration;
        bool         secret_registration;

        uint64_t primary_key() const { return id; }

        bool can_join(account_name &new_participant) const {
            return participant_number < participant_limit;
        }

        EOSLIB_SERIALIZE(ochre_event, (id)(owner)(participant_limit)(participant_number)(description))
    };

    typedef eosio::multi_index<N(events), ochre_event> event_index;

    global_ochre_index global_ochres;
    event_index ochres;
    participant_index participants;
};