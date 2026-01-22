/**
 * TaskManager.h
 *
 * Task management system - replicates the task-based multitasking
 * architecture from the original Sinistar (ADDTASK, DROPTASK, etc.)
 *
 * In the original game, tasks were used for all game entities and
 * were executed in sequence each frame by the executive loop.
 */

#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <vector>
#include <memory>
#include <functional>

class GameObject;

/**
 * Task - represents a single concurrent task
 * Similar to the task workspace structure in the original
 */
class Task {
public:
    using UpdateFunc = std::function<void(float)>;

    Task(GameObject* object, UpdateFunc updateFunc, int priority = 0)
        : object_(object)
        , updateFunc_(updateFunc)
        , priority_(priority)
        , active_(true)
    {}

    void update(float deltaTime) {
        if (active_ && updateFunc_) {
            updateFunc_(deltaTime);
        }
    }

    bool isActive() const { return active_; }
    void setActive(bool active) { active_ = active; }
    int getPriority() const { return priority_; }
    GameObject* getObject() const { return object_; }

private:
    GameObject* object_;
    UpdateFunc updateFunc_;
    int priority_;
    bool active_;
};

/**
 * TaskManager - manages all active tasks
 * Replicates ADDTASK, DROPTASK, and task execution from EXEC.SRC
 */
class TaskManager {
public:
    TaskManager();
    ~TaskManager();

    /**
     * Add a new task (ADDTASK equivalent)
     * @param object GameObject this task belongs to
     * @param updateFunc Function to call each frame
     * @param priority Higher priority tasks execute first
     * @return Task ID for later reference
     */
    int addTask(GameObject* object, Task::UpdateFunc updateFunc, int priority = 0);

    /**
     * Remove a task (DROPTASK equivalent)
     * @param taskId ID returned from addTask
     */
    void removeTask(int taskId);

    /**
     * Update all active tasks
     * Called once per frame by the executive loop
     * @param deltaTime Time since last update in seconds
     */
    void update(float deltaTime);

    /**
     * Get number of active tasks
     */
    size_t getTaskCount() const;

    /**
     * Clear all tasks
     */
    void clear();

private:
    std::vector<std::unique_ptr<Task>> tasks_;
    int nextTaskId_;

    /**
     * Sort tasks by priority
     */
    void sortTasks();
};

#endif // TASKMANAGER_H
