#include <eosiolib/eosio.hpp>
#include <ochre.hpp>

void ochre::start(const account_name owner, const uint64_t participant_limit, const std::string &description) {
    // Create global ochre counter if not exists
    auto g_ochre_itr = global_ochres.begin();
    if( g_ochre_itr == global_ochres.end() ) {
        g_ochre_itr = global_ochres.emplace(_self, [&](auto& g_ochre){
            g_ochre.next_id = 0;
        });
    }

    // Increment global ochre counter
    global_ochres.modify(g_ochre_itr, 0, [&](auto& g_ochre){
        g_ochre.next_id++;
    });

    // Create a new on chain random selection event
    auto ochre_event_itr = ochres.emplace(_self, [&](auto &new_event) {
        new_event.id                = g_ochre_itr->next_id;
        new_event.owner             = owner;
        new_event.participant_limit = participant_limit;
        new_event.description       = description;
    });
}

void ochre::stop(const uint64_t ochre_id) {
    auto iter = ochres.find(ochre_id);

    eosio_assert(iter != ochres.end(), "Event does not exists");
    require_auth(iter->owner);

    ochres.erase(iter);
}

void ochre::join(const uint64_t ochre_id, const account_name participant, const checksum256 &hash) {

}

void ochre::withdraw(const uint64_t ochre_id, const account_name participant) {

}

void ochre::reveal(const uint64_t ochre_id, const account_name participant, const checksum256 &secret) {

}

EOSIO_ABI(ochre, (start)(stop)(join)(withdraw)(reveal))
