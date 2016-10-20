CREATE TABLE Categories (
    id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    category TEXT,
    CONSTRAINT unique_category UNIQUE (category)
)
