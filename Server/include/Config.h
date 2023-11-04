#pragma once

#define CURRENT_ID_AND_BUTTON ids[selected_id].profiles[0].buttons[index] // TODO: Change this to actual profile and page
#define CURRENT_ID			  ids[selected_id]

#include <filesystem>
#include <fstream>
#include <vector>
#include <string>

#include "../../json.hpp"

using std::string;
using std::vector;
using std::ifstream;
using std::ofstream;
using std::istreambuf_iterator;

using std::filesystem::exists;
using std::filesystem::remove;
using std::filesystem::create_directory;

class button {
public:
	string label = "";
	string label_backup = label;
	bool   default_label = true;
	enum   types { File, URL, Command, };
	types  type = File;
	string action;
};

class profile {
public:
	int	   columns = 6;
	int	   rows = 4;
	int	   current_page = 0;
	vector<button> buttons;
};

class id {
public:
	bool reconfigure  = false;
	bool has_commsock = false;
	bool has_confsock = false;
	int belongs_to_comm_thread_no;
	int belongs_to_conf_thread_no;
	string ID;
	vector<profile> profiles;
	id(string in_ID);
	bool operator==(const id& other) const;
};

extern vector<id> ids;

using json = nlohmann::ordered_json;

extern void clear_button(int profile, int page, int button);
extern void configure_id(id* id);
extern void write_config(vector<string> args, size_t arg_size);
