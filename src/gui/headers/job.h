#pragma once
#include "../headers/engine.h"

struct Job {
public:
	Job();
	~Job();

	virtual void execute();
	virtual std::string toString();
	virtual std::string humanReadableName();
};

struct PrepareForJoinJob : Job {
public:
	PrepareForJoinJob();
	~PrepareForJoinJob();

	void execute() override;
	std::string toString() override;
	std::string humanReadableName() override;
};

struct ScanJob : Job {
public:
    std::string command;

    ScanJob(std::string command);
    ~ScanJob();

    void execute() override;
    std::string toString() override;
    std::string humanReadableName() override;
};