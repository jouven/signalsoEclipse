//20160121
#include "signal.hpp"

#ifdef DEBUGJOUVEN
#include "backwardSTso/backward.hpp"
#include "comuso/loggingMacros.hpp"
#include <iostream>
#include <cstring>
#endif

#include <thread>
#include <chrono>

namespace eines
{
namespace signal
{
//IMPORTANT to be thread-safe things must be std::atomic,
// volatile std::sig_atomic_t it's the closest thing before C++11 introduced std::atomic

//which signal so it can be checked outside
std::atomic_int signalNumber_ato(-255);
int signalNumber_f()
{
	return signalNumber_ato;
}
//main variable to signal that the program is still running
std::atomic_bool isRunning_ato(true);
//when the program is exiting how often to check the threadcounter
//to see if all the other threads have ended
//used to check the timeout too
std::atomic_uint_fast32_t frequencyCheckMilliseconds_ato(250);
//if it's greater than 0 other threads are still running
std::atomic_uint_fast32_t threadCounter_ato(0);
uint_fast32_t threadCount_f()
{
	return threadCounter_ato;
}
//timeout if any to end the program anyway if the other threads aren't ending in time
std::atomic_uint_fast32_t timeOutMilliseconds_ato(0);
uint_fast32_t timeOutMilliseconds_f()
{
	return timeOutMilliseconds_ato;
}

//true when all the threads have ended (threadCounter_ato == 0) or the timeout has been hit,
//with this boolean we can hint the main function that there are no more threads running
//so it can exit "safely"
std::atomic_bool isTheEnd_ato(false);
bool isTheEnd_f(const uint_fast32_t checkEveryMilliseconds_par_con)
{
	if (not isTheEnd_ato)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(
			checkEveryMilliseconds_par_con));
	}
	return isTheEnd_ato;
}

//function to add a threadcounter and timeout (optional),
//adding a timeout will compare it to the current one
//and use the max value between both as the new timeout
void addThreatAndTimeoutMilliseconds_f(const uint_fast32_t timeOut_par_con)
{
	threadCounter_ato++;
	if (timeOut_par_con > timeOutMilliseconds_ato)
	{
		timeOutMilliseconds_ato = timeOut_par_con;
	}
}

//check the timeout or the threadcount
//and signal theEnd when all the threads finish
void endPhase_f()
{
#ifdef DEBUGJOUVEN
	std::cout << DEBUGDATETIMEANDSOURCE
		<< "BEGIN\n\tfrequencyCheckMilliseconds: " << frequencyCheckMilliseconds_ato
		<< "\n\tthreadCounter: " << threadCounter_ato << std::endl;
#endif
	std::chrono::steady_clock::time_point start;
	if (timeOutMilliseconds_ato > 0)
	{
		//it's safe to use
		//see http://pubs.opengroup.org/onlinepubs/9699919799/functions/V2_chap02.html#tag_15_04
		start = std::chrono::steady_clock::now();
	}
	bool timeoutHit(false);
	while (threadCounter_ato != 0 and not timeoutHit)
	{
#ifdef DEBUGJOUVEN
		std::cout << DEBUGDATETIMEANDSOURCE
			<< "(while) threadCounter: " << threadCounter_ato << std::endl;
#endif
		//it's is safe to use this in a signal handler uses nanosleep
		std::this_thread::sleep_for(std::chrono::milliseconds(
		                                frequencyCheckMilliseconds_ato));
		if (timeOutMilliseconds_ato > 0)
		{
			auto end(std::chrono::steady_clock::now());
			auto elapsed(std::chrono::duration_cast<std::chrono::milliseconds>(
			                 end - start));
			timeoutHit = elapsed.count() > (timeOutMilliseconds_ato and timeOutMilliseconds_ato != 0);
#ifdef DEBUGJOUVEN
			std::cout << DEBUGDATETIMEANDSOURCE
				<< "\n\t(while) elapsed.count(): " << elapsed.count()
				<< "\n\t(while) timeoutHit: " << std::boolalpha << timeoutHit << std::endl;
#endif
		}
	}
#ifdef DEBUGJOUVEN
	std::cout << DEBUGDATETIMEANDSOURCE
		<< "\n\ttimeoutHit: " << std::boolalpha << timeoutHit
		<< "\n\tthreadCounter: " << threadCounter_ato << std::endl;
#endif
	isTheEnd_ato = true;
}

//if the handler keeps reentering and a thread is stuck and it never hits the timeout
//because it enters the handler again before the timeout is hit,
//keep dividing the timeout value by 2 to force eventually it will get hit
std::atomic_bool reenteredHandler_ato(false);
void signal_handler_f(int signal_par)
{
#ifdef DEBUGJOUVEN
	std::cout << DEBUGDATETIMEANDSOURCE << "signal_handler: " << signal_par << std::endl;
#endif
	if (reenteredHandler_ato and timeOutMilliseconds_ato > 0)
	{
		timeOutMilliseconds_ato = timeOutMilliseconds_ato / 2;
	}
	isRunning_ato = false;
	signalNumber_ato = signal_par;
	reenteredHandler_ato = true;
	endPhase_f();
}

//if the program is "running", this is to hint the threads to end
//so after all of them have ended the main one can do it too
bool isRunning_f()
{
	return isRunning_ato;
}

void stopRunning_f()
{
	//if it waits for the handler thread the program will keep running until
	//it takes effect, it make time "some time", if the stopRunning it's at the end of a loop "while is running"
	//it might do another loop, so immediately put isRunning_ato to false
	//even if the signal_handler function does it too
	if (isRunning_ato)
	{
		//if this wasn't in a thread id would block the main thread
		//while everything else is finishing
		std::thread([]()
		{
			signal_handler_f(-2);
		}).detach();
		isRunning_ato = false;
	}
}

void launchThread_f(
	const std::function<void()>& func_par_con
	, const uint_fast32_t timeOut_par_con)
{
	addThreatAndTimeoutMilliseconds_f(timeOut_par_con);
	std::thread([=]()
	{
		func_par_con();
		threadCounter_ato--;
	}).detach();
}

}
}
