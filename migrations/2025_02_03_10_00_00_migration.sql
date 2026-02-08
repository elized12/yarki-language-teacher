CREATE TABLE "language" (
    id SERIAL PRIMARY KEY,
    code VARCHAR(10) UNIQUE,
    name VARCHAR(100),
    
    CHECK (code ~ '^[a-z]{2,5}$')
);

CREATE INDEX idx_language_code ON "language"(code);

CREATE TABLE word (
    id SERIAL PRIMARY KEY,
    language_id INT,
    content TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

ALTER TABLE "word" ADD CONSTRAINT fk_word_language
FOREIGN KEY (language_id) REFERENCES "language"(id) ON DELETE CASCADE;

CREATE INDEX idx_word_language_id ON "word"(language_id);

CREATE TABLE translation(
    id SERIAL PRIMARY KEY,
    word_a_id INT NOT NULL,
    word_b_id INT NOT NULL,
    user_id INT NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,

    CHECK(word_a_id < word_b_id),
    UNIQUE(word_a_id, word_b_id, user_id)
);

ALTER TABLE translation
ADD CONSTRAINT fk_translation_word_a
FOREIGN KEY (word_a_id) REFERENCES word(id) ON DELETE CASCADE;

ALTER TABLE translation
ADD CONSTRAINT fk_translation_word_b
FOREIGN KEY (word_b_id) REFERENCES word(id) ON DELETE CASCADE;

ALTER TABLE translation ADD CONSTRAINT fk_translation_user
FOREIGN KEY (user_id) REFERENCES "user"(id) ON DELETE CASCADE;