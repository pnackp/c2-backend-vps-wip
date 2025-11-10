#pragma once
#include <curl/curl.h>
#include <cstring>
#include <iostream>

#include "c2vps.h"

#define MAIL_ACC "pnackp@gmail.com"
#define MAIL_APW "tuxk fbpw lftz hvmj "

void send_mail(const std::string& email, const std::string& username , std::function<void(drogon::HttpResponsePtr)>callback);