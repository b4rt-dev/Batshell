/**
  * BatShell 
  *     by Bart Pleiter S4752740
  * Operating Systems
  */

// note1: This shell works best with a monospace Powerline font
// note2: Has to be compiled with C++11 or later

// function/class definitions
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/param.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <pwd.h>
#include <signal.h>
#include <vector>
#include <fcntl.h>
#include <algorithm>
#include <fstream>

// increase readability for pipes
#define READ  0
#define WRITE 1

// for not having to use std::
using namespace std;

// prints the logo of this shell
void showLogo()
{
printf(R"EOF(
                       /\
     .                // \       .
   //|   _____  _____//_ |       |\\
  //  \  \_  \\/  _____ \/      /  \\
 //   .\_  \__|  / V V \ |     /    \\
 ||   || \_   \  \/\_/\/ /   _/||    ||
 ||   ||   \__/          \__/  ||    ||
 ||    \\     |          |     //    ||
 ||     ||   _| BatShell |_   ||     || 
 ||     || _/ \          / \_ ||     ||
 ||____ ||/    \        /    \|| ____||
 |/    \/       '-----.'       \/    \|
                  |    \
                  |     \
                  |      \
                  '|      \
                   |       \
                   |        '

)EOF");
}

// parses a string to form a vector split by a seperator 
vector<string> parseArguments(const string& input, char seperator) 
{
    // create output vector
    vector<string> output;
    for (int pos = 0; pos < input.length(); ) 
    {
        // look for the next space
        int found = input.find(seperator, pos);
        // if no seperator was found, this is the last word
        if (found == string::npos) 
        {
            output.push_back(input.substr(pos));
            break;
        }
        // filter out consequetive seperators
        if (found != pos)
        {
            output.push_back(input.substr(pos, found-pos));
        }
        pos = found+1;
    }
    return output;
}

// removes all spaces in a string
string removeSpaces(string input)
{
    input.erase(remove(input.begin(),input.end(),' '),input.end());
    return input;
}

// redirects stdout to a file at 'path'
// creates new file if it does not exist
// if exists, then clears the file
int redirectStdout(string path)
{
    // open the file
    // 0644 means that the user can read/write, group and others can read only
    int file = open(path.c_str(), O_WRONLY | O_TRUNC | O_CREAT, 0644);
    // redirect stdout
    int retval = dup2(file, STDOUT_FILENO);
    close(file);
    return retval;
}

// redirects stdin from a file at 'path'
int redirectStdin(string path)
{
    // open the file
    int file = open(path.c_str(), O_RDONLY, 0);
    // redirect stdin
    int retval = dup2(file, STDIN_FILENO);
    close(file);
    return retval;
}

// detects if there is input redirection in the first command
// removes the redirection from the command and sets redInput and ifname variables
void parseInputRed(vector<string>& splitcmd, bool& redInput, string& ifname)
{
    // check for '<' and a single argument after it
    vector<string> smallSplit = parseArguments(splitcmd[0], '<');
    if (smallSplit.size() == 2)
    {   
        // check if there was a command before '<'
        if (smallSplit[0].find_first_not_of(' ') != string::npos)
        {
            // check if there is also a '>'. 
            // if so, remove it and what is after it
            // and put it back in the command
            vector<string> smallBigSplit = parseArguments(smallSplit[1], '>');
            if (smallBigSplit.size() == 2)
            {
                smallSplit[1] = smallBigSplit[0];
                smallSplit[0] += string(">") + smallBigSplit[1];
            }
            // enable input redirection
            redInput = true;
            ifname = removeSpaces(smallSplit[1]);
            splitcmd[0] = smallSplit[0];
        }
    }
}

// detects if there is output redirection in the last command
// removes the redirection from the command and sets redOutput and ofname variables
void parseOutputRed(vector<string>& splitcmd, bool& redOutput, string& ofname)
{
    // check for '>' and a single argument after it
    vector<string> bigSplit = parseArguments(splitcmd.back(), '>');
    if (bigSplit.size() == 2)
    {   
        // check if there was a command before '>'
        if (bigSplit[0].find_first_not_of(' ') != string::npos)
        {
            // check if there is also a '<'. 
            // if so, remove it and what is after it
            vector<string> bigSmallSplit = parseArguments(bigSplit[1], '<');
            if (bigSmallSplit.size() == 2)
            {
                bigSplit[1] = bigSmallSplit[0];
            }
            // enable output redirection
            redOutput = true;
            ofname = removeSpaces(bigSplit[1]);
            splitcmd.back() = bigSplit[0];
        }
    }
}

// parses input from splitcmd (a vector with the commands split by '|')
// and fills cmds with the parsed result
void parsePiped(vector<vector<string>>& cmds, vector<string>& splitcmd, bool& redInput, bool& redOutput, string& ifname, string& ofname)
{
    // detect the input/output redirection
    // (the order of these two functions matters)
    parseInputRed (splitcmd, redInput , ifname);
    parseOutputRed(splitcmd, redOutput, ofname);

    // loop over all chained commands
    for (int i = 0; i < splitcmd.size(); ++i) 
    {
        // split the command into a vector of arguments by spaces
        vector<string> args = parseArguments(splitcmd[i], ' ');

        // add colors to ls
        if (args[0] == "ls")
            args.insert(args.begin() + 1, "--color=auto");

        // add the argument list to the array
        cmds.push_back(args);
    }
}

// returns c_args from cmds at position n
char** getCargList(vector<vector<string>>& cmds, int n)
{
    // create empty c_args
    char** c_args = new char*[cmds[n].size() + 1];

    // add all args from cmds at position n to c_args
    for (int i = 0; i < cmds[n].size(); ++i)
    {
        c_args[i] = strdup(cmds[n][i].c_str());
    }
    // always end with a NULL
    c_args[cmds[n].size()] = NULL;

    return c_args;
}

// execute all the commands from cmds with
// background, redInput and redOutput as options
int executeParsed(bool& background, vector<vector<string>>& cmds, bool& redInput, bool& redOutput, string& ifname, string& ofname)
{
    // to store all the child pid's
    vector<pid_t> basement;

    // list to store file descriptors in to close them later
    vector<int> fdCloseList;

    // create the file descriptor container for the pipe
    int fds[2];

    // stdint is used as input for all the commands.
    // it should start with the original input 
    int stdint = STDIN_FILENO;

    // disable stdin if it is a background process 
    // and there is no input redirection
    if (background && !redInput)
        close(STDIN_FILENO);

    // for all commands but the last one
    for(int i = 0; i < (cmds.size() - 1); ++i)
    {
        // create a pipe
        pipe(fds);

        // create a child process and store its pid
        basement.push_back(fork());
        if (basement.back() == -1)
        {
            fprintf(stderr, "%s\n", "unable to create a child process");
            close(fds[WRITE]);
            close(fds[READ]);
            return -1;
        }
        if (basement.back() == 0)
        {   // child process:

            // if it is the first command and
            // input redirection is enabled
            if (i == 0 && redInput)
            {
                // get input from ifname
                redirectStdin(ifname);
            }
            else
            {
                // set the output of the previous child as input
                dup2 (stdint, STDIN_FILENO);
                close (stdint);
            }   

            // put the output in the pipe
            dup2 (fds[WRITE], STDOUT_FILENO);
            close (fds[WRITE]);
          
            // get c_args for the command
            char** c_args = getCargList(cmds,i);

            // execute te command
            execvp(c_args[0], c_args);

            // if we got this far, 
            // free memory, close pipe and return error
            fprintf(stderr, "%s: error %i (%s)\n",  c_args[0], errno, strerror(errno));

            close(fds[READ]);
            close(stdint);

            // also close this to keep valgrind quiet
            close(STDIN_FILENO);
            close(STDOUT_FILENO);

            for (int j = 0; j < cmds[i].size(); ++j)
                free(c_args[j]);

            delete[] c_args;

            return errno;
        }

        // close the write end of pipe
        // but keep the read end, as it is used in the next command
        close (fds[WRITE]);

        // store the read end of the pipe (with the childs output) 
        // for the next child
        stdint = fds[READ];

        // but because the read end has to be closed some time
        // in the parent, we have to store it
        fdCloseList.push_back(stdint);
    }  

    // close all stored read ends of the pipes
    // but the last one, since that one is used for the last command
    if (!fdCloseList.empty())
    {
        for ( int i = 0; i < fdCloseList.size() -1; i++)
            close(fdCloseList[i]);
    }

    // create the last child process and store its pid
    basement.push_back(fork());
    if (basement.back() == -1)
        {
            fprintf(stderr, "%s\n", "unable to create a child process");
            close(fds[READ]);
            return -1;
        }
    if (basement.back() == 0)
    {   // child process:

        // if input redirection is enabled and
        // it is the first command
        if (redInput && cmds.size() == 1)
        {
            // get input from ifname
            redirectStdin(ifname);
        }
        else
        {
            // set the output of the previous child as input
            dup2(stdint, STDIN_FILENO);
            if (stdint != STDIN_FILENO)
                close(stdint);
        }
        
        if (redOutput)
        {
            redirectStdout(ofname);
        }
        
        // get c_args for the command
        char** c_args = getCargList(cmds,cmds.size()-1);

        // execute te command
        execvp(c_args[0], c_args);

        // if we got this far, 
        // free memory, close pipe (if any) and return error
        fprintf(stderr, "%s: error %i (%s)\n",  c_args[0], errno, strerror(errno));
        
        
        for (int i = 0; i < cmds[cmds.size()-1].size(); ++i)
            free(c_args[i]);

        // since c_args is an array, we have to call delete[] and NOT delete
        delete[] c_args;

        // also close this to keep valgrind quiet
        close(STDIN_FILENO);
        close(STDOUT_FILENO);

        // close all file descriptors if there was a pipe
        if (cmds.size() > 1)
        {
            close(fds[READ]);
            close(stdint);
        }
        
        return errno;
    } 

    if (stdint != STDIN_FILENO)
    {
        close(stdint);
    }

    // wait for all the children to finish
    for (int i = 0; i < basement.size(); ++i)
    {
        waitpid(basement[i], NULL, 0);
    }

    return 0;
}

// returns true only if there is at least one command
// and all commands must consist out of more than only spaces
bool validSplit(vector<string> splitcmd)
{
    if (splitcmd.size() == 0)
        return false;
    for (int i = 0; i < splitcmd.size(); i++)
    {
        if (removeSpaces(splitcmd[i]) == "")
            return false;
    }
    return true;
}

// filter, split and execute the user input
int executeInput(const string& str, bool background)
{   
    // define redirection variables
    bool redInput = false;
    bool redOutput = false;
    string ifname;
    string ofname;

    // split the user commands by '|'
    vector<string> splitcmd = parseArguments(str, '|');

    // if the split commands are valid enough to parse
    if (validSplit(splitcmd))
    {
        // create the vector of argument lists
        vector<vector<string>> cmds;

        // parse the input
        parsePiped(cmds, splitcmd, redInput, redOutput, ifname, ofname);

        // execute the parsed input
        return executeParsed(background, cmds, redInput, redOutput, ifname, ofname);
    }
    else
    {
        fprintf(stderr, "%s\n", "syntax error");
        return 0;
    }
}

// change to specified directory
void changeDir(const vector<string>& args)
{
    // if no target directory was specified
    if (args.size() == 1)
    {
        // get home directory and change to it
        struct passwd *pw = getpwuid(getuid());
        const char *homedir = pw->pw_dir;
        if (chdir(homedir) != 0)
        {
            fprintf(stderr, "error %i (%s) while going to home directory\n", errno, strerror(errno));
        }
    }
    else
    {
        if (chdir(args[1].c_str()) != 0)
        {
            fprintf(stderr, "error %i (%s) while going to %s\n", errno, strerror(errno), args[1].c_str());
        }
    }
}

// print the prompt to screen
void displayPrompt() 
{
    // get current working directory
    char buffer[512];
    char* dir = getcwd(buffer, sizeof(buffer));
    
    // define colorscheme
    const char* c_clear = "\e[0m";
    const char* c_name  = "\e[44m\e[97m";
    const char* c_host  = "\e[42m\e[97m";
    const char* c_home  = "\e[47m\e[30m";
    const char* c_dir   = "\e[100m\e[97m";
    const char* c_money= "\e[30m\e[47m";

    // colors for the powerline characters
    const char* c_1 = "\e[34m\e[42m";
    const char* c_2 = "\e[32m\e[47m";
    const char* c_3 = "\e[100m\e[37m";
    const char* c_4 = "\e[90m\e[47m";
    const char* c_5 = "\e[37m";
    const char* c_6 = "\e[32m\e[100m";

    // path starts with the users home directory
    bool atHome = true;

    // if dir is not empty
    if (dir)
    {
        // get home directory and username
        struct passwd *pw = getpwuid(getuid());
        string homedir = pw->pw_dir;
        string username = pw->pw_name;

        // check if path starts with home directory
        for (int i = 0; i < homedir.size() -1; ++i)
        {
            if (homedir[i] != dir[i])
            {
                atHome = false;
            }
        }
        
        //printf("homedirsize: %d\n", homedir.size());

        // dir without home directory
        string homeless;

        // create string version of dir for substring function
        string dirstr = dir;

        // get the hostname
        char hostname[512];
        gethostname(hostname, sizeof(hostname));

        // if path starts with home directory, 
        // then give the home directory a special color
        // and because of sytle reasons, do not display a '~'
        if (atHome)
        {
            homeless = dirstr.substr(homedir.size());
            //strcpy(homeless,dir + homedir.size() + 3);
            cout 
            << c_name << username << c_clear
            << c_1    << ""      << c_clear
            << c_host << hostname << c_clear
            << c_2    << ""      << c_clear
            << c_home << homedir  << c_clear
            << c_3    << ""      << c_clear
            << c_dir  << homeless << c_clear
            << c_4    << ""      << c_clear;
        }
        else
        {
            cout 
            << c_name << username << c_clear
            << c_1    << ""      << c_clear
            << c_host << hostname << c_clear
            << c_6    << ""      << c_clear
            << c_dir << dir      << c_clear
            << c_4    << ""      << c_clear;
        }
    }
    
    // give the super user a 'super' character
    if (getuid() == 0)
    {
        cout << c_money << "Ω" << c_clear
        << c_5      << " " << c_clear;

    }
    else
    {
        cout << c_money << "Σ" << c_clear
        << c_5     << " " << c_clear;

    }

    // flush cout to screen
    flush(cout);
}

// ^C handler
// go to a new line
// and show a (new) prompt
void newLine(int signal)
{
    cout << "\n";
    flush(cout);
    displayPrompt();
}

// get the input of the user as a string
string requestCommandLine(bool showPrompt) 
{
    if (showPrompt)
    {
        displayPrompt();
    }
    string retval;
    getline(cin, retval);
    return retval;
}

// the main shell function
int shell(bool showPrompt) 
{
    // no prompt -> no zubat! (needed for unit tests)
    if (showPrompt)
        showLogo();

    // remap ^C (and other interrupt signals) to a new line
    signal(SIGINT,newLine);

    // keep looping untill the user wants to exit
    bool iWantToExit = false;
    while (!iWantToExit && cin.good())
    {
        // user input
        string commandLine = requestCommandLine(showPrompt);

        ofstream outfile;
        outfile.open("~/.bathistory", ios_base::app);
        outfile << commandLine + '\n';

        if (commandLine == "exit")
        {
            iWantToExit = true;
        }
        else if (commandLine == "help")
        {
            // do not help the user
            printf("%s\n", "You called for help...");
            sleep(1);
            printf("%s\n", "...");
            sleep(1);
            printf("%s\n", "...");
            sleep(1);
            printf("%s\n", "...");
            sleep(1);
            printf("%s\n", "But there was no response.");
        }
        else
        {
            // set return value
            int rc = 0;

            // split by ' ' to determine easier what the user wants to do
            vector<string> args = parseArguments(commandLine, ' ');

            // if there is no command
            if (args.size() == 0)
            {
                // do nothing
            }
            else if (args[0] == "cd")
            {
                // change working directory
                changeDir(args);
            }
            else
            {   
                // check if the user wants to execute in background
                if (commandLine.back() == '&')
                {
                    // create background process and execute in background.
                    // if done executing, close the background process
                    int pid = fork();
                    if (pid == -1)
                    {
                        fprintf(stderr, "%s\n", "unable to create a child process");
                        rc = -1;
                    }
                    if (pid == 0)
                    {
                        // this works since C++11
                        commandLine.pop_back();
                        exit(executeInput(commandLine, true));
                    }
                    // do not wait for the child to finish
                }
                else
                {
                    // execute the user input normally
                    rc = executeInput(commandLine, false);
                }
            }

            // exit if there is a problem 
            if (rc != 0)
            {
                exit(rc);
            }
        }
    }
    return 0;
}


