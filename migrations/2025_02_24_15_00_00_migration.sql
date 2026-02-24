ALTER TABLE train_card
RENAME COLUMN target_word_id TO source_word_id;

UPDATE train_card_mode SET name = 'source_to_target_input' WHERE name = 'text_input';
UPDATE train_card_mode SET name = 'target_to_source_input' WHERE name = 'cards';

INSERT INTO train_card_mode (name) VALUES 
    ('source_to_target_select'), 
    ('target_to_source_select');