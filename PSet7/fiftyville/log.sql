-- Keep a log of any SQL queries you execute as you solve the mystery.

.schema
.timer on

SELECT id, description
  FROM crime_scene_reports
 WHERE year = 2021
   AND month = 7
   AND day = 28;
/*
+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| id  |                                                                                                       description                                                                                                        |
+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| 293 | Vandalism took place at 12:04. No known witnesses.                                                                                                                                                                       |
| 294 | Shoplifting took place at 03:01. Two people witnessed the event.                                                                                                                                                         |
| 295 | Theft of the CS50 duck took place at 10:15am at the Humphrey Street bakery. Interviews were conducted today with three witnesses who were present at the time – each of their interview transcripts mentions the bakery. |
| 296 | Money laundering took place at 20:30. No known witnesses.                                                                                                                                                                |
| 297 | Littering took place at 16:36. No known witnesses.                                                                                                                                                                       |
+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Notes: Theft took place at 10:15am => consider flights taken off shortly after 10:15am;
       Interviews conducted on 2021/7/28 => read transcripts for more details;
           3 witnesses, each transcript mentions 'bakery'
*/

SELECT name, transcript
  FROM interviews
 WHERE year = 2021
   AND month = 7
   AND day = 28
   AND transcript LIKE '%bakery%';
/*
+---------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
|  name   |                                                                                                                                                     transcript                                                                                                                                                      |
+---------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| Ruth    | Sometime within ten minutes of the theft, I saw the thief get into a car in the bakery parking lot and drive away. If you have security footage from the bakery parking lot, you might want to look for cars that left the parking lot in that time frame.                                                          |
| Eugene  | I don't know the thief's name, but it was someone I recognized. Earlier this morning, before I arrived at Emma's bakery, I was walking by the ATM on Leggett Street and saw the thief there withdrawing some money.                                                                                                 |
| Raymond | As the thief was leaving the bakery, they called someone who talked to them for less than a minute. In the call, I heard the thief say that they were planning to take the earliest flight out of Fiftyville tomorrow. The thief then asked the person on the other end of the phone to purchase the flight ticket. |
+---------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Notes: <10:25am since the theft, the thief drove away => search for cars left the parking lot between 10:15am and 10:25am.
       The thief withdrawing money from the ATM on Leggett Street => search for ATM transctions before 10:15am.
       The theif and the accomplice planned to take the earliest flight out of Fiftyville tomorrow. Flight ticket was purchased shortly after the theft => search for flight ticket orders after 10:15am.
       A phone call (< 1min) is made to the accomplice shortly after 10:15am.
*/

SELECT *
  FROM bakery_security_logs
 WHERE year = 2021
   AND month = 7
   AND day = 28
   AND hour = 10
   AND minute <= 25
   AND activity = 'exit'
   AND hour >= 10;

/*
+-----+------+-------+-----+------+--------+----------+---------------+
| id  | year | month | day | hour | minute | activity | license_plate |
+-----+------+-------+-----+------+--------+----------+---------------+
| 260 | 2021 | 7     | 28  | 10   | 16     | exit     | 5P2BI95       |
| 261 | 2021 | 7     | 28  | 10   | 18     | exit     | 94KL13X       |
| 262 | 2021 | 7     | 28  | 10   | 18     | exit     | 6P58WS2       |
| 263 | 2021 | 7     | 28  | 10   | 19     | exit     | 4328GD8       |
| 264 | 2021 | 7     | 28  | 10   | 20     | exit     | G412CB7       |
| 265 | 2021 | 7     | 28  | 10   | 21     | exit     | L93JTIZ       |
| 266 | 2021 | 7     | 28  | 10   | 23     | exit     | 322W7JE       |
| 267 | 2021 | 7     | 28  | 10   | 23     | exit     | 0NTHK55       |
+-----+------+-------+-----+------+--------+----------+---------------+

Notes: consier people who own these license plates
*/

SELECT *
  FROM people
  WHERE license_plate IN
        (SELECT license_plate
           FROM bakery_security_logs
          WHERE year = 2021
            AND month = 7
            AND day = 28
            AND hour = 10
            AND minute <= 25
            AND activity = 'exit'
            AND hour >= 10);
/*
Suspects:
+--------+---------+----------------+-----------------+---------------+
|   id   |  name   |  phone_number  | passport_number | license_plate |
+--------+---------+----------------+-----------------+---------------+
| 221103 | Vanessa | (725) 555-4692 | 2963008352      | 5P2BI95       |
| 243696 | Barry   | (301) 555-4174 | 7526138472      | 6P58WS2       |
| 396669 | Iman    | (829) 555-5269 | 7049073643      | L93JTIZ       |
| 398010 | Sofia   | (130) 555-0289 | 1695452385      | G412CB7       |
| 467400 | Luca    | (389) 555-5198 | 8496433585      | 4328GD8       |
| 514354 | Diana   | (770) 555-1861 | 3592750733      | 322W7JE       |
| 560886 | Kelsey  | (499) 555-9472 | 8294398571      | 0NTHK55       |
| 686048 | Bruce   | (367) 555-5533 | 5773159633      | 94KL13X       |
+--------+---------+----------------+-----------------+---------------+
*/

SELECT account_number, amount
  FROM atm_transactions
 WHERE year = 2021
   AND month = 7
   AND day = 28
   AND atm_location = 'Leggett Street'
   AND transaction_type = 'withdraw';

/*
Suspicious accounts:
+----------------+--------+
| account_number | amount |
+----------------+--------+
| 28500762       | 48     |
| 28296815       | 20     |
| 76054385       | 60     |
| 49610011       | 50     |
| 16153065       | 80     |
| 25506511       | 20     |
| 81061156       | 30     |
| 26013199       | 35     |
+----------------+--------+
*/

SELECT id, name, phone_number, passport_number, license_plate, account_number
  FROM people
       JOIN bank_accounts
         ON bank_accounts.person_id = people.id

 WHERE account_number IN
       (SELECT account_number
          FROM atm_transactions
         WHERE year = 2021
           AND month = 7
           AND day = 28
           AND atm_location = 'Leggett Street'
           AND transaction_type = 'withdraw');

/*
People who own the suspicios accounts
+--------+---------+----------------+-----------------+---------------+----------------+
|   id   |  name   |  phone_number  | passport_number | license_plate | account_number |
+--------+---------+----------------+-----------------+---------------+----------------+
| 686048 | Bruce   | (367) 555-5533 | 5773159633      | 94KL13X       | 49610011       |
| 514354 | Diana   | (770) 555-1861 | 3592750733      | 322W7JE       | 26013199       |
| 458378 | Brooke  | (122) 555-4581 | 4408372428      | QX4YZN3       | 16153065       |
| 395717 | Kenny   | (826) 555-1652 | 9878712108      | 30G67EN       | 28296815       |
| 396669 | Iman    | (829) 555-5269 | 7049073643      | L93JTIZ       | 25506511       |
| 467400 | Luca    | (389) 555-5198 | 8496433585      | 4328GD8       | 28500762       |
| 449774 | Taylor  | (286) 555-6063 | 1988161715      | 1106N58       | 76054385       |
| 438727 | Benista | (338) 555-6650 | 9586786673      | 8X428L0       | 81061156       |
+--------+---------+----------------+-----------------+---------------+----------------+
*/

SELECT *
  FROM flights
       JOIN airports
         ON airports.id = flights.origin_airport_id

 WHERE airports.city = 'Fiftyville'
   AND year = 2021
   AND month = 7
   AND day = 29;

/*
Flights leaving Fiftyville tomorrow
+----+-------------------+------------------------+------+-------+-----+------+--------+----+--------------+-----------------------------+------------+
| id | origin_airport_id | destination_airport_id | year | month | day | hour | minute | id | abbreviation |          full_name          |    city    |
+----+-------------------+------------------------+------+-------+-----+------+--------+----+--------------+-----------------------------+------------+
| 18 | 8                 | 6                      | 2021 | 7     | 29  | 16   | 0      | 8  | CSF          | Fiftyville Regional Airport | Fiftyville |
| 23 | 8                 | 11                     | 2021 | 7     | 29  | 12   | 15     | 8  | CSF          | Fiftyville Regional Airport | Fiftyville |
| 36 | 8                 | 4                      | 2021 | 7     | 29  | 8    | 20     | 8  | CSF          | Fiftyville Regional Airport | Fiftyville |
| 43 | 8                 | 1                      | 2021 | 7     | 29  | 9    | 30     | 8  | CSF          | Fiftyville Regional Airport | Fiftyville |
| 53 | 8                 | 9                      | 2021 | 7     | 29  | 15   | 20     | 8  | CSF          | Fiftyville Regional Airport | Fiftyville |
+----+-------------------+------------------------+------+-------+-----+------+--------+----+--------------+-----------------------------+------------+
*/

SELECT *
  FROM flights
       JOIN airports
         ON airports.id = flights.origin_airport_id

 WHERE airports.city = 'Fiftyville'
   AND year = 2021
   AND month = 7
   AND day = 29

ORDER BY hour, minute
LIMIT 1;

/*
Earliest flight leaving Fiftyville tomorrow
+----+-------------------+------------------------+------+-------+-----+------+--------+----+--------------+-----------------------------+------------+
| id | origin_airport_id | destination_airport_id | year | month | day | hour | minute | id | abbreviation |          full_name          |    city    |
+----+-------------------+------------------------+------+-------+-----+------+--------+----+--------------+-----------------------------+------------+
| 36 | 8                 | 4                      | 2021 | 7     | 29  | 8    | 20     | 8  | CSF          | Fiftyville Regional Airport | Fiftyville |
+----+-------------------+------------------------+------+-------+-----+------+--------+----+--------------+-----------------------------+------------+
*/
SELECT * FROM airports
WHERE id = 4;
/* Destination:
+----+--------------+-------------------+---------------+
| id | abbreviation |     full_name     |     city      |
+----+--------------+-------------------+---------------+
| 4  | LGA          | LaGuardia Airport | New York City |
+----+--------------+-------------------+---------------+
*/

SELECT passport_number
  FROM passengers
 WHERE flight_id =
       (SELECT flights.id
          FROM flights
          JOIN airports
            ON airports.id = flights.origin_airport_id

         WHERE airports.city = 'Fiftyville'
           AND year = 2021
           AND month = 7
           AND day = 29

         ORDER BY hour, minute
         LIMIT 1);

/*
Passport numbers of passengers on the flight taken by the theif:
+-----------------+
| passport_number |
+-----------------+
| 7214083635      |
| 1695452385      |
| 5773159633      |
| 1540955065      |
| 8294398571      |
| 1988161715      |
| 9878712108      |
| 8496433585      |
+-----------------+
*/

-- Find the suspect who appeared in all three contexts.
SELECT *
  FROM people
  -- leaving the bakery shortly after the theft
  WHERE license_plate IN
        (SELECT license_plate
           FROM bakery_security_logs
          WHERE year = 2021
            AND month = 7
            AND day = 28
            AND hour = 10
            AND minute <= 25
            AND activity = 'exit'
            AND hour >= 10)
  -- Withdrew money from ATM used by the thief
    AND name IN
        (SELECT name
           FROM people
                JOIN bank_accounts
                  ON bank_accounts.person_id = people.id

          WHERE account_number IN
                (SELECT account_number
                    FROM atm_transactions
                  WHERE year = 2021
                    AND month = 7
                    AND day = 28
                    AND atm_location = 'Leggett Street'
                    AND transaction_type = 'withdraw'))
  -- Leaving Fiftyville tomorrow by taking the earliest flight
    AND passport_number IN
        (SELECT passport_number
           FROM passengers
          WHERE flight_id =
                (SELECT flights.id
                   FROM flights
                   JOIN airports
                     ON airports.id = flights.origin_airport_id

                  WHERE airports.city = 'Fiftyville'
                    AND year = 2021
                    AND month = 7
                    AND day = 29

                 ORDER BY hour, minute
                 LIMIT 1))
;

/* Suspects:
+--------+-------+----------------+-----------------+---------------+
|   id   | name  |  phone_number  | passport_number | license_plate |
+--------+-------+----------------+-----------------+---------------+
| 467400 | Luca  | (389) 555-5198 | 8496433585      | 4328GD8       |
| 686048 | Bruce | (367) 555-5533 | 5773159633      | 94KL13X       |
+--------+-------+----------------+-----------------+---------------+
*/

SELECT * FROM phone_calls
 WHERE year = 2021
   AND month = 7
   AND day = 28
   AND duration <= 60;
/* Suspicous calls made on the day of theft:
+-----+----------------+----------------+------+-------+-----+----------+
| id  |     caller     |    receiver    | year | month | day | duration |
+-----+----------------+----------------+------+-------+-----+----------+
| 221 | (130) 555-0289 | (996) 555-8899 | 2021 | 7     | 28  | 51       |
| 224 | (499) 555-9472 | (892) 555-8872 | 2021 | 7     | 28  | 36       |
| 233 | (367) 555-5533 | (375) 555-8161 | 2021 | 7     | 28  | 45       |
| 234 | (609) 555-5876 | (389) 555-5198 | 2021 | 7     | 28  | 60       |
| 251 | (499) 555-9472 | (717) 555-1342 | 2021 | 7     | 28  | 50       |
| 254 | (286) 555-6063 | (676) 555-6554 | 2021 | 7     | 28  | 43       |
| 255 | (770) 555-1861 | (725) 555-3243 | 2021 | 7     | 28  | 49       |
| 261 | (031) 555-6622 | (910) 555-3251 | 2021 | 7     | 28  | 38       |
| 279 | (826) 555-1652 | (066) 555-9701 | 2021 | 7     | 28  | 55       |
| 281 | (338) 555-6650 | (704) 555-2131 | 2021 | 7     | 28  | 54       |
+-----+----------------+----------------+------+-------+-----+----------+

Bruce's phone number is in the list, the receiver is (375) 555-8161
*/

SELECT * FROM people
 WHERE phone_number = '(375) 555-8161';
/* Accomplice information:
+--------+-------+----------------+-----------------+---------------+
|   id   | name  |  phone_number  | passport_number | license_plate |
+--------+-------+----------------+-----------------+---------------+
| 864400 | Robin | (375) 555-8161 |                 | 4V16VO0       |
+--------+-------+----------------+-----------------+---------------+
*/

