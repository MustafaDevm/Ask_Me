#include "./src/main.h"

struct AskMeSystem {
	UsersManager users_manager;
	QuestionsManager questions_manager;

	void LoadDatabase(bool fill_user_questions = false) {
		users_manager.LoadDatabase();
		questions_manager.LoadDatabase();

		if (fill_user_questions)	
			questions_manager.FillUserQuestions(users_manager.current_user);
	}

	void run() {
		LoadDatabase();
		users_manager.AccessSystem();
		questions_manager.FillUserQuestions(users_manager.current_user);

		vector<string> menu;
		menu.push_back("Print Questions To Me");
		menu.push_back("Print Questions From Me");
		menu.push_back("Answer Question");
		menu.push_back("Delete Question");
		menu.push_back("Ask Question");
		menu.push_back("List System Users");
		menu.push_back("Feed");
		menu.push_back("Logout");

		while (true) {
			int choice = ShowReadMenu(menu);
			LoadDatabase(true);

			if (choice == 1)
				questions_manager.PrintUserToQuestions(users_manager.current_user);
			else if (choice == 2)
				questions_manager.PrintUserFromQuestions(users_manager.current_user);
			else if (choice == 3) {
				questions_manager.AnswerQuestion(users_manager.current_user);
				questions_manager.UpdateDatabase();
			} else if (choice == 4) {
				questions_manager.DeleteQuestion(users_manager.current_user);
				questions_manager.FillUserQuestions(users_manager.current_user);
				questions_manager.UpdateDatabase();
			} else if (choice == 5) {
				pair<int, int> to_user_pair = users_manager.ReadUserId();
				if (to_user_pair.first != -1) {
					questions_manager.AskQuestion(users_manager.current_user, to_user_pair);
					questions_manager.UpdateDatabase();
				}
			} else if (choice == 6)
				users_manager.ListUsersNamesIds();
			else if (choice == 7)
				questions_manager.ListFeed();
			else
				break;
		}
		run();	
	}
};

int main() {
	AskMeSystem service;
	service.run();

	return 0;
}