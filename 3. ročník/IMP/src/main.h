/*
 * main.h
 *
 *  Created on: 2. 12. 2024
 *      Author: Jiri Kotek
 */

#ifndef MAIN_H_
#define MAIN_H_

enum State {
  STOP,
  MAIN_ROAD_GREEN,
  MAIN_ROAD_ORANGE,
  MAIN_ROAD_PREPARE,
  SIDE_ROAD_GREEN,
  SIDE_ROAD_ORANGE,
  SIDE_ROAD_PREPARE,
  ZEBRA_CROSSING_STOP,
  ZEBRA_CROSSING
};

enum Road {
  MAIN,
  SIDE
};

enum Zebra {
  CARS_MAIN,
  CARS_SIDE,
  WALKERS
};


#endif /* MAIN_H_ */
