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

bool verify_pw(const std::string& password, const std::string& passwordhash) {
	return crypto_pwhash_str_verify(passwordhash.c_str(),
		password.c_str(),
		password.size()) == 0;
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

void decode_token(const std::string& token) {
	try {
		auto verifier = jwt::verify<jwt::default_clock, jwt::traits::nlohmann_json>(jwt::default_clock{})
			.allow_algorithm(jwt::algorithm::hs256{ SECRET_KEY })
			.with_issuer(SET_ISSUER);

		auto decoded = jwt::decoded_jwt<jwt::traits::nlohmann_json>(token);
		verifier.verify(decoded);

		std::string email = decoded.get_payload_claim("email").as_string();
		std::string username = decoded.get_payload_claim("username").as_string();

		std::cout << "Email: " << email << "\n";
		std::cout << "Username: " << username << "\n";
	}
	catch (const std::exception& e) {
		std::cerr << "Invalid token: " << e.what() << std::endl;
	}
}