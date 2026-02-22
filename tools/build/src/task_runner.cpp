// بسم الله الرحمن الرحيم
/**
 * @file task_runner.cpp
 * @brief Task Runner Implementation
 */

#include "../include/task_runner.h"
#include <iostream>
#include <chrono>
#include <cstdlib>
#include <regex>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/wait.h>
#endif

namespace sad {
namespace build {

std::string Task::getFullCommand() const {
    std::string full_cmd = command;
    for (const auto& arg : args) {
        full_cmd += " " + arg;
    }
    return full_cmd;
}

bool Task::isValid() const {
    return !name.empty() && !command.empty();
}

TaskRunner::TaskRunner() {
}

TaskRunner::~TaskRunner() {
}

bool TaskRunner::registerTask(const Task& task) {
    if (!task.isValid()) {
        return false;
    }

    tasks_[task.name] = task;
    return true;
}

int TaskRunner::registerTasks(const std::map<std::string, Task>& tasks) {
    int count = 0;
    for (const auto& [name, task] : tasks) {
        if (registerTask(task)) {
            count++;
        }
    }
    return count;
}

TaskResult TaskRunner::run(const std::string& task_name, 
                          const TaskExecutionOptions& options) {
    // Resolve dependencies
    std::vector<std::string> execution_order = resolveDependencies(task_name);
    
    if (execution_order.empty()) {
        TaskResult result;
        result.success = false;
        result.error = "Circular dependency detected or task not found";
        result.task_name = task_name;
        return result;
    }

    // Execute tasks in order
    for (const auto& task_to_run : execution_order) {
        auto task_opt = getTask(task_to_run);
        if (!task_opt) {
            TaskResult result;
            result.success = false;
            result.error = "Task not found: " + task_to_run;
            result.task_name = task_to_run;
            return result;
        }

        const Task& task = *task_opt;
        
        if (options.verbose) {
            printTaskHeader(task_to_run, true);
        }

        // Substitute variables in command and args
        std::string cmd = substituteVariables(task.command);
        std::vector<std::string> args;
        for (const auto& arg : task.args) {
            args.push_back(substituteVariables(arg));
        }

        auto start = std::chrono::high_resolution_clock::now();
        TaskResult result = executeCommand(cmd, args, options);
        auto end = std::chrono::high_resolution_clock::now();

        result.execution_time_seconds = 
            std::chrono::duration<double>(end - start).count();
        result.task_name = task_to_run;

        if (!result.success) {
            printTaskResult(result);
            return result;
        }

        if (options.verbose) {
            printTaskResult(result);
        }
    }

    TaskResult final_result;
    final_result.success = true;
    final_result.task_name = task_name;
    return final_result;
}

bool TaskRunner::runAll(const std::vector<std::string>& task_names,
                       const TaskExecutionOptions& options) {
    last_results_.clear();

    for (const auto& task_name : task_names) {
        TaskResult result = run(task_name, options);
        last_results_.push_back(result);

        if (!result.success) {
            return false;
        }
    }

    return true;
}

std::vector<Task> TaskRunner::listTasks() const {
    std::vector<Task> tasks;
    for (const auto& [name, task] : tasks_) {
        tasks.push_back(task);
    }
    return tasks;
}

std::optional<Task> TaskRunner::getTask(const std::string& name) const {
    auto it = tasks_.find(name);
    if (it != tasks_.end()) {
        return it->second;
    }
    return std::nullopt;
}

bool TaskRunner::hasTask(const std::string& name) const {
    return tasks_.find(name) != tasks_.end();
}

std::vector<std::string> TaskRunner::resolveDependencies(const std::string& task_name) const {
    std::vector<std::string> order;
    std::map<std::string, bool> visited;
    std::map<std::string, bool> in_progress;

    if (!resolveDependenciesImpl(task_name, order, visited, in_progress)) {
        return {}; // Circular dependency
    }

    return order;
}

void TaskRunner::setVariables(const std::map<std::string, std::string>& variables) {
    variables_ = variables;
}

std::optional<std::string> TaskRunner::getVariable(const std::string& name) const {
    auto it = variables_.find(name);
    if (it != variables_.end()) {
        return it->second;
    }
    return std::nullopt;
}

void TaskRunner::clear() {
    tasks_.clear();
    variables_.clear();
    last_results_.clear();
}

TaskResult TaskRunner::executeCommand(const std::string& command,
                                     const std::vector<std::string>& args,
                                     const TaskExecutionOptions& options) {
    TaskResult result;

    // Build full command
    std::string full_cmd = command;
    for (const auto& arg : args) {
        full_cmd += " " + arg;
    }

    if (options.verbose) {
        std::cout << "Executing: " << full_cmd << "\n";
    }

    // Execute command
    int exit_code = std::system(full_cmd.c_str());

    result.success = (exit_code == 0);
    result.exit_code = exit_code;

    return result;
}

std::string TaskRunner::substituteVariables(const std::string& str) const {
    std::string result = str;
    
    std::regex var_regex(R"(\$\{([^}]+)\})");
    std::smatch match;
    
    while (std::regex_search(result, match, var_regex)) {
        std::string var_name = match[1].str();
        std::string replacement;
        
        auto var_opt = getVariable(var_name);
        if (var_opt) {
            replacement = *var_opt;
        }
        
        result = match.prefix().str() + replacement + match.suffix().str();
    }
    
    return result;
}

bool TaskRunner::resolveDependenciesImpl(const std::string& task_name,
                                        std::vector<std::string>& order,
                                        std::map<std::string, bool>& visited,
                                        std::map<std::string, bool>& in_progress) const {
    if (in_progress[task_name]) {
        return false; // Circular dependency
    }

    if (visited[task_name]) {
        return true;
    }

    auto task_opt = getTask(task_name);
    if (!task_opt) {
        return false;
    }

    const Task& task = *task_opt;

    in_progress[task_name] = true;

    // Process dependencies first
    for (const auto& dep : task.depends) {
        if (!resolveDependenciesImpl(dep, order, visited, in_progress)) {
            return false;
        }
    }

    in_progress[task_name] = false;
    visited[task_name] = true;
    order.push_back(task_name);

    return true;
}

bool TaskRunner::hasCircularDependency(const std::string& task_name) const {
    std::vector<std::string> order;
    std::map<std::string, bool> visited;
    std::map<std::string, bool> in_progress;
    return !resolveDependenciesImpl(task_name, order, visited, in_progress);
}

void TaskRunner::printTaskHeader(const std::string& task_name, bool verbose) const {
    if (verbose) {
        std::cout << "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        std::cout << "Running task: " << task_name << "\n";
        std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    }
}

void TaskRunner::printTaskResult(const TaskResult& result) const {
    if (result.success) {
        std::cout << "✓ Task '" << result.task_name << "' completed in " 
                  << result.execution_time_seconds << "s\n";
    } else {
        std::cerr << "✗ Task '" << result.task_name << "' failed (exit code: " 
                  << result.exit_code << ")\n";
        if (!result.error.empty()) {
            std::cerr << "  Error: " << result.error << "\n";
        }
    }
}

} // namespace build
} // namespace sad
