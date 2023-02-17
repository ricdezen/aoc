#define EXPECTED_1 2696
#define EXPECTED_2 668

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fileutils.h"
#include "min_max.h"
#include "misc.h"
#include "stringset.h"

#define MAX_LINE_LENGTH 1024
#define MAX_REINDEERS 64
#define TRAVEL_TIME 2503

typedef struct {
    int speed;
    int flight_time;
    int resting_time;
} Reindeer;

int Reindeer_travel(Reindeer *reindeer, int time_seconds) {
    int cycle_time = reindeer->flight_time + reindeer->resting_time;

    // The reindeer flies and fully rests this many times.
    int full_cycles = time_seconds / cycle_time;
    int flight_time = full_cycles * reindeer->flight_time;

    // Time left, how far will the reindeer travel? At most its flight_time.
    int time_left = time_seconds - (full_cycles * cycle_time);
    int flight_time_left = mini(time_left, reindeer->flight_time);

    return (flight_time + flight_time_left) * reindeer->speed;
}

int Reindeer_is_flying(Reindeer *reindeer, int time_seconds) {
    int cycle_time = reindeer->flight_time + reindeer->resting_time;

    // The reindeer flies and fully rests this many times.
    int full_cycles = time_seconds / cycle_time;

    // Time left.
    int time_left = time_seconds - (full_cycles * cycle_time);

    // More time left than the reindeer can fly, it will be resting at the end.
    // If time_left == 0, then the reindeer was still resting.
    if (time_left == 0 || time_left > reindeer->flight_time)
        return 0;

    return 1;
}

int main() {
    // Open input and check for errors.
    FILE *input = fopen("2015_day_14.txt", "r");

    if (input == NULL) {
        perror("Input file not found.\n");
        return 1;
    }

    // Reindeers and their stats.
    Reindeer reindeers[MAX_REINDEERS];
    int n_reindeers = 0;

    // Temporary storage for the line.
    char line[MAX_LINE_LENGTH] = {0};
    int len = 0;

    while (len = freadline(input, line, sizeof(line))) {
        Reindeer r = {0};

        // Get reindeer speed and times.
        strtok(line, " ");
        strtok(NULL, " ");
        strtok(NULL, " ");

        r.speed = atoi(strtok(NULL, " "));

        strtok(NULL, " ");
        strtok(NULL, " ");

        r.flight_time = atoi(strtok(NULL, " "));

        for (int i = 0; i < 6; i++)
            strtok(NULL, " ");

        r.resting_time = atoi(strtok(NULL, " "));

        reindeers[n_reindeers++] = r;
    }

    // Find winner for part one. All reindeers just run until they have to
    // stop and rest.
    int winner_one = 0;
    for (int i = 0; i < n_reindeers; i++) {
        int dist = Reindeer_travel(&reindeers[i], TRAVEL_TIME);
        if (dist > winner_one)
            winner_one = dist;
    }

    // For part two we clearly cannot do the same thing anymore. We have to
    // step through every single second to award the points.
    int *points = (int *)calloc(n_reindeers, sizeof(int));
    int *positions = (int *)calloc(n_reindeers, sizeof(int));

    // At each second.
    for (int i = 1; i <= TRAVEL_TIME; i++) {
        // Move each reindeer.
        for (int r = 0; r < n_reindeers; r++)
            if (Reindeer_is_flying(&reindeers[r], i))
                positions[r] += reindeers[r].speed;

        // Give a point to the furthest reindeer (and every tied one).
        int top_position = array_maxi(positions, n_reindeers);
        for (int r = 0; r < n_reindeers; r++)
            if (positions[r] == top_position)
                points[r]++;
    }

    int winner_two = array_maxi(points, n_reindeers);

    free(points);
    free(positions);

    // Close file.
    fclose(input);

    return check_result_i(winner_one, EXPECTED_1) | check_result_i(winner_two, EXPECTED_2);
}