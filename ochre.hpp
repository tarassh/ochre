/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#include <string>
#include <limits>
#include <eosiolib/eosio.hpp>


using namespace eosio;
using namespace std;

class ochre : public eosio::contract {
public:
    using contract::contract;

    ochre(account_name self)
            : contract(self),
              global_ochres(_self, _self),
              events(_self, _self),
              participants(_self, _self)
    {}

    //@abi action
    void start(const account_name owner, const uint64_t participant_limit, const std::string &description);

    //@abi action
    void stop(const uint64_t event_id);

    //@abi action
    void enroll(const uint64_t event_id, const account_name participant, const checksum256 &hash);

    //@abi action
    void reveal(const uint64_t event_id, const account_name participant, const checksum256 &secret);

private:
    //@abi table participant i64
    struct participant {
        uint64_t        id;
        account_name    account;
        uint64_t        event_id;
        checksum256     commitment;
        checksum256     secret;
        uint64_t        reveal_index;

        uint64_t primary_key() const { return id; }

        account_name by_account() const { return account; }

        uint64_t by_event() const { return event_id; }

        void print() const {
            eosio::print("Participant: id = ", id, ", account = ", name{account}, " event = ", event_id);
        }

        bool revealed() const { return reveal_index != std::numeric_limits<uint64_t>::max(); }

        uint128_t get_secret() const {
            uint128_t value = 0;
            memcpy(&value, &secret, sizeof(value));
            return value;
        }

        EOSLIB_SERIALIZE(participant, (id)(account)(event_id)(commitment)(secret)(reveal_index))
    };

    typedef eosio::multi_index<N(participant), participant,
            indexed_by<N(byevent), const_mem_fun<participant, uint64_t, &participant::by_event> >,
            indexed_by<N(byaccount), const_mem_fun<participant, account_name, &participant::by_account> >
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
        uint64_t     enroll_counter;
        uint64_t     reveal_counter;
        std::string  description;
        bool         enrollment;
        bool         revealment;
        account_name winner;

        uint64_t primary_key() const { return id; }

        bool can_enroll() const {
            return enrollment;
        }

        bool can_reveal() const {
            return revealment;
        }

        void print() const {
            eosio::print("Event: id = ", id, ", owner = ", name{owner}, ", limit = ", participant_limit,
                         ", enrolled = ", enroll_counter, ", revealed = ", reveal_counter);
        }

        EOSLIB_SERIALIZE(ochre_event, (id)(owner)(participant_limit)(enroll_counter)(reveal_counter)(description)(enrollment)(revealment)(winner))
    };

    typedef eosio::multi_index<N(events), ochre_event> event_index;

    global_ochre_index global_ochres;
    event_index events;
    participant_index participants;
};