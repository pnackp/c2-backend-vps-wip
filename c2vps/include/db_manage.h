#pragma once
#include <functional>
#include <iostream>

#include <drogon/orm/DbClient.h>
#include <drogon/orm/Result.h>
#include <drogon/orm/Exception.h>


#include "c2vps.h"
#include "hash.h"
#include "mail.h"

void user_insert(std::string& username, std::string& password, std::string& email, std::function<void(const drogon::HttpResponsePtr&)>&& callback);
void user_login(std::string& username, std::string& password, std::function<void(drogon::HttpResponsePtr&)>&& callback);