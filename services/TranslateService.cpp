#include "services/TranslateService.hpp"

using namespace services;

TranslateService::TranslateService(
        repositories::WordRepository wordRepository,
        repositories::TranslateRepository translateRepository
)
    : _wordRepository(std::move(wordRepository)),
      _translateRepository(std::move(translateRepository))
{
}

drogon::Task<models::id> TranslateService::addTranslate(dto::TranslateCreation translate)
{
    std::transform(
            translate.word.content.begin(),
            translate.word.content.end(),
            translate.word.content.begin(),
            [](char s) { return std::tolower(s); }
    );

    std::transform(
            translate.translate.content.begin(),
            translate.translate.content.end(),
            translate.translate.content.begin(),
            [](char s) { return std::tolower(s); }
    );

    std::transform(
            translate.word.code.begin(),
            translate.word.code.end(),
            translate.word.code.begin(),
            [](char s) { return std::toupper(s); }
    );

    std::transform(
            translate.translate.code.begin(),
            translate.translate.code.end(),
            translate.translate.code.begin(),
            [](char s) { return std::toupper(s); }
    );

    if (translate.translate.code == translate.word.code)
    {
        throw ValidationException("Нельзя создавать перевод с одинаковым языковым кодом");
    }

    if (!models::LanguageCode::isLangCodeExist(translate.word.code))
    {
        throw ValidationException(
                std::string("Не существует языка с таким кодом: " + translate.word.code)
        );
    }

    if (!models::LanguageCode::isLangCodeExist(translate.translate.code))
    {
        throw ValidationException(
                std::string("Не существует языка с таким кодом: " + translate.translate.code)
        );
    }

    utils::string::trim(translate.word.content);
    utils::string::trim(translate.translate.content);

    static const std::regex pattern(R"([^\sA-Za-zа-яА-Я])");

    translate.word.content = std::regex_replace(translate.word.content, pattern, "");
    translate.translate.content = std::regex_replace(translate.translate.content, pattern, "");

    static const std::regex patternClearSpace(R"(\s{2,})");

    translate.word.content = std::regex_replace(translate.word.content, patternClearSpace, " ");
    translate.translate.content =
            std::regex_replace(translate.translate.content, patternClearSpace, " ");

    if (translate.word.content.empty() || translate.translate.content.empty())
    {
        throw ValidationException("Слово не может быть пустым");
    }

    std::optional<models::Word> word =
            co_await this->_wordRepository.getByContent(translate.word.content);
    models::id wordId = 0;
    if (word.has_value() && word->languageCode == translate.word.code)
    {
        wordId = word->id;
    }
    else
    {
        models::Word word;
        word.content = translate.word.content;
        word.languageCode = translate.word.code;

        wordId = co_await this->_wordRepository.create(word);
    }

    std::optional<models::Word> translateWord =
            co_await this->_wordRepository.getByContent(translate.translate.content);
    models::id translateWordId = 0;
    if (translateWord.has_value() && translateWord->languageCode == translate.translate.code)
    {
        translateWordId = translateWord->id;
    }
    else
    {
        models::Word word;
        word.content = translate.translate.content;
        word.languageCode = translate.translate.code;

        translateWordId = co_await this->_wordRepository.create(word);
    }

    if (wordId == translateWordId)
    {
        throw ValidationException("Нельзя использовать одинаковые слова");
    }

    models::Translate translateModel;
    translateModel.userId = translate.userId;
    translateModel.firstWordId = std::min(wordId, translateWordId);
    translateModel.secondWordId = std::max(wordId, translateWordId);
    translateModel.createdAt = std::chrono::system_clock::now();

    co_return co_await this->_translateRepository.create(translateModel);
}

drogon::Task<void> TranslateService::removeTranslate(const dto::RemoveTranslate& translate)
{
    co_await this->_translateRepository.remove(
            translate.firstWordId, translate.secondWordId, translate.userId
    );
}

drogon::Task<void> TranslateService::updateTranslate(const models::Translate& translate)
{
    co_await this->_translateRepository.update(translate);
}

drogon::Task<std::vector<models::Word>>
TranslateService::getTranslates(models::id wordId, models::id userId)
{
    co_return co_await this->_wordRepository.getTranslates(wordId, userId);
}

drogon::Task<std::optional<std::pair<models::Word, models::Word>>>
TranslateService::get(models::id translateId)
{
    std::optional<models::Translate> translate =
            co_await this->_translateRepository.get(translateId);
    if (!translate.has_value())
    {
        co_return std::nullopt;
    }

    std::optional<models::Word> wordA = co_await this->_wordRepository.get(translate->firstWordId);
    std::optional<models::Word> wordB = co_await this->_wordRepository.get(translate->secondWordId);

    co_return std::make_optional(std::make_pair(wordA.value(), wordB.value()));
}
