#pragma once

#include <postgresql/libpq-fe.h>
#include <json/json.h>

#include <vector>
#include <regex>
#include <filesystem>
#include <algorithm>
#include <fstream>
#include <format>
#include <memory>
#include <stdexcept>
#include <iostream>

namespace utils
{

  class MigrateException : public std::runtime_error
  {
  public:
    std::string migrationName;

  public:
    MigrateException(
        const std::string &message,
        const std::string &migration)
        : std::runtime_error(message),
          migrationName(migration)
    {
    }
  };

  class AutoMigration
  {
  private:
    std::unique_ptr<PGconn, decltype(&PQfinish)> _conn;

  public:
    AutoMigration(
        const std::string &host,
        const std::string &port,
        const std::string &dbname,
        const std::string &user,
        const std::string &password);
    ~AutoMigration();

  public:
    bool performMigrations(const Json::Value &config);
    void close();

  private:
    void execSimple(const std::string &sql);
    void createMigrationTable();
    std::vector<std::string> getMigrationNames();
    std::vector<std::string> getFileNames(const std::string &path, const std::regex &pattern);
    void executeMigration(const std::string &path);
  };

};