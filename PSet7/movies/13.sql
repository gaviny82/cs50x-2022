SELECT DISTINCT name FROM people
JOIN stars ON people.id = stars.person_id
WHERE movie_id IN (
    -- list of movie_id in which Kevin Bacon (1958) starred
    SELECT movie_id FROM stars
    JOIN people ON people.id = stars.person_id
    WHERE name = 'Kevin Bacon' AND birth = 1958
) AND
name != 'Kevin Bacon';