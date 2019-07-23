#include "parsing.h"

#include <util/util.h>

FuckMake::FuckMake(const String& filename, const String& target) {
	uint64 size = 0;
	uint8* data = ReadFile(filename.str, &size);

	variables.Reserve(1024);
	actions.Reserve(1024);
	targets.Reserve(1024);

	String string((const char* const)data, size);

	Parse(string);

	Target* t = GetTarget(target);

	if (!t) {
		Log(LogLevel::Error, "No target named \"%s\"", target.str);
		exit(1);
	}

	List<String>& ts = t->targets;

	for (uint64 i = 0; i < ts.GetCount(); i++) {
		String s = ts[i];
		ProcessVariables(s);
		ProcessFunctions(s);
	}
}

void FuckMake::Parse(String& string) {
	uint64 fuckMakeStart = string.Find("!FuckMake");

	if (fuckMakeStart == ~0) {
		Log(LogLevel::Error, "Not a Fuckfile!");
		exit(1);
	} 

	if (fuckMakeStart != 0) {
		string.Remove(0, fuckMakeStart+9);
	}

	ParseVariables(string);
	ParseActions(string);
	ParseTargets(string);
}

void FuckMake::ParseVariables(String& string) {
	uint64 equalIndex = 0;

	while ((equalIndex = string.Find('=')) != ~0) {
		uint64 start = string.FindReversed('\n', equalIndex) + 1;
		uint64 end = string.Find('\n', equalIndex);

		if (start == ~0 || end == ~0) {
			Log(LogLevel::Error, "Error");
			exit(1);
		}

		Variable var;

		var.name = string.SubString(start, equalIndex - 1).RemoveWhitespace();
		var.value = string.SubString(equalIndex + 1, end - 1);

		if (GetVariable(var.name)) {
			Log(LogLevel::Error, "Variable \"%s\" already exist", var.name.str);
			exit(1);
		}

		string.Remove(start, end - 1);
		equalIndex = string.Find('=');

		ProcessVariables(var.value);
		ProcessFunctions(var.value);

		variables.Add(var);
	}
}

void FuckMake::ParseActions(String& string) {
	uint64 openBracket = 0;

	while ((openBracket = string.Find('{')) != ~0) {
		uint64 start = string.FindReversed('\n', openBracket)+1;
		uint64 end = string.Find('}', openBracket);

		String tmpValue = string.SubString(openBracket + 1, end - 1);

		ProcessVariables(tmpValue);


		Action action;
		action.name = string.SubString(start, openBracket - 1).RemoveWhitespace();
		action.actions = tmpValue.Split("\n", false);

		string.Remove(start, end);

		actions.Add(action);
	}
}

void FuckMake::ParseTargets(String& string) {
	uint64 colon = 0;

	while ((colon = string.Find(':')) != ~0) {
		uint64 start = string.FindReversed('\n', colon)+1;
		uint64 end = string.Find(':', colon + 1);

		if (end == ~0) {
			end = string.length - 1;
		} else {
			end = string.FindReversed('\n', end);
		}

		String tmpValue = string.SubString(colon + 1, end);

		ProcessVariables(tmpValue);

		Target target;
		target.name = string.SubString(start, colon - 1).RemoveWhitespace();
		target.targets = tmpValue.Split("\n", false);

		string.Remove(start, end);

		targets.Add(target);
	}
}

void FuckMake::ProcessVariables(String& string) {
	uint64 index = 0;
	
	while ((index = string.Find("%(")) != ~0) {
		uint64 start = index;
		uint64 end = string.Find(')', start);

		String name = string.SubString(start+2, end-1);

		Variable* var = GetVariable(name);

		if (var == nullptr) {
			Log(LogLevel::Error, "Variable \"%s\" doesn't exist!", name.str);
			exit(1);
		}

		string.Insert(start, end, var->value);
	}
}

void FuckMake::ProcessFunctions(String& string) {
	uint64 parenthesis = 0;

	while ((parenthesis = string.Find('(', parenthesis + 1)) != ~0) {
		if (string[parenthesis - 1] == '%') continue;

		uint64 start = string.FindReversedOr(" \n\r\t)(=,", parenthesis-1)+1;

		String functionName = string.SubString(start, parenthesis-1).RemoveWhitespace();

		uint64 closingParenthesis = FindMatchingParenthesis(string, parenthesis);

		String tmp = string.SubString(parenthesis + 1, closingParenthesis - 1).RemoveWhitespace(true);

		ProcessFunctions(tmp);

		if (functionName == "GetFiles") {
			ProcessGetFiles(tmp);
		} else if (functionName == "DeleteFiles") {
			ProcessDeleteFiles(tmp);
		} else if (functionName == "Msg") {
			ProcessMsg(tmp);
		} else if (functionName == "ExecuteList") {
			ProcessExecuteList(tmp);
		} else if (functionName == "Execute") {
			ProcessExecute(tmp);
		}

		string.Insert(start, closingParenthesis, tmp);
	}
}

void FuckMake::ProcessInputOuput(String& string, const String& input, const String& output) {
	uint64 index = 0;

	while ((index = string.Find("%Input")) != ~0) {
		string.Insert(index, index+ 5, input);
	}

	while ((index = string.Find("%Output")) != ~0) {
		string.Insert(index, index + 6, output);
	}
}

bool FuckMake::CheckWildcardPattern(const String& source, const String& pattern) {
	uint64 previous = pattern.Find('*');

	if (previous == ~0) {
		Log(LogLevel::Error, "Invalid wildcard \"%s\"", pattern.str);
		exit(1);
	}

	uint64 numAsterix = pattern.Count("*");

	if (numAsterix == 1) {
		if (previous == 0) {
			if (source.EndsWith(pattern.SubString(1, pattern.length - 1))) {
				return true;
			}
		} else if (previous == pattern.length - 1) {
			if (source.StartsWith(pattern.SubString(0, pattern.length - 2))) {
				return true;
			}
		} else {
			if (source.StartsWith(pattern.SubString(0, previous - 1)) && source.EndsWith(pattern.SubString(previous + 1, pattern.length - 1))) {
				return true;
			}
		}
	} else {
		if (previous > 0) {
			if (!source.StartsWith(pattern.SubString(0, previous - 1))) return false;
		}

		uint64 next = pattern.Find('*', previous + 1);
		uint64 offset = 0;

		for (uint64 i = 1; i < numAsterix; i++) {
			String tmp = pattern.SubString(previous + 1, next - 1);

			if ((offset = source.Find(tmp, offset)) == ~0) {
				break;
			}

			offset += tmp.length;

			previous = next;
			next = pattern.Find('*', previous + (i + 1 < numAsterix ? 1 : 0));
		}

		if (offset == ~0) return false;

		if (next < pattern.length - 1) {
			if (source.EndsWith(pattern.SubString(next + 1, pattern.length - 1))) {
				return true;
			}
		} else {
			return true;
		}
	}

	return false;
}

void FuckMake::ProcessGetFiles(String& string) {
	uint64 firstComma = string.Find(',');
	uint64 secondComma = string.Find(',', firstComma + 1);
	
	String directory("*");
	String wildcard;
	String exclusion;

	if (firstComma > 0) {
		directory = string.SubString(0, firstComma-1);
	}

	if (secondComma - firstComma > 1) {
		wildcard = string.SubString(firstComma + 1, secondComma - 1);
	}

	if (secondComma < string.length - 1) {
		exclusion = string.SubString(secondComma + 1, string.length - 1);
	}

	if (!directory.EndsWith("/") && !directory.EndsWith("*")) {
		directory.Append("/");
	}

	string.Remove(0, string.length - 1);

	List<FileInfo> files = ScanDirectory(directory);

	List<String> wildcards = wildcard.Split(" ");
	List<String> exclusions = exclusion.Split(" ");

	for (uint64 i = 0; i < files.GetCount(); i++) {
		String& file = files[i].filename;

		bool included = false;

		for (uint64 j = 0; j < wildcards.GetCount(); j++) {
			if (CheckWildcardPattern(file, wildcards[j])) {
				included = true;
				break;
			}
		}

		if (included) {
			for (uint64 j = 0; j < exclusions.GetCount(); j++) {
				if (CheckWildcardPattern(file, exclusions[j])) included = false;
			}

			if (included) string.Append(file + " ");
		}
	}


}

void FuckMake::ProcessDeleteFiles(String& string) {
	List<String> files = string.Split(" ");

	for (uint64 i = 0; i < files.GetCount(); i++) {
		::remove(files[i].str);
	}
}

void FuckMake::ProcessMsg(String& string) {
	ProcessVariables(string);
	ProcessFunctions(string);

	Log(LogLevel::Info, "%s", string.str);

	string.Remove(0, string.length - 1);
}

void FuckMake::ProcessExecuteList(String& string) {
	uint64 firstComma = string.Find(',');
	uint64 secondComma = string.Find(',', firstComma + 1);

	String file;
	String outdir;

	String a = string.SubString(0, firstComma - 1).RemoveWhitespace();
	Action* action = GetAction(a);

	if (secondComma - firstComma == 1) {
		Log(LogLevel::Error, "No input files to action \"%s\"", a.str);
		exit(1);
	}

	file = string.SubString(firstComma + 1, secondComma - 1);
	outdir = string.SubString(secondComma + 1, string.length - 1).RemoveWhitespace(true);
	
	if (!action) {
		Log(LogLevel::Error, "No action named \"%s\"", a.str);
		exit(1);
	}

	List<String>& actions = action->actions;
	List<String> files = file.Split(" ");

	for (uint64 i = 0; i < files.GetCount(); i++) {
		for (uint64 j = 0; j < actions.GetCount(); j++) {
			String ac = actions[j];
			ProcessVariables(ac);

			String tmp = files[i].RemoveWhitespace(true);
			String tmp2 = outdir + tmp + ".obj";

			ProcessInputOuput(ac, tmp, tmp2);
			ProcessFunctions(ac);

			uint64 index = ac.Find('!');

			if (index == ~0) continue;

			CreateFolderAndFile(tmp2.str);

			system(ac.SubString(index + 1, ac.length-1).str);
		}
	}
}

void FuckMake::ProcessExecute(String& string) {
	uint64 firstComma = string.Find(',');
	uint64 secondComma = string.Find(',', firstComma + 1);

	String file;
	String outdir;

	String a = string.SubString(0, firstComma - 1).RemoveWhitespace();
	Action* action = GetAction(a);

	if (secondComma - firstComma == 1) {
		Log(LogLevel::Error, "No input files to action \"%s\"", a.str);
		exit(1);
	}


	file = string.SubString(firstComma + 1, secondComma - 1).RemoveWhitespace(true);
	outdir = string.SubString(secondComma + 1, string.length - 1).RemoveWhitespace(true);

	if (!action) {
		Log(LogLevel::Error, "No action named \"%s\"", a.str);
		exit(1);
	}

	List<String>& actions = action->actions;
	for (uint64 i = 0; i < actions.GetCount(); i++) {
		String ac = actions[i];
		ProcessVariables(ac);
		ProcessInputOuput(ac, file, outdir);
		ProcessFunctions(ac);

		uint64 index = ac.Find('!');

		if (index == ~0) continue;

		CreateFolderAndFile(outdir);

		system(ac.SubString(index + 1, ac.length - 1).str);
	}
}

uint64 FuckMake::FindMatchingParenthesis(const String& string, uint64 start) {
	uint64 count = 0;

	for (uint64 i = start; i < string.length; i++) {
		if (string[i] == '(') {
			count++;
		} else if (string[i] == ')') {
			if (count-- == 1) return i;
		}
	}

	return ~0;
}

Variable* FuckMake::GetVariable(const String& name) {
	for (uint64 i = 0; i < variables.GetCount(); i++) {
		if (variables[i].name == name) return &variables[i];
	}

	return nullptr;
}

Action* FuckMake::GetAction(const String& name) {
	for (uint64 i = 0; i < actions.GetCount(); i++) {
		if (actions[i].name == name) return &actions[i];
	}

	return nullptr;
}

Target* FuckMake::GetTarget(const String& name) {
	if (name == "__default__") {
		if (targets.GetCount() > 0) {
			return &targets[0];
		} else {
			Log(LogLevel::Error, "No targets!");
			exit(1);
		}
	}

	for (uint64 i = 0; i < targets.GetCount(); i++) {
		if (targets[i].name == name) return &targets[i];
	}

	return nullptr;
}