#pragma once

#include <client/client_engine.hpp>
#include <client/thread_pool.hpp>

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
        _threadPool.enqueue(std::bind(&Command::execute, _command));
    }

    void setCommand(Command* command)
    {
        if (command != nullptr)
        {
            _command = command;
        }
    }
private:
    ThreadPool _threadPool;
    Command* _command;
};

class PageCommand: public Command
{
public:
    PageCommand(ClientEngine* engine) : Command(engine), _pageNumber{0} {}
    virtual ~PageCommand() {}
    void execute() override
    {
        _engine->page(_pageNumber);
    }

    void setPage(const int pageN) {_pageNumber = pageN;}

private:
    int _pageNumber;
};

class FilterCommand: public Command
{
public:
    FilterCommand(ClientEngine* engine) : Command(engine), _filterValues{} {}
    virtual ~FilterCommand() {}
    void execute() override
    {
        _engine->filter(_filterValues);
    }

    void setFilter(const std::map<Column, QString>& filterValues) {_filterValues = filterValues;}

private:
    std::map<Column, QString> _filterValues;
};
