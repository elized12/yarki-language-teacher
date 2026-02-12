CREATE TABLE train_session (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    user_id INTEGER NOT NULL,
    started_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    finished_at TIMESTAMP,
    settings JSONB NOT NULL
);

CREATE TABLE train_card_mode (
    id SERIAL PRIMARY KEY,
    name VARCHAR(100) NOT NULL UNIQUE
);

CREATE TABLE train_card (
    id SERIAL PRIMARY KEY,
    train_session_id UUID,
    train_card_mode_id INTEGER NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    position INTEGER NOT NULL,
    other_params JSONB,
    target_word_id INTEGER,

    FOREIGN KEY (target_word_id)
    REFERENCES word(id),

    FOREIGN KEY (train_card_mode_id)
    REFERENCES train_card_mode(id),

    FOREIGN KEY (train_session_id)
    REFERENCES train_session(id) ON DELETE CASCADE    
);

CREATE TABLE train_user_answer (
    id SERIAL PRIMARY KEY,
    train_card_id INTEGER NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    content TEXT,
    is_correct BOOLEAN NOT NULL,

    FOREIGN KEY (train_card_id)
    REFERENCES train_card(id) ON DELETE CASCADE
);