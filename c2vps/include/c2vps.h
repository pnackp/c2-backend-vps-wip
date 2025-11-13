/**/

#pragma once

#include <iostream>
#include <drogon/drogon.h>
#include <expected>
#include <drogon/HttpFilter.h>
#include "db_manage.h"

#define COOKIES_NAME "login_cookies"

drogon::HttpResponsePtr json_parse(const std::string& status, const std::string& message, const drogon::HttpStatusCode& code);