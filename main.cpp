// Joshua Jeon
// Andrew Whitaker
// 861-14-1892
// TA: Amir
//

#include <iostream>
#include "cpp.h"
#include <string.h>
#include <vector>

//Global Variables
const long FLOORS = 21;
long ELEVS;
const long day_length = 120;
const long TINY = 0.001;
const long GRND = 0;
const long UP = 1;
const long DOWN = 0;
const long ASLEEP = 2;
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
event_set *boarded;
event_set *unloaded;

//Global functions
void floor(long);
void passenger(long,long);
void control_unit(long);
void elevator(long);
void loading(long,long,long&,long);
void unloading(long,long,long&);

class Elevator {
    private:
        long Location;
        long Destination;
        long Direction;
    public:
        Elevator();
        long get_Loc() {return Location;};
        long get_Dest() {return Destination;};
        long get_Dir() {return Direction;};
        void set_Loc(long& loc) {Location = loc;};
        void set_Dest(long& dest) {Destination = dest;};
        void set_Dir(long& dir) {Direction = dir;};
};
//vector of Elevator pointers
//global for any elevator or passenger to access
vector<Elevator*> ElevCU;

Elevator::Elevator() {
    Location = GRND;
    Destination = GRND;
    Direction = ASLEEP;
}

extern "C" void sim() {

    create("sim");
    //add histogram
    
    //ask for user input about program's particulars
    cout << "Num of elevators: ";
    cin >> ELEVS;

    //dynamically creat csim constructs
    mb_up = new mailbox_set("mb_up",FLOORS);
    mb_down = new mailbox_set("mb_down",FLOORS);
    going_up = new event_set("goingUP",ELEVS);
    going_down = new event_set("goingDWN",ELEVS);
    here_is_floor = new event_set("hereIsFlr",ELEVS);
    boarded = new event_set("boarded",ELEVS);
    unloaded = new event_set("unloaded",ELEVS);

    
    //create the floor processes that spawn the passengers
    for (long i = 0; i < FLOORS; i++) {
        floor(i);
    }

    //create the control unit for the elevators
    //     control unit will create elevators
    
    for (long i = 0; i < ELEVS; i++) {
        Elevator* E = new Elevator();
        ElevCU.push_back(E);
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
        long wheretogo = uniform(0,FLOORS - 1);
        //make sure they go to a different floor than where they start
        while (wheretogo == whichami) {
            wheretogo = uniform(0,FLOORS - 1);
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
    //find out if we need to stop an elevator
    //    also helps elevators communicate
    wake_up.set();
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
    //let elevator know that you are properly boarded
    (*boarded)[myElev].set();
    //wait for correct departure event
    while (ElevCU.at(myElev)->get_Location() != wheretogo) {
        (*here_is_floor)[myElev].wait();
    }
    //queue up to get off the elevator
    (*get_off)[myElev].queue();
    //hold for time it takes to get off
    hold(0.5);
    //let people know that you are off
    (*unloaded)[myElev].set();
    
    return;
}


void elevator(long myID) {
    const char* myName = ("Elev" + to_string(myID)).c_str();
    create(myName);
    //start out at the ground level with no passengers
    long num_ppl = 0;
    
    while(clock() < day_length) {
        //wait until wake_up event
        wake_up.wait();
        //
        
        

    }

    return;
}

void control_unit(long myID, long& whereami) {
    //look through want_up and want_down arrays for people
    //at the same time, look through elev_scheduler arrays to see
    //    which elevator is already going to that floor
    // elev_schedule_up[Elevs]
    // elev_schedule_down[Elevs]

    long dest


    return;
}

void elev_move_to(long myID, long& whereami, long& dest, long& dir) {
    //instead of entire trip up or down, sometimes we want to be able to move 
    //  from one location to another in order to pick people up.
    
    //update elev destination
    update_workload.reserve();
    ElevCU.at(myID)->set_Loc(whereami);
    ElevCU.at(myID)->set_Dest(dest);
    ElevCU.at(myID)->set_Dir(dir);
    update_workload.release();

    //hold until elev gets to destination
    long time_int = 5 * sqrt( abs(dest - whereami) );
    hold(time_int);

    //update elev destination
    update_workload.reserve();
    whereami = dest;
    ElevCU.at(myID)->set_Loc(whereami);
    ElevCU.at(myID)->set_Dest(dest);
    ElevCU.at(myID)->set_Dir(dir);
    update_workload.release();

    return;
}


void elev_trip_up (long myID, long& whereami, long& num_ppl) {
    //know you're first destination for pickup of drop off
    long wheretogo = whereami;
    long wheretopick;
    long wheretodrop;
    //while anyone needs pick up or drop off in UP direction
    bool needPickUp = any_pick_up(myID,whereami,UP,wheretopick);
    bool needDropOff = any_drop_off(myID,whereami,UP,wheretodrop);
    while (needPickUp || needDropOff) {
        //which is closer, pick up or drop off?
        if (wheretopick < wheretodrop) {
            //then the pick up is closer
            wheretogo = wheretopick;
        }
        else {
            //then the drop off is closer
            wheretogo = wheretodrop;
        }
        //update elevator destination
        update_workload.reserve();
        ElevCU[myID]->set_Dest(wheretogo);
        update_workload.release();
        
        //hold until you get there
        hold( 5 * sqrt(abs(wheretogo - whereami)) );

        //update elevator location
        whereami = wheretogo;
        update_workload.reserve();
        ElevCU[myID]->set_Loc(whereami);
        update_workload.release();

        //first let people off
        if (want_off[myID][whereami]) {
            unloading(myID,whereami,num_ppl);
        }
        //then let people get on
        if (want_up[whereami]) {
            loading(myID,whereami,UP,num_ppl);
        }
        //find out where the elevator should go next
        needPickUp = any_pick_up(myID,whereami,UP,wheretopick);
        needDropOff = any_drop_off(myID,whereami,UP,wheretodrop);
    }

    return;
}

void elev_trip_down(long myID, long& whereami, long& num_ppl) {
    //know you're first destination for pickup of drop off
    long wheretogo = whereami;
    long wheretopick;
    long wheretodrop;
    //while anyone needs pick up or drop off in UP direction
    bool needPickUp = any_pick_up(myID,whereami,DOWN,wheretopick);
    bool needDropOff = any_drop_off(myID,whereami,DOWN,wheretodrop);
    while (needPickUp || needDropOff) {
        //which is closer, pick up or drop off?
        if (wheretopick > wheretodrop) {
            //then the pick up is closer
            wheretogo = wheretopick;
        }
        else {
            //then the drop off is closer
            wheretogo = wheretodrop;
        }
        //update elevator destination
        update_workload.reserve();
        ElevCU[myID]->set_Dest(wheretogo);
        update_workload.release();
        
        //hold until you get there
        hold( 5 * sqrt(abs(wheretogo - whereami)) );

        //update elevator location
        whereami = wheretogo;
        update_workload.reserve();
        ElevCU[myID]->set_Loc(whereami);
        update_workload.release();

        //first let people off
        if (want_off[myID][whereami]) {
            unloading(myID,whereami,num_ppl);
        }
        //then let people get on
        if (want_down[whereami]) {
            loading(myID,whereami,DOWN,num_ppl);
        }
        //find out where the elevator should go next
        needPickUp = any_pick_up(myID,whereami,DOWN,wheretopick);
        needDropOff = any_drop_off(myID,whereami,DOWN,wheretodrop);
    }

    return;
}

bool any_drop_off(long myID, long whereami, long direction, long& wheretogo) {
    bool dropping_off = false;

    if (direction == UP) {
        for (long i = whereami; i < FLOORS; i++) {
            if (want_off[myID][i]) {
                if (!dropping_off) { 
                    dropping_off = true;
                    wheretogo = i;
                }
            }
        }
        //set wheretogo as highest possible number if don't need to drop off
        if (!dropping_off) {
            wheretogo = FLOORS - 1;
        }
    }
    else if (direction == DOWN) {
        for (long i = whereami; i >= GRND; i--) {
            if (want_off[myID][i]) {
                if (!dropping_off) {
                    dropping_off = true;
                    wheretogo = i;
                }
            }
        }
        //set wheretogo as lowest possible number if don't need to drop off
        if (!dropping_off) {
            wheretogo = GRND;
        }
    }
    else {
        cout << "ERROR in any_drop_off: SLEEPING Elev trying to access function" << endl; 
    }

    return dropping_off;
}

bool any_pick_up(long myID, long whereami, long direction, long& wheretogo) {
    bool picking_up = false;
    
    if (direction == UP) {
        for (long i = whereami; i < FLOORS; i++) {
            if (want_up[i]) {
                if (!picking_up) { 
                    picking_up = true;
                    wheretogo = i;
                }
            }
        }
        //set wheretogo as highest possible number if don't need to pick up
        if (!picking_up) {
            wheretogo = FLOORS - 1;
        }
    }
    else if (direction == DOWN) {
        for (long i = whereami; i >= GRND; i--) {
            if (want_down[i]) {
                if (!picking_up) {
                    picking_up = true;
                    wheretogo = i;
                }
            }
        }
        //set wheretogo as lowest possible number if don't need to pick up
        if (!picking_up) {
            wheretogo = GRND;
        }
    }
    else {
        cout << "ERROR in any_pick_up: SLEEPING Elev trying to access function" << endl; 
    }

    return picking_up;
}

void unloading(long myID, long whereami, long& num_ppl) {
    //let all passengers see our location
    update_workload.reserve();
    ElevCU.at(myID)->set_Loc = whereami;
    update_workload.release();
    //tell them we've reached a new destination
    (*here_is_floor)[myID].set();
    //let passengers queue up for the disembark
    hold(TINY);
    //unload passengers
    while ((*get_off)[myElev].queue_cnt() > 0) {
        //set the event for one person to get off
        (*get_off)[myElev].set();
        //wait until they are off
        (*unloaded)[myElev].wait();
        --num_ppl;
    }
    //need to reset the control unit
    update_workload.reserve();
    want_off[myID][whereami] = false;

    return;
}

void loading(long myID, long whereami, long& num_ppl, long direction) {
    //let all passengers see our location
    elev_loc[myID] = whereami;
    //pick up people going in correct direction
    if (direction == UP) {    
        while ((*mb_up)[whereami].queue_cnt() > 0 ) {
            //pass in elevator ID to waiting passenger
            (*mb_up)[whereami].send(myID);
            //let someone board the elevator
            (*going_up)[myID].set();
            //wait until boarded
            (*boarded)[myID].wait();
            ++num_ppl;
        }
        //reserve facility
        update_workload.reserve();
        //reset the up call at this floor
        want_up[whereami] = false;
        //release facility
        update_workload.release();
    }
    else if (direction == DOWN) {
        while ((*mb_down)[whereami].queue_cnt() > 0 ) {
            //pass in elevator ID to waiting passenger
            (*mb_down)[whereami].send(myID);
            //let someone board the elevator
            (*going_down)[myID].set();
            //wait until boarded
            (*boarded)[myID].wait();
            ++num_ppl;
        }
        //reserve facility
        update_workload.reserve();
        //reset the up call at this floor
        want_down[whereami] = false;
        //release facility
        update_workload.release();
    }
    else {
        cout << "ERROR: Elevator need a correct direction!" << endl;
    }
    return;
}







