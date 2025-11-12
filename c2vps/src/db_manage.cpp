#include "db_manage.h"

using namespace drogon::orm;

void user_insert( std::string& username,  std::string& password, std::string& email, std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
	auto clients = drogon::app().getDbClient();

	std::shared_ptr< std::function<void(const drogon::HttpResponsePtr&)>> cb = std::make_shared<std::function<void(const drogon::HttpResponsePtr&)>>(
		std::move(callback)
	);

	std::string password_hash = hash_password(password);
	if (password_hash.empty()) {
		(*cb)(json_parse("error", "hash password failed", drogon::k500InternalServerError));
		return;
	}

	clients->execSqlAsync(
		"INSERT INTO users (username, password_hash, email) "
		"VALUES ($1, $2, $3) "
		"ON CONFLICT (username) DO NOTHING RETURNING email",
		[&clients, cb, username = std::move(username), email = std::move(email)](const Result& r) {
			if (r.empty()) {
				auto res = json_parse("error", "username or email already exists", drogon::k400BadRequest);
				(*cb)(res);
				return;
			}
			std::future _ = std::async(std::launch::async, send_mail, email, username, cb);
		},
		[callback , cb](const DrogonDbException& e) {
			auto res = json_parse("error", "database insert failed", drogon::k500InternalServerError);
			(*cb)(res);
		},
		username, password_hash, email
	);
	return;
}

void user_login(std::string& username, std::string& password, std::function<void(drogon::HttpResponsePtr&)>&& callback) {
	auto clients = drogon::app().getDbClient();
	auto cb = std::make_shared<std::function<void(drogon::HttpResponsePtr&)>>(std::move(callback));

	clients->execSqlAsync(
		"SELECT password_hash , is_email_verify FROM users WHERE username=$1",
		[clients, password = std::move(password), username = std::move(username), cb](const Result& result) {
			drogon::HttpResponsePtr res;
			if (result.empty()) {
				res = json_parse("error", "Incorrect username or password", drogon::k400BadRequest);
				(*cb)(res);
				return;
			}
			const std::string hash = result[0]["password_hash"].as<std::string>();
			const bool verified = result[0]["is_email_verify"].as<bool>();
			if (!verify_pw(password, hash)) {
				res = json_parse("error", "Incorrect username or password", drogon::k400BadRequest);
				(*cb)(res);
				return;
			}
			if (!verified) {
				res = json_parse("error", "Email not verified", drogon::k400BadRequest);
				(*cb)(res);
				return;
			}
			//
			auto resp = drogon::HttpResponse::newHttpResponse();

			std::string jwtToken = token("", username);
			drogon::Cookie myCookie("login_cookie", jwtToken);
			myCookie.setDomain("c2vps.com");
			myCookie.setPath("/");
			myCookie.setMaxAge(3600);
			myCookie.setHttpOnly(true);
			myCookie.setSecure(true);

			resp->addCookie(myCookie);
			resp->setBody("Login success!");
			resp->setStatusCode(drogon::k200OK);
			(*cb)(res);
		},
		[cb](const DrogonDbException& e) {
			auto res = json_parse("error", "db went wrong", drogon::k500InternalServerError);
			(*cb)(res);
		},
		username
	);
}
