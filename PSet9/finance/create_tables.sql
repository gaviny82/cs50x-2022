CREATE TABLE users (
    id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    username TEXT NOT NULL,
    hash TEXT NOT NULL,
    cash NUMERIC NOT NULL DEFAULT 0
);

CREATE TABLE transactions (
    id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    user_id INTEGER NOT NULL,
    symbol TEXT NOT NULL,
    shares INTEGER NOT NULL,
    price NUMERIC NOT NULL,
    time NUMERIC NOT NULL,

    FOREIGN KEY (user_id) REFERENCES users(id)
);

CREATE TABLE portfolios (
    id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    user_id INTEGER NOT NULL,
    symbol TEXT NOT NULL,
    shares INTEGER NOT NULL,

    FOREIGN KEY(user_id) REFERENCES users(id)
);

CREATE UNIQUE INDEX username ON users (username);
CREATE UNIQUE INDEX id ON users (id);
CREATE INDEX symbol ON portfolios (symbol);
CREATE INDEX user_id ON portfolios (user_id);

CREATE TABLE cached_names (
    symbol TEXT NOT NULL,
    name TEXT NOT NULL,

    PRIMARY KEY(symbol)
);