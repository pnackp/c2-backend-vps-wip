#pragma once
#include <functional>
#include <iostream>

#include <drogon/orm/DbClient.h>
#include <drogon/orm/Result.h>
#include <drogon/orm/Exception.h>


#include "c2vps.h"
#include "hash.h"
#include "mail.h"

void user_insert(const std::string& username, const std::string& password, const std::string&  email, std::function<void(const drogon::HttpResponsePtr)> callback);
void user_login(const std::string& username, const std::string& password , std::function<void(drogon::HttpResponsePtr)>callback);