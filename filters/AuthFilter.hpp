#pragma once

#include <drogon/HttpAppFramework.h>
#include <drogon/HttpFilter.h>
#include <nlohmann/json.hpp>

#include "services/AuthService.hpp"

using namespace drogon;

class AuthFilter : public HttpFilter<AuthFilter>
{
  private:
    services::AuthService _service;

  public:
    AuthFilter();

    void
    doFilter(const HttpRequestPtr& req, FilterCallback&& fcb, FilterChainCallback&& fccb) override;
};
