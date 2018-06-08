#include <eosiolib/eosio.hpp>
#include <eosiolib/print.h>
#include <eosiolib/crypto.h>
#include <ochre.hpp>
#include <vector>

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
    auto iter = events.emplace(_self, [&](auto &new_event) {
        new_event.id                  = g_ochre_itr->next_id;
        new_event.owner               = owner;
        new_event.participant_limit   = participant_limit;
        new_event.enroll_counter      = 0;
        new_event.reveal_counter      = 0;
        new_event.description         = description;

        new_event.enrollment          = true;
        new_event.revealment          = false;
    });

    eosio::print("New OChRe event registered [id = ",  iter->id, "]\n",
                 "Owner: ", eosio::name{iter->owner}, "\n",
                 "Description: ", iter->description, "\n",
                 "Maximum participant number: ", iter->participant_limit, "\n",
                 "Scope: ", name{events.get_scope()}, "\n");
}

void ochre::stop(const uint64_t event_id) {
    auto iter = events.find(event_id);
    eosio_assert(iter != events.end(), "Event does not exists");
    require_auth(iter->owner);

    eosio::print("Removing OChRe event [id = ",  iter->id, "]\n",
                 "Owner: ", eosio::name{iter->owner}, "\n",
                 "Description: ", iter->description, "\n");

    events.erase(iter);

    auto idx = participants.template get_index<N(byevent)>();

    vector<uint64_t> to_remove;
    for (const auto &item: idx) {
        if (item.event_id == event_id) {
            to_remove.push_back(item.id);
        }
    }

    eosio::print("Removing Participants...\n");
    for (const auto &participant_id: to_remove) {
        auto iter = participants.find(participant_id);
        iter->print();
        eosio::print("\n");
        participants.erase(iter);
    }
}

void ochre::enroll(const uint64_t event_id, const account_name participant, const checksum256 &hash) {
    require_auth(participant);

    auto event_iter = events.find(event_id);
    eosio_assert(event_iter != events.end(), "Event does not exists");
    eosio_assert(event_iter->can_enroll(), "Enrollment is closed");

    auto idx = participants.get_index<N(byevent)>();

    for (const auto &item: idx) {
        bool already_enrolled = item.event_id == event_id && item.account == participant;
        eosio_assert(!already_enrolled, "Already enrolled");
    }

    auto iter = participants.emplace(_self, [&](auto &new_participant){
        new_participant.id       = participants.available_primary_key();
        new_participant.account  = participant;
        new_participant.event_id = event_id;
        new_participant.hash     = hash;
        new_participant.secret   = {0};
        new_participant.revealed = false;
    });

    eosio::print("Event [", iter->event_id, "]: new participant: ", eosio::name{iter->account}, "\n",
                 "Scope: ", name{participants.get_scope()}, "\nHash: ");
    printhex(&iter->hash, sizeof(iter->hash));
    eosio::print("\n");

    events.modify(event_iter, 0, [&](auto &event) {
        if (++event.enroll_counter == event.participant_limit) {
            event.enrollment = false;
            event.revealment = true;
        }
    });

    if (event_iter->revealment) {
        eosio::print("Ready to reveal\n");
    }

    eosio::print("------EVENT------\n");
    event_iter->print();
    eosio::print("\n------END-----\n");
    eosio::print("------PARTICIPANTS------\n");
    for (const auto &item: idx) {
        item.print();
        eosio::print("\n");
    }
    eosio::print("------END------\n");
}

void ochre::reveal(const uint64_t event_id, const account_name participant, const checksum256 &secret) {
    require_auth(participant);

    auto event_iter = events.find(event_id);
    eosio_assert(event_iter != events.end(), "Event does not exists");
    eosio_assert(event_iter->can_reveal(), "Revealment is closed");

    auto idx = participants.get_index<N(byevent)>();
    for (const auto &item: idx) {
        if (item.event_id == event_id && item.account == participant) {
            eosio_assert(!item.revealed, "Already revealed");

            assert_sha256( (char *)&secret, sizeof(secret), (const checksum256 *)&item.hash );

            participants.modify(item, 0, [&](auto &p) {
                p.secret = secret;
                p.revealed = true;
            });

            break;
        }
    }

    events.modify(event_iter, 0, [&](auto &event) {
        if (++event.reveal_counter == event.participant_limit) {
            event.enrollment = false;
            event.revealment = false;
        }
    });

    if (!event_iter->revealment) {
        idx = participants.get_index<N(byevent)>();

        uint128_t number = 0;
        for (const auto &item: idx) {
            if (item.event_id == event_id) {
                eosio::print("Participant secret: ");
                printhex(&item.secret, sizeof(item.secret));
                eosio::print(" uint = ", item.get_secret(), "\n");

                number += item.get_secret();
            }
        }

        number %= event_iter->participant_limit;
        eosio::print("WINNER ", number);
    }
}

EOSIO_ABI(ochre, (start)(stop)(enroll)(reveal))
