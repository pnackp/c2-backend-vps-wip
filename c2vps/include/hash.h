#pragma once
#include <sodium.h>
#include "c2vps.h"
#include <jwt-cpp/jwt.h>
#include <jwt-cpp/traits/nlohmann-json/traits.h>
#include <iostream>

#define SECRET_KEY "cc_cutest_character"
#define SET_ISSUER "c2vps"

std::string hash_password(const std::string& password);
std::string token(const std::string& email, const std::string& username);

bool verify_pw(const std::string& password, const std::string& passwordhash);
void decode_token(const std::string& token);



