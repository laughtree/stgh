#include <array>
#include <cstdio>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <string>

#define VERSION "0.1.0"

std::string executeCommand(std::string command) {
  std::array<char, 128> buffer;
  std::string res;

  std::shared_ptr<FILE> pipe(popen(command.c_str(), "r"), pclose);
  if (!pipe) {
    throw std::runtime_error("Failed: popen execution failed.");
  }
  while (!feof(pipe.get())) {
    if (fgets(buffer.data(), 128, pipe.get()) != nullptr) {
      res += buffer.data();
    }
  }

  return res;
}

bool Question(std::string question) {
  std::string answer;
  std::cout << question << " [y/N]: ";
  std::getline(std::cin, answer);
  if (answer == "y" || answer == "Y") {
    return true;
  } else if (answer == "n" || answer == "N" || answer.empty()) {
    return false;
  } else {
    std::cout << "Please answer with 'y' or 'n'." << std::endl;
    return Question(question);
  }
}

void Help() {
  std::cout << "Unfortunatly, help is not available now." << std::endl;
  return;
}

void Push() {
  std::string result, gitusrname, remoteorigin;
  gitusrname = executeCommand("git config user.name");
  if (gitusrname.empty()) {
    std::cerr << "Error: No username found in git config." << std::endl;
    return;
  }
  gitusrname.pop_back(); // Remove \n
  remoteorigin = executeCommand("git remote get-url origin");
  std::string repoOwner = remoteorigin.substr(0, remoteorigin.rfind("/"));
  repoOwner = repoOwner.substr(repoOwner.rfind("/") + 1, repoOwner.size());
  std::cout << "Remote origin repository owner: " << repoOwner << std::endl;
  if (gitusrname != repoOwner) {
    std::cout
        << "Warning: The remote origin repository is probably not owned by you."
        << std::endl;
    std::cout << "Your username is \n"
              << gitusrname << "\nThe remote origin url is \n"
              << remoteorigin << std::endl;
    if (!Question("Are you sure you want to push?")) {
      std::cout << "Push Cancelled" << std::endl;
      return;
    }
  }
  result = executeCommand("git push");
  std::cout << result << std::endl;
  return;
}

void NewBranch(const std::string& branchName) {
    std::string command = "git checkout -b " + branchName;
    std::string result = executeCommand(command);
    std::cout << result << std::endl;
}

void Resume(const std::string& branchName) {
    std::string command = "git checkout -b " + branchName + " HEAD~1";
    std::string result = executeCommand(command);
    std::cout << result << std::endl;
}

std::map<std::string, std::function<void()>> commands = {{"push", Push},
                                                         {"help", Help}};

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: stgh <command> [args]" << std::endl
              << "If you need help, please use 'stgh help'" << std::endl;
    return 1;
  }
  std::string command = argv[1];

  if (command == "newbranch" || command == "resume") {
      if (argc < 3) {
          std::cerr << "Usage: stgh " << command << " <branch_name>" << std::endl;
          return 1;
      }
      std::string branchName = argv[2];
      if (command == "newbranch") {
          NewBranch(branchName);
      } else {
          Resume(branchName);
      }
  }
  else if (commands.find(command) == commands.end()) {
    std::cerr << "Command not found: " << command << std::endl
              << "If you need help, please use 'stgh help'" << std::endl;
    return 1;
  }
  else {
    commands[command]();
  }
  return 0;
}