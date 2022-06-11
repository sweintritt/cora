#include <CppUTest/CommandLineTestRunner.h>
#include <CppUTest/MemoryLeakWarningPlugin.h>

#include <plog/Log.h>
#include <plog/Appenders/ConsoleAppender.h>

IMPORT_TEST_GROUP(CliTest);
IMPORT_TEST_GROUP(SettingsTest);
IMPORT_TEST_GROUP(SettingsDaoTest);
IMPORT_TEST_GROUP(StationsDaoTest);
IMPORT_TEST_GROUP(UtilsTest);

int main(int argc, char* argv[]) {
    try {
        static plog::ConsoleAppender<plog::TxtFormatter> consoleAppender;
        plog::init(plog::debug, &consoleAppender);
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
