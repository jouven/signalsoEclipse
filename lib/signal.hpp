//20160118
//helper header to deal when having DETACHED (NOT joined!)
//threads and needing to end the process as "elegantly" as possible
#ifndef SIGNALSO_SIGNAL_H_
#define SIGNALSO_SIGNAL_H_

#include <csignal>
#include <functional>
#include <atomic>
#include <cstdint>

//remember to add the needed to the main or just before creating the detached threads
//std::signal(SIGTERM, eines::signal::signal_handler);
//or use the following macros

//*nix
#ifndef __WIN32__
	#define MACRO_signalHandler std::signal(SIGTERM, eines::signal::signal_handler_f); \
	std::signal(SIGINT, eines::signal::signal_handler_f); \
	std::signal(SIGHUP, eines::signal::signal_handler_f);
//use this macro to set the process to the lowest priority
	#define MACRO_niceSignalHander nice(19); \
	MACRO_signalHandler
//windows
#else
	#define MACRO_signalHandler std::signal(SIGTERM, eines::signal::signal_handler_f); \
	std::signal(SIGINT, eines::signal::signal_handler_f); \
	std::signal(SIGBREAK, eines::signal::signal_handler_f);
//windows doesn't have nice
#endif

namespace eines
{
namespace signal
{
//which signal hit so it can be checked outside, by notset/initial/default is -255
extern int signalNumber_f();

//when the program is exiting how often to check the threadcounter to see if all the other threads have ended
//used to check the timeout too, can be modified on the fly, 250 by default
extern std::atomic_uint_fast32_t frequencyCheckMilliseconds_ato;

//if it's greater than 0 other threads are still running
extern uint_fast32_t threadCount_f();

//timeout, if any, to end the program anyway if the other threads aren't ending
extern uint_fast32_t timeOutMilliseconds_f();

//signal handler function it will "notify" the threads to end
//when a signal happens isRunning_f() will start returning false
//and will wait for the threadCount to reach 0, once that happens,
//isTheEnd will start returning true
//if there is a timeout and threadCount hasn't reached 0,
//when timeout is hit isTheEnd will return true anyway
extern void signal_handler_f(int signal_par);

//true when all the threads have ended (threadCounter_ato == 0) or the timeout has been hit,
//with this boolean we can hint the main function that there are no more threads running
//so it can exit "safely"
//can be used on a while without choking the cpu (it has a sleep/wait)
extern bool isTheEnd_f(const uint_fast32_t checkEveryMilliseconds_par_con = frequencyCheckMilliseconds_ato);

//becomes false when a signal is hit,
//this one will choke the cpu if looped with no sleep or operation happening in between
extern bool isRunning_f();

//requires the signal handler to be set
//function to be able to call the signal handler manually uses a -2 value (AFAIK -2 is not used)
extern void stopRunning_f();

//use this function to launch a detached thread, the timeout is the time in milliseconds
//to wait for a thread/s to finish when a signal happens. So when a signal happens and all
//the threads (they should be checking isRunning_f() periodically) are, theoretically, cleaning up/ending
//if the timeout is hit, isTheEnd_f() will return true even if there are threads still running
//and the main, which should be checking isTheEnd_f() should/will be able to "exit" (gracefully or not)
extern void launchThread_f(
	const std::function<void()>& func_par_con
	, const uint_fast32_t timeOut_par_con = 0
);

}
}
#endif /* SIGNALSO_SIGNAL_H_ */
