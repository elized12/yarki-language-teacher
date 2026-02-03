CREATE TABLE "user" (
    id SERIAL PRIMARY KEY,
    nickname VARCHAR(20) NOT NULL,
    email VARCHAR(200) NOT NULL UNIQUE,
    hashed_password VARCHAR(1000) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX idx_user_email ON "user"(email);

CREATE TABLE token (
    id SERIAL PRIMARY KEY,
    user_id INTEGER NOT NULL,
    hashed_token VARCHAR(1000) NOT NULL,
    issued_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    expired_at TIMESTAMP NOT NULL,
    revoked_at TIMESTAMP NULL

);

ALTER TABLE token 
ADD CONSTRAINT fk_token_user 
FOREIGN KEY (user_id) 
REFERENCES "user"(id) 
ON DELETE CASCADE;

CREATE INDEX idx_token_user_id ON token(user_id);
CREATE INDEX idx_token_hashed_token ON token(hashed_token);

CREATE INDEX idx_token_expires ON token(expired_at);
CREATE INDEX idx_token_issued ON token(issued_at);