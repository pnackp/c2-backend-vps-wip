#include "mail.h"

struct upload_status {
	size_t bytes_read;
	const char* payload;
};

static size_t read_cb(char* ptr, size_t size, size_t nmemb, void* userp) {
	struct upload_status* upload_ctx = (struct upload_status*)userp;
	const char* data;
	size_t room = size * nmemb;
	size_t len;

	if (size == 0 || nmemb == 0 || (size * nmemb) < 1)
		return 0;

	data = &upload_ctx->payload[upload_ctx->bytes_read];
	len = strlen(data);

	if (room < len)
		len = room;

	memcpy(ptr, data, len);
	upload_ctx->bytes_read += len;

	return len;
}

void send_mail(const std::string& email,
	const std::string& username,
	std::shared_ptr<std::function<void(const drogon::HttpResponsePtr&)>> cb) {

	std::string token_email = token(email, username);
	if (token_email.empty()) {
		auto resp = json_parse("error", "hash_token error", drogon::k500InternalServerError);
		(*cb)(resp);
		return;
	}

	std::string verify_link = "https://localhost:8080/api/verify_email?token=" + token_email;
	std::string payload =
		"To: " + email + "\r\n"
		"From:" + MAIL_ACC + "\r\n"
		"Subject: Verify your email from CC VPS\r\n"
		"Content-Type: text/html; charset=utf-8\r\n"
		"\r\n"
		"<html><body>"
		"<h2>Hello!</h2>"
		"<p>Click the button below to verify your email:</p>"
		"<a href='" + verify_link + "' "
		"style='display:inline-block; padding:10px 20px; background-color:#4CAF50; color:white; "
		"text-decoration:none; border-radius:5px;'>Verify Email</a>"
		"<p>If you didn’t request this, you can ignore this email.</p>"
		"</body></html>";

	CURL* curl = curl_easy_init();
	if (curl) {
		struct curl_slist* recipients = nullptr;
		struct upload_status upload_ctx = { 0, payload.c_str() };

		curl_easy_setopt(curl, CURLOPT_URL, "smtps://smtp.gmail.com:465");
		curl_easy_setopt(curl, CURLOPT_USERNAME, MAIL_ACC);
		curl_easy_setopt(curl, CURLOPT_PASSWORD, MAIL_APW);
		curl_easy_setopt(curl, CURLOPT_MAIL_FROM, MAIL_ACC);

		recipients = curl_slist_append(recipients, email.c_str());
		curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

		curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_cb);
		curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
		curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

		CURLcode res = curl_easy_perform(curl);

		if (res != CURLE_OK) {
			auto resp = json_parse("error", curl_easy_strerror(res), drogon::k500InternalServerError);
			(*cb)(resp);
		}
		else {
			auto resp = json_parse("ok", "email sent successfully", drogon::k200OK);
			(*cb)(resp);
		}

		curl_slist_free_all(recipients);
		curl_easy_cleanup(curl);
	}
	return;
}