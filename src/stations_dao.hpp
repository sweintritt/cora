#pragma once

#include <string>
#include <memory>

#include "station.hpp"

class StationsDao {
    public:
        virtual ~StationsDao() = default;
        virtual void open(const std::string& url) = 0;
        virtual void close() = 0;
        virtual void save(Station& station) = 0;
        virtual std::vector<long> find(const std::string& name, const std::string& genre, const std::string& country) = 0;
        virtual std::shared_ptr<Station> findById(const long id) = 0;
        virtual void deleteAllAddedBy(const std::string& addedBy) = 0;
        virtual std::shared_ptr<Station> getRandom() = 0;
        virtual std::vector<long> getAllIds() = 0;
        virtual void beginTransaction() = 0;
        virtual void commit() = 0;
        virtual void rollback() = 0;
};
