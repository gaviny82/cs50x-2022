ALTER TABLE transactions RENAME TO _transactions_old;

CREATE TABLE transactions (
    id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    user_id INTEGER NOT NULL,
    symbol TEXT NOT NULL,
    shares INTEGER NOT NULL,
    price NUMERIC NOT NULL,
    time NUMERIC NOT NULL,

    FOREIGN KEY (user_id) REFERENCES users(id)
);

INSERT INTO transactions (id, user_id, symbol, shares, price, time)
  SELECT id, user_id, symbol, shares, price, time
  FROM _transactions_old;