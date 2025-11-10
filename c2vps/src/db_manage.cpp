#include "db_manage.h"

using namespace drogon::orm;

void user_insert(const std::string& username, const std::string& password, const std::string& email, std::function<void(const drogon::HttpResponsePtr)> callback) {
	auto clients = drogon::app().getDbClient();
	clients->execSqlAsync(
		"SELECT * FROM users WHERE username=$1 OR email=$2",
		[clients, callback, username, email, password](const Result& result) {
			if (!result.empty()) {
				callback(json_parse("error", "username or email already exits", drogon::k400BadRequest));
			}
			else {
				std::string password_hash = hash_password(password);
				if (password_hash.empty()) { callback(json_parse("error", "db_cp error", drogon::k500InternalServerError));return; }
				clients->execSqlAsync(
					"INSERT INTO users (username, password_hash, email) VALUES ($1, $2, $3)",
					[clients, callback, username, email, password_hash](const Result& result) {
						send_mail(email, username, [callback](const drogon::HttpResponsePtr json) {
							callback(json);
							});
					},
					[callback, password_hash](const DrogonDbException& e) {
						callback(json_parse("error", "registerd fail", drogon::k500InternalServerError));
					},
					username, password_hash, email
				);
			}
		},
		[callback](const DrogonDbException& e) {
			callback(json_parse("error", "db select failed", drogon::k500InternalServerError));
		},
		username, email
	);
	return;
}

void user_login(const std::string& username, const std::string& password, std::function<void(drogon::HttpResponsePtr)>callback) {
	auto clients = drogon::app().getDbClient();
	clients->execSqlAsync(
		"SELECT password_hash FROM users WHERE username=$1",
		[clients, password, callback, username](const Result& result) {
			if (result.empty()) {
				callback(json_parse("error", "Incorrect username or password", drogon::k400BadRequest));
				return;
			}
			else {
				std::string hash_from_db = result[0]["password_hash"].as<std::string>();
				if (verify_pw(password, hash_from_db)) {
					clients->execSqlAsync("SELECT is_email_verify FROM users WHERE username=$1 AND password_hash=$2",
						[clients, password, callback, username](const Result& result_mail) {
							if (result_mail.empty()) {
								callback(json_parse("error", "db wents wrong", drogon::k500InternalServerError));
								return;
							}
							else {
								if (!result_mail[0]["is_email_verify"].as<bool>()) {
									callback(json_parse("error", "This account not verify this email", drogon::k400BadRequest));
								}
								else {
									//... 
								}
							}
						},
						[callback](const DrogonDbException& e) {
							callback(json_parse("error", "db wents wrong", drogon::k500InternalServerError));
						},
						username, hash_from_db);
				}
				else {
					callback(json_parse("error", "Incorrect username or password", drogon::k400BadRequest));
					return;
				}
			}
		},
		[callback](const DrogonDbException& e) {
			callback(json_parse("error", "db wents wrong", drogon::k500InternalServerError));
			return;
		},
		username
	);
	return;
}