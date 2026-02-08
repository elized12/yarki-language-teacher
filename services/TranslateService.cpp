#include "services/TranslateService.hpp"

using namespace services;

TranslateService::TranslateService(
    repositories::WordRepository wordRepository,
    repositories::TranslateRepository translateRepository,
    services::WordCleaner cleaner)
    : _wordRepository(std::move(wordRepository)),
      _translateRepository(std::move(translateRepository)),
      _wordCleaner(std::move(cleaner))
{
}

drogon::Task<models::id> TranslateService::addTranslate(dto::TranslateCreation translate)
{
    translate.word.content = this->_wordCleaner.clean(translate.word.content);
    translate.translate.content = this->_wordCleaner.clean(translate.translate.content);

    if (translate.translate.code == translate.word.code)
        throw ValidationException("Нельзя создавать перевод с одинаковым языковым кодом");

    if (!models::LanguageCode::isLangCodeExist(translate.word.code))
        throw ValidationException("Не существует языка с таким кодом: " + translate.word.code);

    if (!models::LanguageCode::isLangCodeExist(translate.translate.code))
        throw ValidationException("Не существует языка с таким кодом: " + translate.translate.code);

    if (translate.word.content.empty() || translate.word.content.empty())
    {
        throw ValidationException("Слово не может быть пустым");
    }

    std::optional<models::Word> word = co_await this->_wordRepository.getByContent(translate.word.content);
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

    std::optional<models::Word> translateWord = co_await this->_wordRepository.getByContent(translate.translate.content);
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

    bool existTranslate = co_await this->existTranslate(translateModel.userId, translateModel.firstWordId, translateModel.secondWordId);
    if (existTranslate)
    {
        throw TranslateAlreadyExistException("этот перевод уже существует");
    }

    co_return co_await this->_translateRepository.create(translateModel);
}

drogon::Task<bool> TranslateService::removeTranslate(const dto::RemoveTranslate &translate)
{
    co_return co_await this->_translateRepository.remove(
        translate.firstWordId, translate.secondWordId, translate.userId);
}

drogon::Task<void> TranslateService::updateTranslate(const models::Translate &translate)
{
    co_await this->_translateRepository.update(translate);
}

drogon::Task<std::vector<models::Word>>
TranslateService::getTranslates(models::id wordId, models::id userId)
{
    std::optional<models::Word> existWord = co_await this->_wordRepository.get(wordId);
    if (!existWord.has_value())
    {
        throw services::ValidationException("Такого слова не существует");
    }

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

drogon::Task<bool> TranslateService::existTranslate(models::id userId, models::id firstWordId, models::id secondWordId)
{
    std::optional<models::Translate> translate = co_await this->_translateRepository.get(userId, firstWordId, secondWordId);
    co_return translate.has_value() ? true : false;
}