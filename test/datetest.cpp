#include <iostream>
#include <ctime>
// #include <thread>
#include <unistd.h>
// #include <chrono>
#include "date.h"

void afterTest()
{
   std::cout << "Testing after():" << std::endl;
   p1::logger::Date d2;
   // std::this_thread::sleep_for(std::chrono::seconds(1));
   sleep(1);
   p1::logger::Date d1;
   std::cout << d1.toString() << " (after) " << d2.toString() << " = " << d1.after(d2) << std::endl;
}

void equalsTest()
{
   std::cout << "Testing equals():" << std::endl;
   p1::logger::Date d1;
   p1::logger::Date d2;
   // std::this_thread::sleep_for(std::chrono::seconds(1));
   sleep(1);
   p1::logger::Date d3;
   std::cout << d1.toString() << " (equals) " << d2.toString() << " = " << d1.equals(d2) << std::endl;
   std::cout << d1.toString() << " (equals) " << d3.toString() << " = " << d1.equals(d3) << std::endl;
   std::cout << d2.toString() << " (equals) " << d3.toString() << " = " << d2.equals(d3) << std::endl;
}

void beforeTest()
{
   std::cout << "Testing before():" << std::endl;
   p1::logger::Date d1;
   // std::this_thread::sleep_for(std::chrono::seconds(1));
   sleep(1);
   p1::logger::Date d2;
   std::cout << d1.toString() << " (before) " << d2.toString() << " = " << d1.before(d2) << std::endl;
}

void constructorsTest()
{
   std::cout << "Testing Date():" << std::endl;
   time_t timestamp = time(0);

   p1::logger::Date d1(timestamp);
   std::cout << "d1: " << d1.toString() << std::endl;

   p1::logger::Date d2(*localtime(&timestamp));
   std::cout << "d2: " << d2.toString() << std::endl;

   p1::logger::Date d3(d2);
   std::cout << "d3: " << d3.toString() << std::endl;
}

void toStringTest()
{
   std::cout << "Testing toString():" << std::endl;
   p1::logger::Date date;
   std::cout << "Now: " << date.toString() << std::endl;
}

int main()
{
   afterTest();
   equalsTest();
   beforeTest();
   constructorsTest();
   toStringTest();
   return 0;
}
