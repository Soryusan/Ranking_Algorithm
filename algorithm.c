#include <stdio.h>
#include <stdlib.h>
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
/*Calculates expected placement based on HR*/
void expected_place(Car *match, int num_cars) {
  int i, j, place;
  for(i = 0; i < num_cars; i++) {
    place = 4;
    for(j = 0; j < num_cars; j++) {
      if(match[i].HR > match[j].HR) {
        place--;
      }
    }
    match[i].expected = place;
  }
}

void match_data(Car *match, int num_cars) {
  int i;
  int* placement = calloc(sizeof(int), num_cars);
  for(i = 0; i < num_cars; i++) {
    printf("Enter placement for car %d: ", i);
    scanf("%d", &match[i].place);
    if(match[i].place <= 4 && match[i].place >= 1) {
      if(placement[match[i].place - 1]) {
        i--;
        printf("This placement has already been used\n");
      }
      else {
        match[i].place_diff = match[i].expected - match[i].place;
        placement[match[i].place - 1] = 1;
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

void compare(Car one, Car two) {
  int rank_diff = one.HR - two.HR, place_diff = one.place - two.place, 
        pool = (one.HR + two.HR) / 2;
  //check ranks of cars
  //check which car placed ahead of the other
  //check to see if car expected to place ahead of other
  //Higher ranked car
  if(rank_diff > 0) {
    //Came in better placement, expected
    if(place_diff < 0) {
      
    }
    //Came behind lower car, upset
    else {

    }
  }
  //Lower/equal ranked car
  else {
    
  }
}

void run_match(Car *match, int num_cars) {
  int i,j;
  for(i = 0; i < num_cars; i++) {
    for(j = 0; j < num_cars; j++) {
      compare(match[i], match[j]);
    }
  }
}

void show_status(Car *match, int num_cars) {
  int i;
  for(i = 0; i < num_cars; i++) {
    printf("Car %d: HR: %d Placement: %d Expected: %d\n", 
      i, match[i].HR, match[i].place, match[i].expected);
  }
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
  Car match[MAX_CARS] = {{0}, {0}, {0}, {0}};
  int num_cars = 0, choice, i, j;
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
          match_data(match, num_cars);
        }
        else {
          printf("Initialize data\n");
        }
        break;
      case RUN_MATCH:
        run_match(match, num_cars);
        break;
      case SHOW_STATUS:
        show_status(match, num_cars);
        break;
      case ADD_CAR:
        if(num_cars && num_cars < MAX_CARS) {
          num_cars++;
          add_car(match, num_cars - 1);
          printf("Car %d added\n", num_cars);
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
          match[num_cars].init = 0;
          num_cars--;
          printf("Car %d removed\n", num_cars+1);
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
}
