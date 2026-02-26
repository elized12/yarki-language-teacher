#include "AutoMigration.hpp"

using namespace utils;

AutoMigration::AutoMigration(
    const std::string &host,
    const std::string &port,
    const std::string &dbname,
    const std::string &user,
    const std::string &password)
    : _conn(nullptr, &PQfinish)
{
    std::string conninfo = std::format(
        "host={} port={} dbname={} user={} password={}",
        host, port, dbname, user, password);

    PGconn *raw = PQconnectdb(conninfo.c_str());
    if (!raw || PQstatus(raw) != CONNECTION_OK)
    {
        std::string error = raw ? PQerrorMessage(raw)
                                : "Failed to allocate PGconn";

        if (raw)
        {
            PQfinish(raw);
        }

        throw std::runtime_error(error);
    }

    this->_conn.reset(raw);
}

AutoMigration::~AutoMigration()
{
    this->close();
}

void AutoMigration::close()
{
    if (this->_conn)
    {
        this->_conn.reset();
    }
}

void AutoMigration::execSimple(const std::string &sql)
{
    PGresult *res = PQexec(_conn.get(), sql.c_str());
    if (!res)
    {
        throw std::runtime_error("PQexec returned null");
    }

    ExecStatusType status = PQresultStatus(res);
    if (status != PGRES_COMMAND_OK &&
        status != PGRES_TUPLES_OK)
    {
        std::string error = PQerrorMessage(_conn.get());
        PQclear(res);
        throw std::runtime_error(error);
    }

    PQclear(res);
}

bool AutoMigration::performMigrations(const Json::Value &config)
{
    if (!config["path_migration"].isString())
    {
        return false;
    }

    std::string path = config["path_migration"].asString();
    if (!std::filesystem::exists(path))
    {
        throw std::runtime_error("Migration directory not found");
    }

    std::regex pattern(
        R"(\d{4}_\d{2}_\d{2}_\d{2}_\d{2}_\d{2}_migration\.sql)");
    if (config["regex_migration"].isString())
    {
        pattern = std::regex(config["regex_migration"].asString());
    }

    try
    {
        this->execSimple("SELECT pg_advisory_lock(987654321);");

        this->createMigrationTable();

        auto candidates = this->getFileNames(path, pattern);
        std::sort(candidates.begin(), candidates.end());

        auto completed = this->getMigrationNames();
        std::sort(completed.begin(), completed.end());

        std::vector<std::string> pending;
        std::set_difference(
            candidates.begin(), candidates.end(),
            completed.begin(), completed.end(),
            std::back_inserter(pending));

        for (const auto &migration : pending)
        {
            this->executeMigration(path + "/" + migration);
            std::cout << std::format(
                             "[INFO] Migration Success \"{}\"",
                             migration)
                      << std::endl;
        }

        this->execSimple("SELECT pg_advisory_unlock(987654321);");

        return true;
    }
    catch (const MigrateException &ex)
    {
        std::cerr << std::format(
                         "[ERROR] Migration Failed \"{}\": {}",
                         ex.migrationName,
                         ex.what())
                  << std::endl;

        this->execSimple("SELECT pg_advisory_unlock(987654321);");
        return false;
    }
    catch (const std::exception &ex)
    {
        std::cerr << std::format(
                         "[ERROR] Migration Failed: {}",
                         ex.what())
                  << std::endl;

        this->execSimple("SELECT pg_advisory_unlock(987654321);");
        return false;
    }
}

void AutoMigration::createMigrationTable()
{
    execSimple(R"(
        CREATE TABLE IF NOT EXISTS migrations(
            id SERIAL PRIMARY KEY,
            name VARCHAR(500) NOT NULL UNIQUE,
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP NOT NULL
        );
    )");
}

std::vector<std::string> AutoMigration::getMigrationNames()
{
    PGresult *res = PQexec(
        this->_conn.get(),
        "SELECT name FROM migrations ORDER BY id ASC");

    if (!res)
    {
        throw std::runtime_error("PQexec returned null");
    }

    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        std::string error = PQerrorMessage(this->_conn.get());
        PQclear(res);
        throw std::runtime_error(error);
    }

    std::vector<std::string> names;

    int rows = PQntuples(res);
    for (int i = 0; i < rows; ++i)
    {
        names.emplace_back(PQgetvalue(res, i, 0));
    }

    PQclear(res);

    return names;
}

std::vector<std::string> AutoMigration::getFileNames(
    const std::string &path,
    const std::regex &pattern)
{
    std::vector<std::string> fileNames;
    for (const auto &entry :
         std::filesystem::directory_iterator(path))
    {
        if (std::regex_search(
                entry.path().filename().string(),
                pattern))
        {
            fileNames.push_back(
                entry.path().filename().string());
        }
    }

    return fileNames;
}

void AutoMigration::executeMigration(const std::string &path)
{
    std::ifstream file(path, std::ios::binary);
    if (!file)
    {
        throw MigrateException(
            "Cannot open file",
            std::filesystem::path(path).filename().string());
    }

    file.seekg(0, std::ios::end);
    std::string sql;
    sql.resize(file.tellg());
    file.seekg(0);
    file.read(sql.data(), sql.size());

    std::string filename =
        std::filesystem::path(path).filename().string();

    try
    {
        execSimple("BEGIN");

        execSimple(sql);

        const char *paramValues[1] = {filename.c_str()};

        PGresult *res = PQexecParams(
            this->_conn.get(),
            "INSERT INTO migrations(name) VALUES($1)",
            1,
            nullptr,
            paramValues,
            nullptr,
            nullptr,
            0);

        if (!res ||
            PQresultStatus(res) != PGRES_COMMAND_OK)
        {
            std::string error =
                PQerrorMessage(this->_conn.get());

            if (res)
                PQclear(res);

            throw std::runtime_error(error);
        }

        PQclear(res);

        execSimple("COMMIT");
    }
    catch (const std::exception &ex)
    {
        execSimple("ROLLBACK");
        throw MigrateException(ex.what(), filename);
    }
}