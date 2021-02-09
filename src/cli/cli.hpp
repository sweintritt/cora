#pragma once

#include <string>
#include <list>

/**
 * @Brief Internal data structure to store options.
 */
struct Option { // TODO move to separate class
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

/**
 * @brief Command line parser
 */
class Cli {
   public:
      /**
       * @brief Adds the given parameters as new option.
       * @param[in] opt short name of the option
       * @param[in] hasValue true if the option as an additional value
       * @param[in] description description of this option.
       */
      void addOption(const char opt, const bool hasValue, const std::string& description);

      /**
       * @brief Adds the given parameters as new option.
       * @param[in] longOpt long name of the option
       * @param[in] hasValue true if the option as an additional value
       * @param[in] description description of this option.
       */
      void addOption(const std::string& longOpt, const bool hasValue, const std::string& description);

      /**
       * @brief Adds the given parameters as new option.
       * @param[in] opt short name of the option
       * @param[in] longOpt long name of the option
       * @param[in] hasValue true if the option as an additional value
       * @param[in] description description of this option.
       */
      void addOption(const char opt, const std::string& longOpt, const bool hasValue,
               const std::string& description);

      /**
       * @brief Parses the command line arguments for all specified options.
       * @param[in] argc length of the argument array
       * @param[in] argv command line arguments
       */
      void parse(int argc, char* argv[]); // TODO argv should be const

      /**
       * @brief Returns true if the given option is possible and has been passed
       *        as command line argument.
       * @param[in] opt short option name
       * @return true if the given option is possible and has been passed
       *         as command line argument.
       */
      bool hasOption(const char opt);

      /**
       * @brief Returns true if the given option is possible and has been passed
       *        as command line argument.
       * @param[in] longOpt long option name
       * @return true if the given option is possible and has been passed
       *         as command line argument.
       */
      bool hasOption(const std::string& longOpt);

      /**
       * @brief Returns true if the given option has a value and a value was passed
       *        as command line argument.
       * @param[in] opt short option name
       * @return true if the given option has a value and a value was passed
       *         as command line argument.
       */
      bool hasValue(const char opt);

      /**
       * @brief Returns true if the given option has a value and a value was passed
       *        as command line argument.
       * @param[in] longOpt long option name
       * @return true if the given option has a value and a value was passed
       *         as command line argument.
       */
      bool hasValue(const std::string& longOpt);

      /**
       * @brief Returns the value for the specified option or an empty string if the option has no
       *        value or no value was passed on command line
       * @param[in] opt short option name
       * @return the value for the specified option or an empty string if the option has no value or no
       *         value was passed on command line
       */
      std::string getValue(const char opt);

      /**
       * @brief Returns the value for the specified option or an empty string if the option has no
       *        value or no value was passed on command line
       * @param[in] longOpt long option name
       * @return the value for the specified option or an empty string if the option has no value or no
       *         value was passed on command line
       */
      std::string getValue(const std::string& longOpt);

      /**
       * @brief
       * @param[in] application name of the application that will be used in the usage page
       * @return a string containing a short help for all options.
       */
      std::string usage(const std::string& application);

   private:
      std::list<Option> m_options;
};