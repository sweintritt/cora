#pragma once

#include "cli/option.hpp"

#include <string>
#include <vector>
#include <list>

/**
 * @brief Command line parser
 */
class Cli {
   public:
      /**
       * Constructor
       * @param[in] commmand name of the command.
       * @param[in] description description of the command.
       */
      Cli(const std::string& command, const std::string& description);
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
      void parse(const int argc, char* argv[]);

      /**
       * @brief Parses the command line arguments for all specified options.
       * @param[in] args command line arguments
       */
      void parse(const std::vector<std::string>& args);

      /**
       * @brief Returns true if the given option is possible and has been passed
       *        as command line argument.
       * @param[in] opt short option name
       * @return true if the given option is possible and has been passed
       *         as command line argument.
       */
      bool hasOption(const char opt) const;

      /**
       * @brief Returns true if the given option is possible and has been passed
       *        as command line argument.
       * @param[in] longOpt long option name
       * @return true if the given option is possible and has been passed
       *         as command line argument.
       */
      bool hasOption(const std::string& longOpt) const;

      /**
       * @brief Returns true if the given option has a value and a value was passed
       *        as command line argument.
       * @param[in] opt short option name
       * @return true if the given option has a value and a value was passed
       *         as command line argument.
       */
      bool hasValue(const char opt) const;

      /**
       * @brief Returns true if the given option has a value and a value was passed
       *        as command line argument.
       * @param[in] longOpt long option name
       * @return true if the given option has a value and a value was passed
       *         as command line argument.
       */
      bool hasValue(const std::string& longOpt) const;

      /**
       * @brief Returns the value for the specified option or an empty string if the option has no
       *        value or no value was passed on command line
       * @param[in] opt short option name
       * @return the value for the specified option or an empty string if the option has no value or no
       *         value was passed on command line
       */
      std::string getValue(const char opt);
      // TODO getValue(opt, default);
      /**
       * @brief Returns the value for the specified option or an empty string if the option has no
       *        value or no value was passed on command line
       * @param[in] longOpt long option name
       * @return the value for the specified option or an empty string if the option has no value or no
       *         value was passed on command line
       */
      std::string getValue(const std::string& longOpt);

      /**
       * Reset the values of all options for another parse. parse will call this itself.
       */
      void reset();

      /**
       * @brief Return  a string containing a short help for all options.
       * @return a string containing a short help for all options.
       */
      const std::string usage() const;

   private:
      /**
       * Name of the command.
       */
      const std::string m_command;
      /**
       * Description of the command.
       */
      const std::string m_description;
      /**
       * Options.
       */
      std::list<Option> m_options;
};