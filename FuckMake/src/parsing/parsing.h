#pragma once

#include <util/list.h>
#include <util/string.h>


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
	void Parse(String string);

public:
	FuckMake(const String& filename);
};