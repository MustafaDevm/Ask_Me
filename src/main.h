#include <vector>
#include <queue>
#include <set>
#include <map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <assert.h>
using namespace std;

vector<string> ReadFileLines(string path);
void WriteFileLines(string path, vector<string> lines, bool append = true);
vector<string> SplitString(string s, string delimiter = ",");
int ToInt(string str);
int ReadInt(int low, int high);
int ShowReadMenu(vector<string> choices);

struct Question {
	int question_id;
	int parent_question_id;
	int from_user_id;
	int to_user_id;
	int is_anonymous_questions;	
	string question_text;
	string answer_text;			

	Question() {
		question_id = parent_question_id = from_user_id = to_user_id = -1;
		is_anonymous_questions = 1;
	}

	Question(string line) {
		vector<string> substrs = SplitString(line);
		assert(substrs.size() == 7);

		question_id = ToInt(substrs[0]);
		parent_question_id = ToInt(substrs[1]);
		from_user_id = ToInt(substrs[2]);
		to_user_id = ToInt(substrs[3]);
		is_anonymous_questions = ToInt(substrs[4]);
		question_text = substrs[5];
		answer_text = substrs[6];
	}

	string ToString() {
		ostringstream oss;
		oss << question_id << "," << parent_question_id << "," << from_user_id << "," << to_user_id << "," << is_anonymous_questions << "," << question_text << "," << answer_text;

		return oss.str();
	}

	void PrintToQuestion() {
		string prefix = "";

		if (parent_question_id != -1)
			prefix = "\tThread: ";

		cout << prefix << "Question Id (" << question_id << ")";
		if (!is_anonymous_questions)
			cout << " from user id(" << from_user_id << ")";
		cout << "\t Question: " << question_text << "\n";

		if (answer_text != "")
			cout << prefix << "\tAnswer: " << answer_text << "\n";
		cout << "\n";
	}

	void PrintFromQuestion() {
		cout << "Question Id (" << question_id << ")";
		if (!is_anonymous_questions)
			cout << " !AQ";

		cout << " to user id(" << to_user_id << ")";
		cout << "\t Question: " << question_text;

		if (answer_text != "")
			cout << "\tAnswer: " << answer_text << "\n";
		else
			cout << "\tNOT Answered YET\n";
	}

	void PrintFeedQuestion() {
		if (parent_question_id != -1)
			cout << "Thread Parent Question ID (" << parent_question_id << ") ";

		cout << "Question Id (" << question_id << ")";
		if (!is_anonymous_questions)
			cout << " from user id(" << from_user_id << ")";

		cout << " To user id(" << to_user_id << ")";

		cout << "\t Question: " << question_text << "\n";
		if (answer_text != "")
			cout << "\tAnswer: " << answer_text <<"\n";
	}

};

struct User {
	int user_id;		
	string user_name;
	string password;
	string name;
	string email;
	int allow_anonymous_questions;	// 0 or 1

	vector<int> questions_id_from_me;
	map<int, vector<int>> questionid_questionidsThead_to_map;

	User() {
		user_id = allow_anonymous_questions = -1;
	}

	User(string line) {
		vector<string> substrs = SplitString(line);
		assert(substrs.size() == 6);

		user_id = ToInt(substrs[0]);
		user_name = substrs[1];
		password = substrs[2];
		name = substrs[3];
		email = substrs[4];
		allow_anonymous_questions = ToInt(substrs[5]);
	}

	string ToString() {
		ostringstream oss;
		oss << user_id << "," << user_name << "," << password << "," << name << "," << email << "," << allow_anonymous_questions;

		return oss.str();
	}

	void Print() {
		cout << "User " << user_id << ", " << user_name << " " << password << ", " << name << ", " << email << "\n";
	}
};

struct QuestionsManager {
	map<int, vector<int>> questionid_questionidsThead_to_map;

	map<int, Question> questionid_questionobject_map;

	int last_id;

	QuestionsManager() {
		last_id = 0;
	}

	void LoadDatabase() {
		last_id = 0;
		questionid_questionidsThead_to_map.clear();
		questionid_questionobject_map.clear();

		vector<string> lines = ReadFileLines("questions.txt");
		for (auto &line : lines) {       
			Question question(line);
			last_id = max(last_id, question.question_id);

			questionid_questionobject_map[question.question_id] = question;

			if (question.parent_question_id == -1)
				questionid_questionidsThead_to_map[question.question_id].push_back(question.question_id);
			else
				questionid_questionidsThead_to_map[question.parent_question_id].push_back(question.question_id);
		}
	}

	void FillUserQuestions(User &user) {
		user.questions_id_from_me.clear();
		user.questionid_questionidsThead_to_map.clear();

		for (auto &pair : questionid_questionidsThead_to_map) {	
			for (auto &question_id : pair.second) {	

				Question &question = questionid_questionobject_map[question_id];

				if (question.from_user_id == user.user_id)
					user.questions_id_from_me.push_back(question.question_id);

				if (question.to_user_id == user.user_id) {
					if (question.parent_question_id == -1)
						user.questionid_questionidsThead_to_map[question.question_id].push_back(question.question_id);
					else
						user.questionid_questionidsThead_to_map[question.parent_question_id].push_back(question.question_id);
				}
			}
		}
	}

	void PrintUserToQuestions(User &user) {
		cout << "\n";

		if (user.questionid_questionidsThead_to_map.size() == 0)
			cout << "No Questions";

		for (auto &pair : user.questionid_questionidsThead_to_map) {		
			for (auto &question_id : pair.second) {		

				Question &question = questionid_questionobject_map[question_id];
				question.PrintToQuestion();
			}
		}
		cout << "\n";
	}

	void PrintUserFromQuestions(User &user) {
		cout << "\n";
		if (user.questions_id_from_me.size() == 0)
			cout << "No Questions";

		for (auto &question_id : user.questions_id_from_me) {		
			Question &question = questionid_questionobject_map[question_id];
			question.PrintFromQuestion();
		}
		cout << "\n";
	}

	int ReadQuestionIdAny(User &user) {
		int question_id;
		cout << "Enter Question id or -1 to cancel: ";
		cin >> question_id;

		if (question_id == -1)
			return -1;

		if (!questionid_questionobject_map.count(question_id)) {
			cout << "\nERROR: No question with such ID. Try again\n\n";
			return ReadQuestionIdAny(user);
		}
		Question &question = questionid_questionobject_map[question_id];

		if (question.to_user_id != user.user_id) {
			cout << "\nERROR: Invalid question ID. Try again\n\n";
			return ReadQuestionIdAny(user);
		}
		return question_id;
	}

	int ReadQuestionIdThread(User &user) {
		int question_id;
		cout << "For thread question: Enter Question id or -1 for new question: ";
		cin >> question_id;

		if (question_id == -1)
			return -1;

		if (!questionid_questionidsThead_to_map.count(question_id)) {
			cout << "No thread question with such ID. Try again\n";
			return ReadQuestionIdThread(user);
		}
		return question_id;
	}

	void AnswerQuestion(User &user) {
		int question_id = ReadQuestionIdAny(user);

		if (question_id == -1)
			return;

		Question &question = questionid_questionobject_map[question_id];

		question.PrintToQuestion();

		if (question.answer_text != "")
			cout << "\nWarning: Already answered. Answer will be updated\n";

		cout << "Enter answer: ";	
		getline(cin, question.answer_text);
		getline(cin, question.answer_text);
	}

	void DeleteQuestion(User &user) {
		int question_id = ReadQuestionIdAny(user);

		if (question_id == -1)
			return;

		vector<int> ids_to_remove;	

		if (questionid_questionidsThead_to_map.count(question_id)) { 
			ids_to_remove = questionid_questionidsThead_to_map[question_id];
			questionid_questionidsThead_to_map.erase(question_id);
		} else {
			ids_to_remove.push_back(question_id);

			for (auto &pair : questionid_questionidsThead_to_map) {
				vector<int> &vec = pair.second;
				for (int pos = 0; pos < (int) vec.size(); ++pos) {
					if (question_id == vec[pos]) {
						vec.erase(vec.begin() + pos);
						break;
					}
				}
			}

		}

		for (auto id : ids_to_remove) {
			questionid_questionobject_map.erase(id);
		}
	}

	void AskQuestion(User &user, pair<int, int> to_user_pair) {
		Question question;

		if (!to_user_pair.second) {
			cout << "Note: Anonymous questions are not allowed for this user\n";
			question.is_anonymous_questions = 0;
		} else {
			cout << "Is anonymous questions?: (0 or 1): ";
			cin >> question.is_anonymous_questions;
		}

		question.parent_question_id = ReadQuestionIdThread(user);

		cout << "Enter question text: ";	
		getline(cin, question.question_text);
		getline(cin, question.question_text);

		question.from_user_id = user.user_id;
		question.to_user_id = to_user_pair.first;


		question.question_id = ++last_id;

		questionid_questionobject_map[question.question_id] = question;

		if (question.parent_question_id == -1)
			questionid_questionidsThead_to_map[question.question_id].push_back(question.question_id);
		else
			questionid_questionidsThead_to_map[question.parent_question_id].push_back(question.question_id);
	}

	void ListFeed() {
		for (auto &pair : questionid_questionobject_map) {
			Question &question = pair.second;

			if (question.answer_text == "")
				continue;

			question.PrintFeedQuestion();
		}
	}

	void UpdateDatabase() {
		vector<string> lines;

		for (auto &pair : questionid_questionobject_map)
			lines.push_back(pair.second.ToString());

		WriteFileLines("questions.txt", lines, false);
	}
};

struct UsersManager {
	map<string, User> userame_userobject_map;
	User current_user;
	int last_id;

	UsersManager() {
		last_id = 0;
	}

	void LoadDatabase() {
		last_id = 0;
		userame_userobject_map.clear();

		vector<string> lines = ReadFileLines("users.txt");
		for (auto &line : lines) {
			User user(line);
			userame_userobject_map[user.user_name] = user;
			last_id = max(last_id, user.user_id);
		}
	}

	void AccessSystem() {
		int choice = ShowReadMenu( { "Login", "Sign Up" });
		if (choice == 1)
			DoLogin();
		else
			DoSignUp();
	}

	void DoLogin() {
		LoadDatabase();

		while (true) {
			cout << "Enter user name & password: ";
			cin >> current_user.user_name >> current_user.password;

			if (!userame_userobject_map.count(current_user.user_name)) {
				cout << "\nInvalid user name or password. Try again\n\n";
				continue;
			}
			User user_exist = userame_userobject_map[current_user.user_name];

			if (current_user.password != user_exist.password) {
				cout << "\nInvalid user name or password. Try again\n\n";
				continue;
			}
			current_user = user_exist;
			break;
		}
	}

	void DoSignUp() {
		while (true) {
			cout << "Enter user name. (No spaces): ";
			cin >> current_user.user_name;

			if (userame_userobject_map.count(current_user.user_name))
				cout << "Already used. Try again\n";
			else
				break;
		}
		cout << "Enter password: ";
		cin >> current_user.password;

		cout << "Enter name: ";
		cin >> current_user.name;

		cout << "Enter email: ";
		cin >> current_user.email;

		cout << "Allow anonymous questions? (0 or 1): ";
		cin >> current_user.allow_anonymous_questions;


		current_user.user_id = ++last_id;
		userame_userobject_map[current_user.user_name] = current_user;

		UpdateDatabase(current_user);
	}

	void ListUsersNamesIds() {
		for (auto &pair : userame_userobject_map)
			cout << "ID: " << pair.second.user_id << "\t\tName: " << pair.second.name << "\n";
	}

	pair<int, int> ReadUserId() {
		int user_id;
		cout << "Enter User id or -1 to cancel: ";
		cin >> user_id;

		if (user_id == -1)
			return make_pair(-1, -1);

		for (auto &pair : userame_userobject_map) {
			if (pair.second.user_id == user_id)
				return make_pair(user_id, pair.second.allow_anonymous_questions);
		}

		cout << "Invalid User ID. Try again\n";
		return ReadUserId();
	}

	void UpdateDatabase(User &user) {
		string line = user.ToString();
		vector<string> lines(1, line);
		WriteFileLines("users.txt", lines);
	}
};