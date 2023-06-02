import os

from datetime import datetime
from cs50 import SQL
from flask import Flask, flash, redirect, render_template, request, session
from flask_session import Session
from tempfile import mkdtemp
from werkzeug.security import check_password_hash, generate_password_hash

from helpers import apology, login_required, lookup, usd, get_name

# export API_KEY=pk_84e1f5f051bb44a2afccec8edcf8d540

# Configure application
app = Flask(__name__)

# Ensure templates are auto-reloaded
app.config["TEMPLATES_AUTO_RELOAD"] = True

# Custom filter
app.jinja_env.filters["usd"] = usd

# Configure session to use filesystem (instead of signed cookies)
app.config["SESSION_PERMANENT"] = False
app.config["SESSION_TYPE"] = "filesystem"
Session(app)

# Configure CS50 Library to use SQLite database
db = SQL("sqlite:///finance.db")
cached_names = {}
for entry in db.execute("SELECT * FROM cached_names"):
    cached_names[entry["symbol"]] = entry["name"]

# Make sure API key is set
if not os.environ.get("API_KEY"):
    raise RuntimeError("API_KEY not set")


@app.after_request
def after_request(response):
    """Ensure responses aren't cached"""
    response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    response.headers["Expires"] = 0
    response.headers["Pragma"] = "no-cache"
    return response


@app.route("/")
@login_required
def index():
    """Show portfolio of stocks"""

    entries = db.execute("SELECT symbol, shares FROM portfolios WHERE user_id=? ORDER BY symbol", session["user_id"])
    total = 0

    portfolio = []
    for item in entries:

        # Get the quote for a symbol
        quote = lookup(item["symbol"])

        # Ignore invalid symbols and log the issue
        if quote is None:
            app.logger.warning(f"""Cannot get quote. Name: {item["symbol"]}""")
            continue

        item["name"] = quote["name"]
        item["price"] = usd(quote["price"])
        value = quote["price"] * item["shares"]
        item["total"] = usd(value)
        total += value
        portfolio.append(item)

    cash = db.execute("SELECT cash FROM users WHERE id=?", session["user_id"])[0]["cash"]

    return render_template("index.html", portfolio=portfolio, cash=usd(cash), total=usd(total))


@app.route("/buy", methods=["GET", "POST"])
@login_required
def buy():
    """Buy shares of stock"""

    if request.method == "POST":
        symbol = request.form["symbol"]
        shares = request.form["shares"]
        quote = lookup(symbol)

        # Check for invalid symbols and shares
        if quote is None:
            return apology("Please provide a valid symbol.")
        try:
            if '.' in shares:
                raise
            shares = int(shares)
            if shares <= 0:
                raise
        except:
            return apology("Shares must be a positive integer.")

        cash = db.execute("SELECT cash FROM users WHERE id=?", session["user_id"])[0]["cash"]
        total_price = quote["price"] * shares

        # Not enough cash
        if cash < total_price:
            return apology(f"Failed to buy {shares} {symbol}: not enough cash.")

        # Process transaction
        cash -= total_price
        db.execute("UPDATE users SET cash=? WHERE id=?", cash, session["user_id"])

        # Add shares to portfolio
        entry = db.execute("SELECT * FROM portfolios WHERE user_id=? AND symbol=?", session["user_id"], symbol)
        if len(entry) == 0:
            db.execute("INSERT INTO portfolios (user_id, symbol, shares) VALUES(?, ?, ?)", session["user_id"], symbol, shares)
        elif len(entry) == 1:
            db.execute("UPDATE portfolios SET shares=? WHERE user_id=? AND symbol=?", entry[0]["shares"] + shares, session["user_id"], symbol)
        else:
            raise

        db.execute("INSERT INTO transactions (user_id, symbol, shares, price, time) VALUES(?, ?, ?, ?, ?)", session["user_id"], symbol, shares, quote["price"], datetime.timestamp(datetime.utcnow()))

        app.logger.info(f"""User id={session["user_id"]} has purchased {shares} stocks of {symbol}.""")
        return redirect("/")

    else:
        return render_template("buy.html")


@app.route("/history")
@login_required
def history():
    """Show history of transactions"""

    entries = db.execute("SELECT id, symbol, shares, price, time FROM transactions WHERE user_id=? ORDER BY time", session["user_id"])
    balance = 0

    transactions = []
    for transaction in entries:
        transaction["time"] = datetime.fromtimestamp(transaction["time"]).strftime("%H:%M:%S, %Y/%d/%m")

        # TOP UP transaction
        if transaction["symbol"] == "__TOP_UP__":
            balance += transaction["price"]
            transaction["price"] = usd(transaction["price"])
            transactions.append(transaction)
            continue

        # Get the name for a symbol
        name = get_name(transaction["symbol"], cached_names, db)

        # Ignore invalid symbols and log the issue
        if name is None:
            app.logger.warning(f"""Cannot get name for {transaction["symbol"]} using IEX API.\n""")
            continue

        value = transaction["price"] * abs(transaction["shares"])

        transaction["name"] = name
        transaction["price"] = usd(transaction["price"])

        if transaction["shares"] > 0:
            transaction["operation"] = "BUY"
            transaction["credit"] = usd(value)
            balance -= value
        else:
            transaction["operation"] = "SELL"
            transaction["debit"] = usd(value)
            balance += value

        transaction["shares"] = abs(transaction["shares"])
        transactions.append(transaction)


    return render_template("history.html", transactions=transactions, balance=usd(balance), now=datetime.utcnow().strftime("%H:%M:%S, %Y/%d/%m"))


@app.route("/login", methods=["GET", "POST"])
def login():
    """Log user in"""

    # Forget any user_id
    session.clear()

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        # Ensure username was submitted
        if not request.form.get("username"):
            return apology("must provide username", 403)

        # Ensure password was submitted
        elif not request.form.get("password"):
            return apology("must provide password", 403)

        # Query database for username
        rows = db.execute("SELECT * FROM users WHERE username = ?", request.form.get("username"))

        # Ensure username exists and password is correct
        if len(rows) != 1 or not check_password_hash(rows[0]["hash"], request.form.get("password")):
            return apology("invalid username and/or password", 403)

        # Remember which user has logged in
        session["user_id"] = rows[0]["id"]

        # Redirect user to home page
        return redirect("/")

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("login.html")


@app.route("/logout")
def logout():
    """Log user out"""

    # Forget any user_id
    session.clear()

    # Redirect user to login form
    return redirect("/")


@app.route("/quote", methods=["GET", "POST"])
@login_required
def quote():

    if request.method == "POST":
        name = request.form["symbol"]
        if not name:
            return f"Please provide a symbol."

        quote = lookup(name)
        if quote == None:
            return f"Cannot quote symbol {name}."
        else:
            quote["price"] = usd(quote["price"])
            return render_template("quoted.html", quote=quote)

    else:
        return render_template("quote.html")


@app.route("/register", methods=["GET", "POST"])
def register():
    """Register user"""

    # Process registration request
    if request.method == "POST":
        username = request.form["username"]
        password1 = request.form["password"]
        password2 = request.form["confirmation"]

        # Check for empty username
        if not username:
            return apology(f"Unable to create an account: require a username.")

        # Check for mismatching/empty password
        if password1 != password2:
            return apology(f"Unable to create an account: mismatching password.")
        if not password1:
            return apology(f"Unable to create an account: require a password.")

        # Check for duplicate username
        existing_un = db.execute("SELECT id FROM users WHERE username=?", username)
        if len(existing_un) > 0:
            return apology(f"Unable to create an account: username {username} already exists.")

        # Register the user
        hash = generate_password_hash(password1)
        db.execute("INSERT INTO users (username, hash) VALUES(?, ?)", username, hash)
        return redirect("/login")

    # Display Register page
    else:
        return render_template("register.html")


@app.route("/sell", methods=["GET", "POST"])
@login_required
def sell():
    """Sell shares of stock"""

    if request.method == "POST":
        symbol = request.form["symbol"]
        shares = request.form["shares"]
        quote = lookup(symbol)

        # Check for invalid symbols and shares
        if quote is None:
            return apology("Please provide a valid symbol.")
        try:
            if '.' in shares:
                raise
            shares = int(shares)
            if shares <= 0:
                raise
        except:
            return apology("Shares must be a positive integer.")

        # Check if the user has enough shares to sell
        entry = db.execute("SELECT * FROM portfolios WHERE user_id=? AND symbol=?", session["user_id"], symbol)
        if len(entry) == 0:
            return apology(f"You don't have any share for {symbol} in your portfolio.")

        shares_in_portfolio = entry[0]["shares"]
        if shares > shares_in_portfolio:
            return apology(f"You don't have enough shares for {symbol}. You only have {shares_in_portfolio}.")
        elif shares == shares_in_portfolio:
            db.execute("DELETE FROM portfolios WHERE user_id=? AND symbol=?", session["user_id"], symbol)
        else:
            db.execute("UPDATE portfolios SET shares=? WHERE user_id=? AND symbol=?", shares_in_portfolio - shares, session["user_id"], symbol)

        # Record transaction
        db.execute("INSERT INTO transactions (user_id, symbol, shares, price, time) VALUES(?, ?, ?, ?, ?)", session["user_id"], symbol, -shares, quote["price"], datetime.timestamp(datetime.utcnow()))
        return redirect("/")

    else:
        return render_template("sell.html")

@app.route("/topup", methods=["GET", "POST"])
@login_required
def topup():
    if request.method == "POST":
        amount = request.form["amount"]
        try:
            amount = round(float(amount), 2)
            if amount <= 0:
                raise
        except:
            return apology("Invalid amount")

        cash = db.execute("SELECT cash FROM users WHERE id=?", session["user_id"])[0]["cash"]
        print(cash, amount)
        cash += amount
        print(cash)
        db.execute("UPDATE users SET cash=? WHERE id=?", cash, session["user_id"])
        db.execute("INSERT INTO transactions (user_id, symbol, shares, price, time) VALUES(?, ?, ?, ?, ?)", session["user_id"], "__TOP_UP__", 1, amount, datetime.timestamp(datetime.utcnow()))

        return redirect("/")
    else:
        return render_template("topup.html")