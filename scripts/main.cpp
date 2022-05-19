#include <iostream>
#include <Spinnaker.h>
#include <functional>
#include <boost/shared_ptr.hpp>

#include "myIndent.h"
#include "sensor.h"
#include "simpleSensor.h"

boost::mutex io_mutex;

void thread_fun(boost::barrier& cur_barier, boost::atomic<int>& current)
{
    ++current;
    cur_barier.wait();
    boost::lock_guard<boost::mutex> locker(io_mutex);
    std::cout << current << std::endl;
}

int main(int, char**) {
    std::cout << "Hello, world!\n";
    
    SimpleSensor a("i", "i");
    a.printPls();
    auto b = a.AtomicAcquire();
    std::cout << b.count() << "\n";
    std::cout << a.sensorName << "\n";

    boost::shared_ptr<int> p1{new int{1}};
    std::cout << *p1 << '\n';

    boost::barrier bar(3);
    boost::atomic<int> current(0);
    boost::thread thr1(boost::bind(&thread_fun, boost::ref(bar), boost::ref(current)));
    boost::thread thr2(boost::bind(&thread_fun, boost::ref(bar), boost::ref(current)));
    boost::thread thr3(boost::bind(&thread_fun, boost::ref(bar), boost::ref(current)));
    thr1.join();
    thr2.join();
    thr3.join();
}