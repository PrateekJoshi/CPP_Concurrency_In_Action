/*
 * demo.cc
 *
 *  Created on: 05-Feb-2021
 *      Author: prateek
 *      Pg 22
The thread handling the request isn’t going to care about waiting
for that other thread to finish, because it’s working on an unrelated document, so this
makes it a prime candidate for running a detached thread.

If the user chooses to open a new document, you prompt them for the document to
open, start a new thread to open that document B, and then detach it c. Because
the new thread is doing the same operation as the current thread but on a different
file, you can reuse the same function (edit_document) with the newly chosen file-
name as the supplied argument.
This example also shows a case where it’s helpful to pass arguments to the function
used to start a thread: rather than just passing the name of the function to the
std::thread constructor B, you also pass in the filename parameter.
 *
 */
#include <thread>
#include <string>

void open_document_and_display_gui(std::string const &filename) {}

bool done_editing(){ return true; }

enum command_type{
	open_new_document
};

struct user_command{
	command_type type;

	user_command(): type(open_new_document){}
};

user_command get_user_input(){
	return user_command();
}

std::string get_filename_from_user()
{
	return "foo.doc";
}

void process_user_input(user_command const &cmd){}

void edit_document(std::string const &filename)
{
	open_document_and_display_gui(filename);
	while(!done_editing())
	{
		user_command cmd = get_user_input();

		if( cmd.type == open_new_document )
		{
			std::string const new_name = get_filename_from_user();

			std::thread t(edit_document, new_name);
			t.detach();
		}
		else
		{
			process_user_input(cmd);
		}
	}
}

int main()
{
	edit_document("prateek.doc");
}






