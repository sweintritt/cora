#include "cli/cli.hpp"

#include <plog/Log.h>

#include <sstream>

Cli::Cli(const std::string& command, const std::string& description)
   : m_command(command)
   , m_description(description)
   , m_options() 
   , m_residualValues() { }

void Cli::addOption(const char opt, const bool hasValue, const std::string& description) {
   addOption(opt, "", hasValue, description);
}

void Cli::addOption(const std::string& longOpt, const bool hasValue, const std::string& description) {
   addOption(' ', longOpt, hasValue, description);
}

void Cli::addOption(const char opt, const std::string& longOpt, const bool hasValue,
         const std::string& description) {
   Option option;
   option.opt = opt;
   option.longOpt = longOpt;
   option.hasValue = hasValue;
   option.description = description;
   option.found = false;
   m_options.push_back(option);
}

void Cli::parse(const std::vector<std::string>& args) {
   reset();
   LOG(plog::debug) << "parsing arguments";
   for (unsigned int i = 1; i < args.size(); ++i)    {
      std::string current = args[i];
      LOG(plog::debug) << "checking: " << current;
      if (current[0] == '-')       {
         current.erase(0, 1);
         // remove possible second hyphen
         if (current[0] == '-') {
            current.erase(0, 1);
         }

         LOG(plog::debug) << "found possible option: " << current;
         for (auto& it : m_options) {
            LOG(plog::debug) << "checking against: " << it.opt << ", " << it.longOpt;
            if (current.compare(std::string{it.opt}) == 0 || current.compare(it.longOpt) == 0) {
               LOG(plog::debug) << "setting option: " << current;
               it.found = true;
               if (it.hasValue && (i + 1) < args.size()) {
                  LOG(plog::debug) << "setting value for option: " << current;
                  if (args[i + 1][0] != '-') {
                     LOG(plog::debug) << "found value: " << args[i + 1] << " for option: " << current;
                     it.value = args[i + 1];
                     ++i;
                  }
               }
            }
         }
      } else {
         // FIXME Unknown options will not be stored in the residual vector
         m_residualValues.push_back(current);
         LOG(plog::debug) << "residual value: " << current;
      }
   }
}

bool Cli::hasOption(const char opt) const {
   const auto result = std::find_if(m_options.begin(), m_options.end(), [opt](const Option& o){ return o.opt == opt && o.found; });

   if (result != m_options.end()) {
      LOG(plog::debug) << "searched for '" << opt << "', found '" << result->opt << "' isSet:" << result->found;
      return true;
   }

   return false;
}

bool Cli::hasOption(const std::string& longOpt) const {
   const auto result = std::find_if(m_options.begin(), m_options.end(), [longOpt](const Option& o){ return o.longOpt.compare(longOpt) == 0 && o.found; });

   if (result != m_options.end()) {
      LOG(plog::debug) << "searched for '" << longOpt << "', found '" << result->longOpt << "' isSet:" << result->found;
      return true;
   }

   return false;
}

bool Cli::hasValue(const char opt) const {
   const auto result = std::find_if(m_options.begin(), m_options.end(), [opt](const Option& o){ return o.opt == opt && o.found && o.hasValue; });

   if (result != m_options.end()) {
      return !result->value.empty();
   }

   return false;
}

bool Cli::hasValue(const std::string& longOpt) const {
   const auto result = std::find_if(m_options.begin(), m_options.end(), [longOpt](const Option& o){ return o.longOpt.compare(longOpt) == 0 && o.found && o.hasValue; });

   if (result != m_options.end()) {
      return !result->value.empty();
   }

   return false;
}

std::string Cli::getValue(const char opt) const {
   return getValue(opt, "");
}

std::string Cli::getValue(const char opt, const std::string& defaultValue) const {
   const auto result = std::find_if(m_options.begin(), m_options.end(), [opt](const Option& o){ return o.opt == opt && o.found && o.hasValue; });

   if (result != m_options.end()) {
      return result->value;
   }

   return defaultValue;
}

std::string Cli::getValue(const std::string& longOpt) const {
   return getValue(longOpt, "");
}

std::string Cli::getValue(const std::string& longOpt, const std::string& defaultValue) const {
   const auto result = std::find_if(m_options.begin(), m_options.end(), [longOpt](const Option& o){ return o.longOpt.compare(longOpt) == 0 && o.found && o.hasValue; });

   if (result != m_options.end()) {
      return result->value;
   }

   return defaultValue;
}

void Cli::reset() {
   for (auto& option : m_options) {
      option.value = "";
      option.found = false;
   }
}

const std::string Cli::command() const {
   return m_command;
}

const std::string Cli::description() const {
   return m_description;
}

const std::vector<std::string>& Cli::getResidualValues() const {
   return m_residualValues;
}

const std::string Cli::usage() const {
   std::stringstream stream;
   stream << "NAME" << std::endl;
   stream << "  " << m_command << " - " << m_description << std::endl << std::endl;
   stream << "SYNOPSIS" << std::endl;
   stream << "  " << m_command << " [OPTIONS]" << std::endl << std::endl;
   stream << "DESCRIPTION" << std::endl;
   for (auto const& it : m_options) {
      stream << "  ";

      if (it.opt != ' ') {
         stream << "-" << it.opt;
      }

      if (it.opt != ' ' && !it.longOpt.empty()) {
         stream << ", ";
      }

      if (!it.longOpt.empty()) {
         stream << "--" << it.longOpt;
      }

      if (it.hasValue) {
         stream << " <VALUE> ";
      }

      // Wrap long descriptions
      const std::string indent = "              ";
      const std::string newLineIndent = "\n              ";
      std::string wrappedDescription = it.description;
      unsigned int line = 0;

      for (unsigned int i = 0; i < wrappedDescription.size(); ++i) {
         if (line >= 60 && wrappedDescription[i] == ' ') {
            wrappedDescription.replace(i, 1, newLineIndent);
            line = 0;
         }
         ++line;
      }

      stream << std::endl << indent << wrappedDescription << std::endl << std::endl;
   }
   return stream.str();
}
