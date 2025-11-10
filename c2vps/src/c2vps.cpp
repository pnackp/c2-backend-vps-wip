/*main file*/

#include "c2vps.h"

using namespace Json;

//-func check special char?
auto is_blank = [](const std::string& s) {
	for (char c : s) {
		if (c == ' ' || c == '/' || c == '\"' || c == '$') {
			return true;
		}
	}
	return false;
};

//-func create payload to callback return type is Httpresponse ptr
drogon::HttpResponsePtr json_parse(const std::string& status , const std::string& message , const drogon::HttpStatusCode& code) {
	Value json;
	json["status"] = status;
	json["message"] = message;
	drogon::HttpResponsePtr resp = drogon::HttpResponse::newHttpJsonResponse(json);
	resp->setStatusCode(code);
	return resp;
}

//-func check back end is working?
void status(const drogon::HttpRequestPtr& req,std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
	callback(json_parse("ok","server working",drogon::k200OK));
	return;
}


void user_regis(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
	const std::shared_ptr<Value> payload = req->getJsonObject();
	if (!payload ||!payload->isMember("username") || !payload->isMember("password") || !payload->isMember("email")) {
		callback(json_parse("error", "missing argruments", drogon::k400BadRequest));
		return;
	}
	const std::string username = (*payload)["username"].as<std::string>();
	const std::string password = (*payload)["password"].as<std::string>();
	const std::string email = (*payload)["email"].as<std::string>();
	if (is_blank(username) || is_blank(password) || is_blank(email)) {
		callback(json_parse("error", "Missing Arguments", drogon::k400BadRequest));
		return;
	}
	user_insert(username, password, email,[callback](const drogon::HttpResponsePtr json) {
		callback(json);
	});
	return;
}

int main() {
	try {
		drogon::app().loadConfigFile("./config.json");
	}
	catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
	}
	drogon::app().registerHandler("/api/status", &status, { drogon::Get })
		.registerHandler("/api/register", &user_regis, { drogon::Post });
	drogon::app().run();
	return 0;
}