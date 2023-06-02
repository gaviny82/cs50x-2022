#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
    int strength;
}
pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);
bool search(int winner, int loser);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    // Check for invalid name
    int candidate_index = -1;
    for (int i = 0; i < candidate_count; i++)
    {
        if (strcmp(candidates[i], name) == 0)
        {
            candidate_index = i;
            break;
        }
    }
    if (candidate_index == -1)
    {
        return false; // Name not exist
    }

    // Check for invalid rank
    for (int i = 0; i < rank; i++)
    {
        if (ranks[i] == candidate_index)
        {
            return false; // Duplicate candidate ranks
        }
    }

    // Update ranks
    ranks[rank] = candidate_index;

    return true;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    for (int i = 0; i < candidate_count - 1; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            int more_preferred = ranks[i];
            int less_preferred = ranks[j];

            preferences[more_preferred][less_preferred]++;
        }
    }
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            // Consider the preference between candiate i and j
            // strength > 0 if more voters prefer i over j
            int diff = preferences[i][j] - preferences[j][i];
            if (diff != 0)
            {
                pair p;
                p.winner = diff > 0 ? i : j;
                p.loser = diff > 0 ? j : i;

                pairs[pair_count] = p;
                pair_count++;
            }
        }
    }
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    // Selection sort
    for (int i = 0; i < pair_count - 1; i++)
    {
        int max_index = i;
        int max_strength = preferences[pairs[i].winner][pairs[i].loser];
        for (int j = i + 1; j < pair_count; j++)
        {
            int strength = preferences[pairs[j].winner][pairs[j].loser];
            if (strength > max_strength)
            {
                max_index = j;
                max_strength = strength;
            }
        }
        // Swap
        if (max_index != i)
        {
            pair tmp = pairs[i];
            pairs[i] = pairs[max_index];
            pairs[max_index] = tmp;
        }
    }
}

// Starting from the loser node, try searching for the winner node
// Returns true if there is a cycle
bool search(int winner, int loser)
{
    for (int i = 0; i < candidate_count; i++)
    {
        bool *nodes = locked[loser];

        // Loser i can be locked in over multiple other candidates
        for (int j = 0; j < candidate_count; j++)
        {
            if (nodes[j]) // The loser is locked in over candidate j
            {
                if (j == winner) // Cycle found
                {
                    return true;
                }
                else // Cycle not found, then recursively search for candidates over which are locked in
                {
                    if (search(winner, j))
                    {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    for (int i = 0; i < pair_count; i++)
    {
        pair *p = &pairs[i]; // *p is being added to the graph
        bool cycle = search(p -> winner, p -> loser);
        if (!cycle)
        {
            locked[p -> winner][p -> loser] = true;
        }
    }
}

// Print the winner of the election
void print_winner(void)
{
    // Find an edge
    int source = -1;
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            if (locked[i][j])
            {
                source = i;
                break;
            }
        }
        if (source != -1)
        {
            break;
        }
    }

    bool flag = true;
    while (flag)
    {
        flag = false;
        for (int i = 0; i < candidate_count; i++)
        {
            if (locked[i][source])
            {
                // When candidate source is locked over by candidate i
                source = i;
                flag = true;
            }
        }
    }
    printf("%s\n", candidates[source]);
}