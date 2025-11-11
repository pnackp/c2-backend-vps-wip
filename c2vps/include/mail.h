#pragma once
#include <curl/curl.h>
#include <cstring>
#include <iostream>

#include "c2vps.h"

#define MAIL_ACC ""
#define MAIL_APW ""

void send_mail(const std::string& email,
    const std::string& username,
    std::shared_ptr<std::function<void(const drogon::HttpResponsePtr&)>> cb);