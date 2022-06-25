#pragma once

#include "db/dao.hpp"
#include "db/station.hpp"

#include <sqlite3.h>
#include <memory>

class StationsDao : public Dao {
    public:
        StationsDao();
        virtual ~StationsDao() = default;
        virtual void save(Station& station);
        virtual std::vector<long> find(const std::string& name, const std::string& genre, const std::string& country);
        virtual std::shared_ptr<Station> findById(const long id);
        virtual void deleteAllAddedBy(const std::string& addedBy);
        virtual std::shared_ptr<Station> getRandom();
        virtual std::vector<long> getAllIds();
    
    protected:
        void onOpen() override;
        bool onClose() override;

    private:
        sqlite3_stmt* insertStationStmnt;
        sqlite3_stmt* findStationByIdStmnt;
        sqlite3_stmt* findStationStmnt;
        sqlite3_stmt* deleteByAddedByStmnt;
        sqlite3_stmt* getAllIdsStmnt;
        sqlite3_stmt* getRandomStationStmnt;

        std::string serializeUrls(const std::vector<std::string>& urls) const;
        std::vector<std::string> deserializeUrls(const std::string& urls) const;
        Station getStation(sqlite3_stmt* stmnt);
};
