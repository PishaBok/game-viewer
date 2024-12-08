#pragma once

#include <server/database_manager.hpp>
#include <libcommon/requests/unique_values.hpp>

class UniqueValuesExecutor: public UniqueValuesRequest
{
public:
    UniqueValuesExecutor(DatabaseManager& dbManager);
    virtual ~UniqueValuesExecutor() = default;

    std::unique_ptr<Response> execute() override;
private:
    DatabaseManager& _dbManager;

    QStringList getUniqueValues();
};