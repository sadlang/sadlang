// بسم الله الرحمن الرحيم
/**
 * @file task_runner.h
 * @brief Task Runner - منفذ المهام المخصصة
 * 
 * Executes custom tasks defined in sad.toml [tasks] section.
 * Handles task dependencies, argument substitution, and output capture.
 * 
 * @author Sad Language Team
 * @date 2026-01-07
 */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <optional>
#include <chrono>

namespace sad {
namespace build {

/**
 * @brief Task execution result - نتيجة تنفيذ المهمة
 */
struct TaskResult {
    bool success = false;               ///< Task succeeded
    int exit_code = 0;                  ///< Exit code
    std::string output;                 ///< Standard output
    std::string error;                  ///< Standard error
    double execution_time_seconds = 0.0;///< Execution time
    std::string task_name;              ///< Task that was executed
};

/**
 * @brief Task definition - تعريف المهمة
 */
struct Task {
    std::string name;                   ///< Task name
    std::string description;            ///< Task description
    std::string command;                ///< Command to execute
    std::vector<std::string> args;      ///< Command arguments
    std::vector<std::string> depends;   ///< Task dependencies
    bool is_background = false;         ///< Run in background
    
    /**
     * @brief Get full command string
     */
    std::string getFullCommand() const;
    
    /**
     * @brief Validate task definition
     */
    bool isValid() const;
};

/**
 * @brief Task execution options - خيارات تنفيذ المهمة
 */
struct TaskExecutionOptions {
    bool verbose = false;               ///< Print command before executing
    bool capture_output = true;         ///< Capture stdout/stderr
    std::string working_dir;            ///< Working directory
    std::map<std::string, std::string> env_vars; ///< Environment variables
    std::chrono::seconds timeout = std::chrono::seconds(300); ///< Timeout
};

/**
 * @brief Task runner class - منفذ المهام
 * 
 * Manages and executes custom tasks defined in sad.toml.
 * 
 * Features:
 * - Task registration
 * - Dependency resolution (topological sort)
 * - Parallel execution (when possible)
 * - Variable substitution in commands
 * - Output capture
 * - Background tasks
 * 
 * Example usage:
 * @code
 * TaskRunner runner;
 * 
 * Task build_task;
 * build_task.name = "build";
 * build_task.command = "sadc";
 * build_task.args = {"src/main.s", "-o", "build/app"};
 * runner.registerTask(build_task);
 * 
 * Task run_task;
 * run_task.name = "run";
 * run_task.command = "build/app";
 * run_task.depends = {"build"};
 * runner.registerTask(run_task);
 * 
 * TaskResult result = runner.run("run");
 * @endcode
 */
class TaskRunner {
public:
    /**
     * @brief Constructor
     */
    TaskRunner();
    
    /**
     * @brief Destructor
     */
    ~TaskRunner();

    /**
     * @brief Register a task - تسجيل مهمة
     * 
     * @param task Task to register
     * @return true if successful
     */
    bool registerTask(const Task& task);

    /**
     * @brief Register multiple tasks
     * 
     * @param tasks Map of task name to Task
     * @return Number of tasks registered
     */
    int registerTasks(const std::map<std::string, Task>& tasks);

    /**
     * @brief Run a task - تشغيل مهمة
     * 
     * Resolves dependencies and executes them in order before running the task.
     * 
     * @param task_name Name of task to run
     * @param options Execution options
     * @return TaskResult with execution details
     * 
     * @code
     * TaskExecutionOptions opts;
     * opts.verbose = true;
     * TaskResult result = runner.run("deploy", opts);
     * if (result.success) {
     *     std::cout << "Deployment successful!\n";
     * }
     * @endcode
     */
    TaskResult run(const std::string& task_name, 
                  const TaskExecutionOptions& options = TaskExecutionOptions{});

    /**
     * @brief Run multiple tasks in order
     * 
     * @param task_names Names of tasks to run
     * @param options Execution options
     * @return true if all tasks succeeded
     */
    bool runAll(const std::vector<std::string>& task_names,
               const TaskExecutionOptions& options = TaskExecutionOptions{});

    /**
     * @brief List all registered tasks - سرد جميع المهام
     * 
     * @return Vector of tasks
     */
    std::vector<Task> listTasks() const;

    /**
     * @brief Get task by name
     * 
     * @param name Task name
     * @return Optional Task
     */
    std::optional<Task> getTask(const std::string& name) const;

    /**
     * @brief Check if task exists
     * 
     * @param name Task name
     * @return true if task is registered
     */
    bool hasTask(const std::string& name) const;

    /**
     * @brief Get task dependencies in execution order
     * 
     * Uses topological sort to resolve dependencies.
     * 
     * @param task_name Task name
     * @return Vector of task names in execution order, or empty if circular dependency
     */
    std::vector<std::string> resolveDependencies(const std::string& task_name) const;

    /**
     * @brief Set variable substitution map
     * 
     * Variables like ${output}, ${main}, etc. will be replaced.
     * 
     * @param variables Map of variable name to value
     * 
     * @code
     * runner.setVariables({
     *     {"output", "build/"},
     *     {"main", "src/main.s"}
     * });
     * @endcode
     */
    void setVariables(const std::map<std::string, std::string>& variables);

    /**
     * @brief Get variable value
     * 
     * @param name Variable name
     * @return Optional value
     */
    std::optional<std::string> getVariable(const std::string& name) const;

    /**
     * @brief Clear all registered tasks
     */
    void clear();

    /**
     * @brief Get last execution results
     * 
     * @return Vector of TaskResult for last runAll() call
     */
    const std::vector<TaskResult>& getLastResults() const { return last_results_; }

private:
    std::map<std::string, Task> tasks_;                 ///< Registered tasks
    std::map<std::string, std::string> variables_;      ///< Variable substitution map
    std::vector<TaskResult> last_results_;              ///< Last execution results
    
    /**
     * @brief Execute a single command
     * 
     * @param command Command to execute
     * @param args Command arguments
     * @param options Execution options
     * @return TaskResult
     */
    TaskResult executeCommand(const std::string& command,
                            const std::vector<std::string>& args,
                            const TaskExecutionOptions& options);

    /**
     * @brief Substitute variables in string
     * 
     * @param str String with variables (e.g., "${output}/app")
     * @return String with substituted values
     */
    std::string substituteVariables(const std::string& str) const;

    /**
     * @brief Resolve dependencies using topological sort
     * 
     * @param task_name Task name
     * @param order Output vector for resolved order
     * @param visited Set of visited tasks
     * @param in_progress Set of tasks currently being processed
     * @return true if successful (no circular dependencies)
     */
    bool resolveDependenciesImpl(const std::string& task_name,
                                std::vector<std::string>& order,
                                std::map<std::string, bool>& visited,
                                std::map<std::string, bool>& in_progress) const;

    /**
     * @brief Check for circular dependencies
     * 
     * @param task_name Task name
     * @return true if circular dependency detected
     */
    bool hasCircularDependency(const std::string& task_name) const;

    /**
     * @brief Print task execution header
     */
    void printTaskHeader(const std::string& task_name, bool verbose) const;

    /**
     * @brief Print task execution result
     */
    void printTaskResult(const TaskResult& result) const;
};

} // namespace build
} // namespace sad
