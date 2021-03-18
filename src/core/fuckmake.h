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
	void ProcessTarget(const Target* target);

	bool CheckWildcardPattern(const String& source, const String& pattern);

	void ProcessGetFiles(String& string);
	void ProcessDeleteFiles(String& string);
	void ProcessMsg(String& string);
	void ProcessExecuteList(String& string);
	void ProcessExecute(String& string);
	void ProcessExecuteTarget(String& string);

	uint64 FindMatchingParenthesis(const String& string, uint64 start);

	void InitializeBuiltinVaraibles();

	Variable* GetVariable(const String& name);
	Action* GetAction(const String& name);
	Target* GetTarget(const String& name);

	omp_lock_t msgMutex;

	String rootDir;
	bool rootSet;
public:
	FuckMake(const String& rootDir, const String& filename, const String& target);
};