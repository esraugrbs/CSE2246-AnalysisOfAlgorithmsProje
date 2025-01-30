#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

#define MAX_CITIES 50000

typedef struct {
    int id;
    double x;
    double y;
} City;

City *cities;
City *region1;
City *region2;
int region1_size = 0;
int region2_size = 0;

double euclidean_distance(City c1, City c2) {
    return round(sqrt((c1.x - c2.x) * (c1.x - c2.x) + (c1.y - c2.y) * (c1.y - c2.y)));
}

void parse_input(const char* file_path, int* total_cities) {
    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        printf("Could not open file %s\n", file_path);
        exit(1);
    }
    int i = 0;
    while (fscanf(file, "%d %lf %lf", &cities[i].id, &cities[i].x, &cities[i].y) != EOF && i < MAX_CITIES) {
        i++;
    }
    *total_cities = i;
    fclose(file);
}

void k_means_clustering(int k, int total_cities) {
    
    int midpoint = cities[total_cities / 2].x;
    int i;
    for ( i = 0; i < total_cities; i++) {
        if (cities[i].x <= midpoint) {
            region1[region1_size++] = cities[i];
        } else {
            region2[region2_size++] = cities[i];
        }
    }
}

double tour_length(City* tour, int size) {
    double length = 0;
    int i;
    for ( i = 0; i < size - 1; i++) {
        length += euclidean_distance(tour[i], tour[i + 1]);
    }
    length += euclidean_distance(tour[size - 1], tour[0]);
    return length;
}

void nearest_neighbor_tour(City* cities, int size, City* tour) {
    int *visited = (int *)calloc(size, sizeof(int));
    tour[0] = cities[0];
    visited[0] = 1;
    int i,j;
    for ( i = 1; i < size; i++) {
        City last = tour[i - 1];
        double min_distance = INT_MAX;
        int nearest = -1;
        for (j = 0; j < size; j++) {
            if (!visited[j]) {
                double distance = euclidean_distance(last, cities[j]);
                if (distance < min_distance) {
                    min_distance = distance;
                    nearest = j;
                }
            }
        }
        tour[i] = cities[nearest];
        visited[nearest] = 1;
    }
    free(visited);
}

void two_opt(City* tour, int size) {
    int improved = 1;
    int i,k,j;
    while (improved) {
        improved = 0;
        for ( i = 1; i < size - 2; i++) {
            for (j = i + 1; j < size - 1; j++) {
                if (j - i == 1) continue;
                double current_distance = euclidean_distance(tour[i - 1], tour[i]) + euclidean_distance(tour[j], tour[j + 1]);
                double new_distance = euclidean_distance(tour[i - 1], tour[j]) + euclidean_distance(tour[i], tour[j + 1]);
                if (new_distance < current_distance) {
                    
                    for ( k = 0; k < (j - i + 1) / 2; k++) {
                        City temp = tour[i + k];
                        tour[i + k] = tour[j - k];
                        tour[j - k] = temp;
                    }
                    improved = 1;
                }
            }
        }
    }
}

void save_output(const char* file_path, City* tour1, int size1, City* tour2, int size2) {
    FILE *file = fopen(file_path, "w");
    if (file == NULL) {
        printf("Could not open file %s\n", file_path);
        exit(1);
    }
    int i;
    double total_length = tour_length(tour1, size1) + tour_length(tour2, size2);
    fprintf(file, "%.0f\n", total_length);
    fprintf(file, "%.0f %d\n", tour_length(tour1, size1), size1);
    for ( i = 0; i < size1; i++) {
        fprintf(file, "%d\n", tour1[i].id);
    }
    fprintf(file, "\n");
    fprintf(file, "%.0f %d\n", tour_length(tour2, size2), size2);
    for (i = 0; i < size2; i++) {
        fprintf(file, "%d\n", tour2[i].id);
    }
    fprintf(file, "\n");
    fclose(file);
}

void solve_2tsp(const char* input_file, const char* output_file) {
    int total_cities = 0;
    cities = (City *)malloc(MAX_CITIES * sizeof(City));
    region1 = (City *)malloc(MAX_CITIES * sizeof(City));
    region2 = (City *)malloc(MAX_CITIES * sizeof(City));

    parse_input(input_file, &total_cities);
    k_means_clustering(2, total_cities);
    City *tour1 = (City *)malloc(region1_size * sizeof(City));
    City *tour2 = (City *)malloc(region2_size * sizeof(City));
    nearest_neighbor_tour(region1, region1_size, tour1);
    nearest_neighbor_tour(region2, region2_size, tour2);
    two_opt(tour1, region1_size);
    two_opt(tour2, region2_size);
    save_output(output_file, tour1, region1_size, tour2, region2_size);

    free(cities);
    free(region1);
    free(region2);
    free(tour1);
    free(tour2);
}

int main() {
    solve_2tsp("test-input-4.txt", "test-output-4.txt");
    return 0;
}

