#include <CppUTest/CommandLineTestRunner.h>
#include <CppUTest/MemoryLeakWarningPlugin.h>

#include <plog/Log.h>
#include <plog/Appenders/ConsoleAppender.h>

#include "logging/message_only_formatter.hpp"
#include "testutils/string_stream_appender.hpp"

IMPORT_TEST_GROUP(CliTest);
IMPORT_TEST_GROUP(ImporterTest);
IMPORT_TEST_GROUP(MessageOnlyFormatterTest);
IMPORT_TEST_GROUP(PlayCommandTest);
IMPORT_TEST_GROUP(VersionCommandTest);
IMPORT_TEST_GROUP(SettingsDaoTest);
IMPORT_TEST_GROUP(StationsDaoTest);
IMPORT_TEST_GROUP(UtilsTest);

int main(int argc, char* argv[]) {
    try {
        static plog::ConsoleAppender<MessageOnlyFormatter> consoleAppender;
        plog::init(plog::info, &consoleAppender).addAppender(&*STRING_STREAM_APPENDER);
        LOG(plog::info) << "Running cora tests";
        MemoryLeakWarningPlugin::turnOffNewDeleteOverloads();
        return CommandLineTestRunner::RunAllTests(argc, argv);
    } catch (const std::exception& error) {
        LOG(plog::error) << error.what();
    } catch (const std::string& error) {
        LOG(plog::error) << error;
    } catch (const char* error) {
        LOG(plog::error) << error;
    } catch (...) {
        LOG(plog::error) << "error of unknown type";
    }

    return 1;
}
