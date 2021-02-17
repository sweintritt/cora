#pragma once

#include <string>

/**
 * @Brief Internal data structure to store options.
 */
struct Option {

    Option() : opt(' '), longOpt(), hasValue(false), found(false), value(""), description("") {}

   /**
    * @brief Short option
    */
   char opt;

   /**
    * @brief Long option
    */
   std::string longOpt;

   /**
    * @brief True if the option has an argument
    */
   bool hasValue;

   /**
    * @brief True if the option was found when parsing commandline arguments
    */
   bool found;

   /**
    * @brief Contains any value passed to an argument
    */
   std::string value;

   /**
    * @brief Description of the option. Will be printed when using Cli::usage()
    */
   std::string description;
};
