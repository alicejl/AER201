/**
 * @file
 * @author Alice Liang
 *
 * Created on January 21, 2019
 * 
 * @brief Main Function of Autonomous Robot
 * 
 * Preconditions
 * @pre Co-processor running default program
 * @pre Correct connections and wiring of peripherals and sensors
 */

void main(void){
	while (1){ //while loop that keeps the robot running
		mainMenu(); //alter main funtion of menu.c
		initRun(); //initiate motors, timer, distance sensor
		set return to 0; //return is a variable that states 
		run(); //regular run of robot
	}
}

/**
 * @brief Set up and initiate peripherals, timer, and motors
 */

void initRun(void){
	enable interupts for peripherals;
	set up peripherals;
	initiate stack;
	enable interupt for timer;
	set timer to 3:50 minutes;
	start timer;
	start motors;
}

/**
 * @brief regular run of robot
 */

void run(void){ //should there and back be the same function?
	int atCanister = 0; //is there a canister detected: 0 no, 1 yes
	int deployBall = 0; //should a ball be deployed: 0 no, 1 yes
	int atEnd = 0; // reused variable, 1 at turn around, and 1 for termination
	float dist=0; //var for distance

	while(1){
		atEnd = isEnd();
		if (atEnd ==1){
			atEnd=0;
			turnAround(); //initate turn
			break;
		atCanister = isCanister();
		if (atCanister == 1){
			runMotor(0); //pause motor
			deployBall = canisterStatus(&dist); //check conditions: is a ball inside is it past 20cm
			if (deployBall == 1){
				dropBall();
			}
			push(flag); //info for return
			enque(dist, deployBall, 'L')//add to queue for run stats
	}
}