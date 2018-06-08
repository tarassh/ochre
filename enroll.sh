#!/usr/bin/env bash


cleos push action ochre start '["user", 3, "TEST event"]' -p user

cleos push action ochre enroll '[1, "p1", "d533f24d6f28ddcef3f066474f7b8355383e485681ba8e793e037f5cf36e4883"]' -p p1
cleos push action ochre enroll '[1, "p2", "50ed53fcdaf27f88d51ea4e835b1055efe779bb87e6cfdff47d28c88ffb27129"]' -p p2
cleos push action ochre enroll '[1, "p3", "50ed53fcdaf27f88d51ea4e835b1055efe779bb87e6cfdff47d28c88ffb27129"]' -p p3

cleos push action ochre reveal '[1, "p1", "28349b1d4bcdc9905e4ef9719019e55743c84efa0c5e9a0b077f0b54fcd84905"]' -p p1
cleos push action ochre reveal '[1, "p2", "15fe76d25e124b08feb835f12e00a879bd15666a33786e64b655891fba7d6c12"]' -p p2
cleos push action ochre reveal '[1, "p3", "15fe76d25e124b08feb835f12e00a879bd15666a33786e64b655891fba7d6c12"]' -p p3

