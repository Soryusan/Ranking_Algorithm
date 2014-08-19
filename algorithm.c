#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define MAX_CARS 4
#define INIT 1
#define MATCH_DATA 2
#define RUN_MATCH 3
#define SHOW_STATUS 4
#define RUN_RANDOM 5
#define ADD_CAR 6
#define REMOVE_CAR 7
#define QUIT 8
/* Struct for car data, contains whether car has been intialized,
 * it's hidden rank, score gain, placement in match, and expected
 * placement
 */
typedef struct {
  int init;
  double HR;
  double score;
  int place;
  int expected;
  int checked;
} Car;

/*Initializes number of cars present for match*/
void initialize_num_cars(int *num_cars) {
  while(*num_cars < 2 || *num_cars > 4) {
    printf("Number of cars: ");
    scanf("%d", num_cars);
    if(*num_cars < 2 || *num_cars > 4) {
      printf("Only a minimum of 2 cars and a maximum of 4 cars can be played!\n");
    }
  }
}

/*Adds a car to the match*/
void add_car(Car *match, int car) {
  printf("Enter initial rank for car %d: ", car);
  scanf("%lf", &match[car].HR);
  match[car].init = 1;
}
/*Check for duplicate rankings*/
int check_same_HR(Car *match, int num_cars) {
  int i, j, same = num_cars, checked = 0;
  for(i = 0; i < num_cars; i++) {
    match[i].checked = 0;
  }
  for(i = 0; i < num_cars; i++) {
    for(j = 0; j < num_cars; j++) {
      if(i != j && !match[i].checked && !match[j].checked && match[i].HR == match[j].HR) {
        same--;
        match[i].checked = 1;
      }
    }
  }
  return same;
}

/*Calculates expected placement based on HR*/
void expected_place(Car *match, int num_cars) {
  int i, j, place, max_place;
  max_place = check_same_HR(match, num_cars);
  for(i = 0; i < num_cars; i++) {
    place = 1;
    for(j = 0; j < num_cars; j++) {
      if(match[i].HR < match[j].HR) {
        if(!max_place || place < max_place) {
          place++;
        }
      }
    }
    match[i].expected = place;
  }
}
/*collection of match data, placement*/
void match_data(Car *match, int num_cars, FILE *result) {
  int i;
  int* placement = calloc(sizeof(int), num_cars);
  for(i = 0; i < num_cars; i++) {
    printf("Enter placement for car %d: ", i);
    scanf("%d", &match[i].place);
    if(match[i].place <= num_cars && match[i].place >= 1) {
      if(placement[match[i].place - 1]) {
        i--;
        printf("This placement has already been used\n");
      }
      else {
        placement[match[i].place - 1] = 1;
        fprintf(result,"Placement for car %d: %d\n", i, match[i].place);
      }
    }
    else {
      printf("Invalid placement\n");
      i--;
    }
  }
  //expected_place(match, num_cars);
  free(placement);
}

/*Car comparisons*/
void compare(Car *one, Car *two, int total, FILE *result) {
  int place_diff = one->place - two->place;
  double rank_diff = abs(one->HR - two->HR), pool = two->HR, percentage;
  //check ranks of cars
  //check which car placed ahead of the other
  //check to see if car expected to place ahead of other
  if(!rank_diff) {
    rank_diff = 1;
  }
  percentage = one->HR / total;
  percentage = 1 - percentage;
  pool *= percentage;
  //fprintf(result, "Percentage: %.2f Pool: %.2f\n", percentage, pool);
  if(place_diff < 0) {
    one->score += .70 * pool;
  }
  else {
    one->score += .30 * pool;
  }

  printf("%.2f new score\n", one->score);
}

/*DEE ALGORITHM*/
void run_algorithm(Car *match, int num_cars, FILE *result) {
  int i, j, total = 0;
  double average, deviation;
  //Calculate average
  /*for(i = 0; i < num_cars; i++) {
    average += match[i].HR;
  }
  average /= num_cars;*/
  for(i = 0; i < num_cars; i++) {
    total += match[i].HR;
  }
  //fprintf(result, "Average: %.2f\n", average);
  for(i = 0; i < num_cars; i++) {
    /*deviation = match[i].HR - average;
    if(!deviation) {
      deviation = 1;
    }*/
    for(j = 0; j < num_cars; j++) {
      if(i != j) {
        compare(&match[i], &match[j], total, result);
      }
    }
    //match[i].score = sqrt(match[i].score);
    printf("\n");
  }

  //fprintf(result, "Average: %.2f\n", average);
  for(i = 0; i < num_cars; i++) {
    /*if(match[i].expected > match[i].place) {
      match[i].score = match[i].score * (1 + ((match[i].expected - match[i].place) * .5));
    }*/
    match[i].score /= sqrt(total);

    fprintf(result, "Adding score %.2f to car %d\n", match[i].score, i);
    match[i].HR = match[i].score + match[i].HR;
    match[i].score = 0;
  }
  fprintf(result, "\nNew HR\n");
}

/*display current status of all cars*/
void show_status(Car *match, int num_cars, FILE *result) {
  int i;
  for(i = 0; i < num_cars; i++) {
    fprintf(result, "Car: %d HR: %.2f Placement: %d Expected: %d\n", 
      i, match[i].HR, match[i].place, match[i].expected);
  }
  fprintf(result, "\n");
}

/*simulate match, run results through algorithm*/
void run_match(Car *match, int num_cars, FILE *result) {
  run_algorithm(match, num_cars, result);
  show_status(match, num_cars, result);
}

void random_match(Car *match, int num_cars, FILE *result, int *match_count) {
  int i, random, *place;
  place = calloc(sizeof(int), num_cars);
  for(i = 0; i < num_cars; i++) {
    random = rand() % num_cars + 1;
    if(place[random - 1]) {
      i--;
    }
    else {
      place[random - 1] = 1;
      match[i].place = random;
    }
  }
  free(place);
  expected_place(match, num_cars);
  show_status(match, num_cars, result);
  fprintf(result, "Running match %d...\n\n", ++(*match_count));
  run_match(match, num_cars, result);
}

void print_options() {
  printf("\n");
  printf("Option 1: Initialize cars\n");
  printf("Option 2: Enter match data\n");
  printf("Option 3: Run match\n");
  printf("Option 4: Show status\n");
  printf("Option 5: Random match\n");
  printf("Option 6: Add car\n");
  printf("Option 7: Remove car\n");
  printf("Option 8: Quit\n");
}

int main(int argc, char* argv[]) {
  FILE *result = fopen("result", "w+");
  Car match[MAX_CARS] = {{0}, {0}, {0}, {0}};
  int num_cars = 0, choice, match_count = 0, rounds, i, j;
  do {
    print_options();
    printf("Enter option: ");
    scanf("%d", &choice);
    printf("\n");
    switch(choice) {
      case INIT:
        initialize_num_cars(&num_cars);
        for(i = 0; i < num_cars; i++) {
          add_car(match, i);
        }
        expected_place(match, num_cars);
        break;
      case MATCH_DATA:
        if(num_cars || match[num_cars - 1].init) {
          match_data(match, num_cars, result);
        }
        else {
          printf("Initialize cars\n");
        }
        break;
      case RUN_MATCH:
        //match_count++;
        fprintf(result, "Running match %d...\n\n", ++match_count);
        run_match(match, num_cars, result);
        expected_place(match, num_cars);
        break;
      case SHOW_STATUS:
        show_status(match, num_cars, result);
        break;
      case RUN_RANDOM:
        printf("Enter number of rounds: ");
        scanf("%d", &rounds);
        for(i = 0; i < rounds; i++) {
          //fprintf(result, "Running match %d...\n\n", ++match_count);
          random_match(match, num_cars, result, &match_count);
          expected_place(match, num_cars);
        }
        break;
      case ADD_CAR:
        if(num_cars && num_cars < MAX_CARS) {
          num_cars++;
          add_car(match, num_cars - 1);
          printf("Car %d added\n", num_cars);
          expected_place(match, num_cars);
        }
        else if(!num_cars) {
          printf("Initialize a match\n");
        }
        else {
          printf("No more cars can be added\n");
        }
        break;
      case REMOVE_CAR:
        if(num_cars && num_cars > 2) {
          match[num_cars-1].init = 0;
          num_cars--;
          printf("Car %d removed\n", num_cars+1);
          expected_place(match, num_cars);
        }
        else {
          printf("No cars to remove\n");
        }
        break;
      case QUIT:
        printf("Goodbye\n");
        break;
      default:
        printf("Enter value 1-%d\n", QUIT);
    }
  } while(choice != QUIT);
  fclose(result);
}
