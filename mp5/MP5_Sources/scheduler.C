/*
 File: scheduler.C
 
 Author: Rajendra Thottempudi
 Date  :
 
 */

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "scheduler.H"
#include "thread.H"
#include "console.H"
#include "utils.H"
#include "assert.H"
#include "simple_keyboard.H"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* METHODS FOR CLASS   S c h e d u l e r  */
/*--------------------------------------------------------------------------*/

Scheduler::Scheduler() {
  //assert(false);
  Console::puts("Constructed Scheduler.\n");
}

void Scheduler::yield() {
  bool iEnab = false;
  iEnab = Machine::interrupts_enabled();
    Console :: puts("Currently Interrupts (in yield method)are :");
            Console :: puti(iEnab);
            Console :: puts("\n");
  if(iEnab){
  	Machine::disable_interrupts();
  }
      Console :: puts("Disabled the interrupts (in yield method)");
                  Console :: puts("\n");
  bool received = false;
  Thread* aglaThread = queue.getFirst();
  received = true;
  queue.pop(); 
  bool iDis = false;
  iEnab = Machine::interrupts_enabled();
  iDis = Machine::interrupts_enabled();
        Console :: puts("Currently interrupts are (end of yield method)");
        Console :: puti(iEnab);
                    Console :: puts("\n");
  if(!iDis){
  	Machine::enable_interrupts();
  }
  iEnab = Machine::interrupts_enabled();
  iDis = Machine::interrupts_enabled();
  Thread :: dispatch_to(aglaThread);
  //assert(false);
}

void Scheduler::resume(Thread * _thread) {
  bool iEnab = false;
  iEnab = Machine::interrupts_enabled();
  if(iEnab){
  	Machine::disable_interrupts();
  }
  queue.insert(_thread);
  bool iDis = false;
  iDis = Machine::interrupts_enabled();
  if(!iDis){
  	Machine::enable_interrupts();
  }
  iEnab = Machine::interrupts_enabled();
  iDis = Machine::interrupts_enabled();
  //assert(false);
}

void Scheduler::add(Thread * _thread) {
  bool iEnab = false;
  iEnab = Machine::interrupts_enabled();
  if(iEnab){
  	Machine::disable_interrupts();
  }
  //assert(false);
  queue.insert(_thread);
  bool iDis = false;
  iDis = Machine::interrupts_enabled();
  if(!iDis){
  	Machine::enable_interrupts();
  }
  iEnab = Machine::interrupts_enabled();
  iDis = Machine::interrupts_enabled();
}

void Scheduler::terminate(Thread * _thread) {
  bool iEnab = false;
  iEnab = Machine::interrupts_enabled();
  if(iEnab){
  	Machine::disable_interrupts();
  }

  //assert(false);
  queue.deleteThisNode(_thread);
  bool iDis = false;
  iDis = Machine::interrupts_enabled();
  if(!iDis){
  	Machine::enable_interrupts();
  }
  iEnab = Machine::interrupts_enabled();
  iDis = Machine::interrupts_enabled();
}
