#pragma once

#include <util/list.h>
#include <util/string.h>
#include <omp.h>


struct Variable {
	String name;
	String value;
};

struct Action {
	String name;
	List<String> actions;
};

struct Target {
	String name;
	List<String> targets;
};

class FuckMake {
private:
	List<Variable> variables;
	List<Action> actions;
	List<Target> targets;

private:
	void Parse(String& string);
	void ParseVariables(String& string);
	void ParseActions(String& string);
	void ParseTargets(String& string);

	void ProcessVariables(String& string);
	void ProcessFunctions(String& string);
	void ProcessInputOuput(String& string, const String& input, const String& output);

	bool CheckWildcardPattern(const String& source, const String& pattern);

	void ProcessGetFiles(String& string);
	void ProcessDeleteFiles(String& string);
	void ProcessMsg(String& string);
	void ProcessExecuteList(String& string);
	void ProcessExecute(String& string);

	uint64 FindMatchingParenthesis(const String& string, uint64 start);

	Variable* GetVariable(const String& name);
	Action* GetAction(const String& name);
	Target* GetTarget(const String& name);

	omp_lock_t msgMutex;

public:
	FuckMake(const String& filename, const String& target);
};