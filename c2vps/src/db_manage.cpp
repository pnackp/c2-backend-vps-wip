#include "db_manage.h"

using namespace drogon::orm;

void user_insert(const std::string& username, const std::string& password, const std::string& email, std::function<void(const drogon::HttpResponsePtr)> callback) {
	auto clients = drogon::app().getDbClient();
	clients->execSqlAsync(
		"SELECT * FROM users WHERE username=$1 OR email=$2",
		[clients, callback , username , email , password](const Result& result) {
			if (!result.empty()) {
				callback(json_parse("error", "username or email already exits", drogon::k400BadRequest));
			}
			else {
				std::string password_hash = hash_password(password);
				if (password_hash.empty()) { callback(json_parse("error", "db_cp error", drogon::k500InternalServerError));return; }
				clients->execSqlAsync(
					"INSERT INTO users (username, password_hash, email) VALUES ($1, $2, $3)",
					[clients, callback, username, email , password_hash](const Result& result) {
						send_mail(email, username, [callback](const drogon::HttpResponsePtr json) {
							callback(json);
							});
					},
					[callback, password_hash](const DrogonDbException& e) {
						callback(json_parse("error","registerd fail",drogon::k500InternalServerError));
					},
					username, password_hash ,email
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