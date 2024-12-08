#pragma once

#include <client/client_engine.hpp>
#include <client/thread_pool.hpp>

enum class CommandType
{
    page,
    filter,
    search
};

class Command
{
public:
    Command(ClientEngine* engine) : _engine{engine} {}
    virtual ~Command() = default;
    virtual void execute() = 0;

protected:
    ClientEngine* _engine;
};

class Invoker
{
public: 
    Invoker() : _threadPool{std::thread::hardware_concurrency()} {}

    void runCommand()
    {
        if (_command)
        {
            auto command = _command;
            _threadPool.enqueue([command]()
            {
                command->execute();
            });
        }
    }

    void setCommand(std::shared_ptr<Command> command)
    {
        if (command != nullptr)
        {
            _command = command;
        }
    }
private:
    ThreadPool _threadPool;
    std::shared_ptr<Command> _command;
};

class PageCommand: public Command
{
public:
    PageCommand(ClientEngine* engine, const int pageNumber) 
        : Command(engine), _pageNumber{pageNumber} {}
    virtual ~PageCommand() {}
    void execute() override
    {
        _engine->page(_pageNumber);
    }
private:
    int _pageNumber;
};

class FilterCommand: public Command
{
public:
    FilterCommand(ClientEngine* engine, const std::map<Column, FilterParams>& filterValues) 
        : Command(engine), _filterValues{filterValues} {}
    virtual ~FilterCommand() {}
    void execute() override
    {
        _engine->filter(_filterValues);
    }
private:
    std::map<Column, FilterParams> _filterValues;
};

class SearchOnCommand: public Command
{
public:
    SearchOnCommand(ClientEngine* engine, const std::map<Column, QString>& searchValues) 
        : Command(engine), _searchValues{searchValues} {}
    virtual ~SearchOnCommand() {}
    void execute() override
    {
        _engine->searchOn(_searchValues);
    }
private:
    std::map<Column, QString> _searchValues;
};

class SearchOffCommand: public Command
{
public:
    SearchOffCommand(ClientEngine* engine) 
        : Command(engine){}
    virtual ~SearchOffCommand() {}
    void execute() override
    {
        _engine->searchOff();
    }
};

class SearchRecordCommand: public Command
{
public:
    SearchRecordCommand(ClientEngine* engine, const int recordNumber)
        : Command(engine), _recordNumber{recordNumber} {}
    virtual ~SearchRecordCommand() {}
    void execute() override
    {
        _engine->searchRecord(_recordNumber);
    }
private:
    int _recordNumber;
};
