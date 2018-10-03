#include <gtest/gtest.h>
#include <stdlib.h>
#include <fcntl.h>

using namespace std;

// note: to execute the test, you have to make sure everything is in the right directory

// shell to run tests on
#define SHELL "/home/bart/shell/shell -t 0"
//#define SHELL "/bin/sh"

// declarations of methods you want to test (should match exactly)
std::vector<std::string> parseArguments(const std::string& str, char seperator);
std::string removeSpaces(std::string input);
bool validSplit(std::vector<string> splitcmd);

namespace {

void Execute(std::string command, std::string expectedOutput);
void Execute(std::string command, std::string expectedOutput, std::string expectedOutputFile, std::string expectedOutputFileContent);

TEST(Shell, removeSpaces) {
    std::string expected;

    expected = "<";
    EXPECT_EQ(expected, removeSpaces("<"));
    EXPECT_EQ(expected, removeSpaces(" <"));
    EXPECT_EQ(expected, removeSpaces("< "));
    EXPECT_EQ(expected, removeSpaces("  <"));
    EXPECT_EQ(expected, removeSpaces("<  "));
    EXPECT_EQ(expected, removeSpaces(" <  "));
    EXPECT_EQ(expected, removeSpaces("  < "));
    EXPECT_EQ(expected, removeSpaces("      <      "));

    expected = "<>";
    EXPECT_EQ(expected, removeSpaces("<>"));
    EXPECT_EQ(expected, removeSpaces(" <>"));
    EXPECT_EQ(expected, removeSpaces("<> "));
    EXPECT_EQ(expected, removeSpaces("  < >"));
    EXPECT_EQ(expected, removeSpaces("< >  "));
    EXPECT_EQ(expected, removeSpaces(" <  >  "));
    EXPECT_EQ(expected, removeSpaces("  <     >"));
    EXPECT_EQ(expected, removeSpaces("      <            > "));
}

TEST(Shell, validSplit) {
    bool expected;

    expected = false;

    vector<string> input = {""};
    EXPECT_EQ(expected, validSplit(input));
    input = {" "};
    EXPECT_EQ(expected, validSplit(input));
    input = {" ", " "};
    EXPECT_EQ(expected, validSplit(input));
    input = {"   "};
    EXPECT_EQ(expected, validSplit(input));
    input = {"   ", " "};
    EXPECT_EQ(expected, validSplit(input));
    input = {" ", "ls"};
    EXPECT_EQ(expected, validSplit(input));
    input = {" ls", "", "boxes"};
    EXPECT_EQ(expected, validSplit(input));
    input = {"ls", " ", " boxes "};
    EXPECT_EQ(expected, validSplit(input));

    expected = true;

    input = {"ls", " head -3 ", "cowsay", "  figlet   ", "  boxes >   o  "};
    EXPECT_EQ(expected, validSplit(input));

}

TEST(Shell, ParseArguments) {
    std::vector<std::string> expected;

    expected = {};
    EXPECT_EQ(expected, parseArguments("", ' '));
    EXPECT_EQ(expected, parseArguments(" ", ' '));
    EXPECT_EQ(expected, parseArguments("  ", ' '));

    expected = {"foo"};
    EXPECT_EQ(expected, parseArguments("foo", ' '));
    EXPECT_EQ(expected, parseArguments(" foo", ' '));
    EXPECT_EQ(expected, parseArguments("foo ", ' '));
    EXPECT_EQ(expected, parseArguments(" foo ", ' '));
    EXPECT_EQ(expected, parseArguments("  foo  ", ' '));

    expected = {"foo", "bar"};
    EXPECT_EQ(expected, parseArguments("foo bar", ' '));
    EXPECT_EQ(expected, parseArguments(" foo  bar", ' '));
    EXPECT_EQ(expected, parseArguments("  foo   bar  ", ' '));

    expected = {"cmd1", "arg1", "<", "inputfile", "|", "cmd2", "arg2", ">", "outputfile"};
    EXPECT_EQ(expected, parseArguments("cmd1 arg1 < inputfile | cmd2 arg2 > outputfile", ' '));

    expected = {"cmd1 arg1 < inputfile ", "boxes", " sort -R ", "cmd2 arg2 > outputfile"};
    EXPECT_EQ(expected, parseArguments("cmd1 arg1 < inputfile |boxes| sort -R |cmd2 arg2 > outputfile", '|'));
}

TEST(Shell, ReadFromFile) {
    Execute("cat < 1", "line 1\nline 2\nline 3\nline 4");
    Execute("cat<1", "line 1\nline 2\nline 3\nline 4");
    Execute("  cat  <  1  ", "line 1\nline 2\nline 3\nline 4");
}

TEST(Shell, ReadFromAndWriteToFile) {
    Execute("cat < 1 > ../foobar", "", "../foobar", "line 1\nline 2\nline 3\nline 4");
    Execute("cat > ../foobar  < 1 ", "", "../foobar", "line 1\nline 2\nline 3\nline 4");
}

TEST(Shell, ReadFromAndWriteToFileChained) {
    Execute("cat < 1 | head -n 3 > ../foobar", "", "../foobar", "line 1\nline 2\nline 3\n");
    Execute("cat < 1 | head -n 3 | tail -n 1 > ../foobar", "", "../foobar", "line 3\n");
}

TEST(Shell, WriteToFile) {
    Execute("ls -1 > ../foobar", "", "../foobar", "1\n2\n3\n4\n");
}

TEST(Shell, Execute) {
    Execute("uname", "Linux\n");
    Execute("ls", "1\n2\n3\n4\n");
    Execute("ls -1", "1\n2\n3\n4\n");
}

TEST(Shell, ExecuteChained) {
    Execute("ls -1 | head -n 2", "1\n2\n");
    Execute("ls -1 | head -n 2 | tail -n 1", "2\n");
}

TEST(Shell, DefinedInvalidInput) {
    Execute("|", "");
    Execute("ls | ", "");
    Execute(" | ls | ", "");
    Execute(" ", "");
    Execute("", "");
}


//////////////// HELPERS

std::string filecontents(const std::string& str) {
  std::string retval;
  int fd = open(str.c_str(), O_RDONLY);
  struct stat st;
  if (fd >= 0 && fstat(fd, &st) == 0) {
    long long size = st.st_size;
    retval.resize(size);
    char *current = (char*)retval.c_str();
    ssize_t left = size;
    while (left > 0) {
      ssize_t bytes = read(fd, current, left);
      if (bytes == 0 || (bytes < 0 && errno != EINTR))
        break;
      if (bytes > 0) {
        current += bytes;
        left -= bytes;
      }
    }
  }
  if (fd >= 0)
    close(fd);
  return retval;
}

void filewrite(const std::string& str, std::string content) {
  int fd = open(str.c_str(), O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);
  if (fd < 0)
    return;
  while (content.size() > 0) {
    int written = write(fd, content.c_str(), content.size());
    if (written == -1 && errno != EINTR) {
      std::cout << "error writing file '" << str << "': error " << errno << std::endl;
      break;
    }
    content = content.substr(written);
  }
  close(fd);
}

void Execute(std::string command, std::string expectedOutput) {
    filewrite("input", command);
    system("cd /home/bart/shell/test-dir; " SHELL " < /home/bart/shell/input > /home/bart/shell/output 2> /dev/null");
    std::string got = filecontents("output");
    EXPECT_EQ(expectedOutput, got);
}

void Execute(std::string command, std::string expectedOutput, std::string expectedOutputFile, std::string expectedOutputFileContent) {
    std::string expectedOutputLocation = "/home/bart/shell/test-dir/" + expectedOutputFile;
    unlink(expectedOutputLocation.c_str());
    filewrite("/home/bart/shell/input", command);
    int rc = system("cd /home/bart/shell/test-dir; " SHELL " < /home/bart/shell/input > /home/bart/shell/output 2> /dev/null");
    EXPECT_EQ(0, rc);
    std::string got = filecontents("output");
    EXPECT_EQ(expectedOutput, got) << command;
    std::string gotOutputFileContents = filecontents(expectedOutputLocation);
    EXPECT_EQ(expectedOutputFileContent, gotOutputFileContents) << command;
    unlink(expectedOutputLocation.c_str());
}

}
