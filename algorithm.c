#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define MAX_CARS 4
#define INIT 1
#define MATCH_DATA 2
#define RUN_MATCH 3
#define SHOW_STATUS 4
#define ADD_CAR 5
#define REMOVE_CAR 6
#define QUIT 7
/* Struct for car data, contains whether car has been intialized,
 * it's hidden rank, score gain, placement in match, and expected
 * placement
 */
typedef struct {
  int init;
  int HR;
  int score;
  int place;
  int expected;
  int place_diff;
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
  scanf("%d", &match[car].HR);
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
        match[i].place_diff = match[i].expected - match[i].place;
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

/*DEE ALGORITHM*/
void compare(Car *one, Car *two) {
  int rank_diff = one->HR - two->HR, place_diff = one->place - two->place, pool = two->HR / one->place;
  //check ranks of cars
  //check which car placed ahead of the other
  //check to see if car expected to place ahead of other
  if(rank_diff < 0) {
    rank_diff *= -1;
  }
  //Higher ranked car
  if(one->HR > two->HR) {
    //Came in better placement, expected
    if(place_diff < 0) {
      one->score += (int)(.65 * pool);
    }
    //Came behind lower car, upset
    else {
      one->score += (int)(.25 * pool);
    }
  }
  //Lower ranked car
  else if(one->HR < two->HR){
    //Came behind better car, expected
    if(place_diff > 0) {
      one->score += (int)(.35 * pool);
    }
    //Came ahead of better car, upset
    else {
      one->score += (int)(.75 * pool);
    }
  }
  //Equal HR
  else {
    //This car won
    if(place_diff < 0) {
      one->score += (int)(.70 * pool);
    }
    //This car lost
    else {
      one->score += (int)(.30 * pool);
    }
  }
  printf("%d one new score\n", one->score);
}

void show_status(Car *match, int num_cars, FILE *result) {
  int i;
  for(i = 0; i < num_cars; i++) {
    fprintf(result, "Car: %d HR: %d Placement: %d Expected: %d\n", 
      i, match[i].HR, match[i].place, match[i].expected);
  }
  fprintf(result, "\n");
}

void run_match(Car *match, int num_cars, FILE *result) {
  int i,j;
  for(i = 0; i < num_cars; i++) {
    for(j = 0; j < num_cars; j++) {
      if(i != j){
        compare(&match[i], &match[j]);
      }
    }
    if(match[i].expected > match[i].place) {
      match[i].score = match[i].score * (1 + ((match[i].expected - match[i].place) * .3));
    }
    match[i].score = sqrt(match[i].score);
    printf("\n");
  }
  for(i = 0; i < num_cars; i++) {
    fprintf(result, "Adding score %d to car %d\n", match[i].score, i);
    match[i].HR = match[i].score + match[i].HR;
    match[i].score = 0;
  }
  fprintf(result, "\nNew HR\n");
  show_status(match, num_cars, result);
}

void print_options() {
  printf("\n");
  printf("Option 1: Initialize match\n");
  printf("Option 2: Enter match data\n");
  printf("Option 3: Run match\n");
  printf("Option 4: Show status\n");
  printf("Option 5: Add car\n");
  printf("Option 6: Remove car\n");
  printf("Option 7: Quit\n");
}

int main(int argc, char* argv[]) {
  FILE *result = fopen("result", "w+");
  Car match[MAX_CARS] = {{0}, {0}, {0}, {0}};
  int num_cars = 0, choice, match_count = 0, i, j;
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
          printf("Initialize data\n");
        }
        break;
      case RUN_MATCH:
        match_count++;
        fprintf(result, "Running match %d...\n\n", match_count);
        run_match(match, num_cars, result);
        expected_place(match, num_cars);
        break;
      case SHOW_STATUS:
        show_status(match, num_cars, result);
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
