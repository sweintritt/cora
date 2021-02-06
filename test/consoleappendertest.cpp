#include <iostream>
#include "logger.h"
#include "appender.h"
#include "consoleappender.h"
#include "fileappender.h"

void checkAllLevels(p1::logger::Logger* logger)
{
   logger->debug("Debug message");
   logger->info("Info message");
   logger->warn("Warn message");
   logger->error("Error message");
   logger->fatal("Fatal message");
}

void testLogger()
{
   p1::logger::ConsoleAppender consoleAppender;
   p1::logger::FileAppender fileAppender("test.log");
   p1::logger::Logger::add(&consoleAppender);
   p1::logger::Logger::add(&fileAppender);
   p1::logger::Logger* logger = p1::logger::Logger::logger("TestLogger");

   p1::logger::Logger::globalLevel(p1::logger::Logger::DEBUG);
   std::cout << "Level set to DEBUG:" << std::endl;
   checkAllLevels(logger);

   p1::logger::Logger::globalLevel(p1::logger::Logger::INFO);
   std::cout << "Level set to INFO:" << std::endl;
   checkAllLevels(logger);

   p1::logger::Logger::globalLevel(p1::logger::Logger::WARN);
   std::cout << "Level set to WARN:" << std::endl;
   checkAllLevels(logger);

   p1::logger::Logger::globalLevel(p1::logger::Logger::ERROR);
   std::cout << "Level set to ERROR:" << std::endl;
   checkAllLevels(logger);

   p1::logger::Logger::globalLevel(p1::logger::Logger::FATAL);
   std::cout << "Level set to FATAL:" << std::endl;
   checkAllLevels(logger);

   p1::logger::Logger::globalLevel(p1::logger::Logger::OFF);
   std::cout << "Level set to OFF:" << std::endl;
   checkAllLevels(logger);
}

void testUnvalidLevel()
{
   p1::logger::ConsoleAppender appender;
   p1::logger::Logger::add(&appender);
   p1::logger::Logger* logger = p1::logger::Logger::logger("TestLogger");

   try
   {
      p1::logger::Logger::globalLevel(-1);
   }
   catch (const std::string e)
   {
      std::cout << "EXCEPTION: " << e << std::endl;
   }

   try
   {
      p1::logger::Logger::globalLevel(6);
   }
   catch (const std::string e)
   {
      std::cout << "EXCEPTION: " << e << std::endl;
      // FIXME After the exception is thrown the info-call causes an memory access error (segmentation fault?)
      return;
   }

   logger->info("done.");
}

int main()
{
   try
   {
      testLogger();
      testUnvalidLevel();
   }
   catch (const std::string e)
   {
      std::cout << e << std::endl;
   }

   return 0;
}
