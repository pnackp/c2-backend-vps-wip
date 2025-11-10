#include "hash.h"

std::string hash_password(const std::string& password) {
	char hash[crypto_pwhash_STRBYTES];
	if (crypto_pwhash_str(
		hash,
		password.c_str(),
		password.size(),
		crypto_pwhash_OPSLIMIT_INTERACTIVE,
		crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0) {
		std::cerr << "Out of memory" << std::endl;
		return "";
	}
	return std::string(hash);
}

std::string token(const std::string& email, const std::string& username) {
	try {
		auto token = jwt::builder<jwt::default_clock, jwt::traits::nlohmann_json>{ jwt::default_clock{} }
			.set_issuer(SET_ISSUER)
			.set_payload_claim("email", jwt::basic_claim<jwt::traits::nlohmann_json>(email))
			.set_payload_claim("username", jwt::basic_claim<jwt::traits::nlohmann_json>(username))
			.sign(jwt::algorithm::hs256{ SECRET_KEY });
		return token;
	}
	catch (...) {
		return "";
	}
}