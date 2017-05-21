// Joshua Jeon
// Andrew Whitaker
// 861-14-1892
// TA: Amir
//

#include <iostream>
#include "cpp.h"
#include <string.h>

//Global Variables
long FLOORS;
long ELEVS;
const long day_length = 120;
const long TINY = 0.001;
const long GRND = 0;
long elevID = 0;
long* elev_loc;
bool* want_up;
bool* want_down;
bool* want_off;

//Global CSIM Constructs
facility update_workload("update_Work");

mailbox_set *mb_up;
mailbox_set *mb_down;
event_set *going_up;
event_set *going_down;
event_set *here_is_floor;

//Global functions
void floor(long);
void passenger(long,long);
void elevator(long);
void loading();
void unloading();



extern "C" void sim() {

    create("sim");
    //add histogram
    
    //ask for user input about program's particulars
    
    //dynamically creat csim constructs
    elev_buttons = new mailbox_set("elevButtons",FLOORS);
    going_up = new event_set("goingUP",ELEVS);
    going_down = new event_set("goingDWN",ELEVS);
    here_is_floor = new event_set("hereIsFlr",ELEVS);

    
    //create the floor processes that spawn the passengers
    for (long i = 0; i < FLOORS; i++) {
        floor(i);
    }

    //create the elevators to pick up passengers
    for (long i = 0; i < ELEVS; i++) {
        elevator(i);
    }

    //hold for the whole day
    hold(day_length);

    report();

}


void floor(long whichami) {
    const char* myName = ("Flr:" + to_string(whichami)).c_str();
    create(myName);
    
    while (clock < day_length) {
        //wait certain amount of time before spawns passenger
        hold(uniform(4,10));
        //stochastically determine where the passenger goes
        long wheretogo = uniform(0,FLOORS);
        //make sure they go to a different floor than where they start
        while (wheretogo == whichami) {
            wheretogo = uniform(0,FLOORS);
        }
        //call to make this passenger
        passenger(whichami,wheretogo);
    }

    return;
}

void passenger(long whereami, long wheretogo) {
    const char* myName = (to_string(whereami) + " => " + to_string(wheretogo)).c_str();
    create(myName);

    //reserve update_workload facility
    update_workload.reserve();
    //set wakeup event and correct call button
    if (wheretogo > wherami) {
        want_up[whereami] = true;
    }
    else if (wheretogo < whereami) {
        want_down[whereami] = true;
    }
    else {
        cout << "ERROR: passsenger's wheretogo == whereami" << endl;
    }
    //release the update_workload facility
    update_workload.release();
    //wait for elevator to arrive and pass its ID
    long myElev;
    if (wheretogo > whereami) {
        (*mb_up)[whereami].receive(&myElev);
    }
    else if (wheretogo < whereami) {
        (*mb_down)[whereami].receive(&myElev);
    }
    else {
        cout << "ERROR: passsenger's wheretogo == whereami" << endl;   
    }
    //wait for proper boarding event before getting on elevator
    if (wheretogo > whereami) {
        (*going_up)[myElev].queue();
    }
    else if (wheretogo < whereami) {
        (*going_down)[myElev].queue();
    }
    else {
        cout << "ERROR: passsenger's wheretogo == whereami" << endl;   
    }
    //reserve update_workload faciity
    update_workload.reserve();
    //set its departure floor
    want_off[myElev][wheretogo] = true;
    //release facility
    update_workload.release();
    //wait for correct departure event
    while (elev_loc[myElev] != wheretogo) {
        (*here_is_floor)[myElev].wait();
    }
    (*get_off)[myElev].queue();
    
    return;
}

void elevator(long myID) {
    
    //wait for event to wake up
    
    //decide which elevator is closest
    
    //if it's me, I'll go pick them up
        //hold until I get there
        //while people still waiting
        //
        //
    //otherwise, I'll shift somehwhere else to minimize wait time

    //

    return;
}







