CREATE TABLE Records (
    id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    categoryId INTEGER,
    record TEXT,
    FOREIGN KEY(categoryId) REFERENCES Categories(id)
)
