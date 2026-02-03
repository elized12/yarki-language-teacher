#pragma once

#include "repositories/WordRepository.hpp"
#include "services/ValidataionException.hpp"

namespace services
{

class WordService
{
  private:
    repositories::WordRepository _wordRepository;

  public:
    WordService(repositories::WordRepository wordRepository);

  public:
    drogon::Task<std::vector<models::Word>>
    get(models::id userId, const std::string& code, ssize_t limit, ssize_t offset);

    drogon::Task<ssize_t> getCountWord(models::id userId, const std::string& code);
};

} // namespace services