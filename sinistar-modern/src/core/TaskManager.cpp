/**
 * TaskManager.cpp
 *
 * Implementation of task management system
 */

#include "TaskManager.h"
#include "../entities/GameObject.h"
#include <algorithm>
#include <iostream>

TaskManager::TaskManager()
    : nextTaskId_(0)
{
    std::cout << "TaskManager initialized" << std::endl;
}

TaskManager::~TaskManager() {
    clear();
    std::cout << "TaskManager destroyed" << std::endl;
}

int TaskManager::addTask(GameObject* object, Task::UpdateFunc updateFunc, int priority) {
    auto task = std::make_unique<Task>(object, updateFunc, priority);
    tasks_.push_back(std::move(task));

    // Sort by priority after adding
    sortTasks();

    return nextTaskId_++;
}

void TaskManager::removeTask(int taskId) {
    // Mark task as inactive (will be removed during next update)
    // This prevents iterator invalidation during update loop
    if (taskId >= 0 && taskId < static_cast<int>(tasks_.size())) {
        tasks_[taskId]->setActive(false);
    }
}

void TaskManager::update(float deltaTime) {
    // Execute all active tasks in priority order
    for (auto& task : tasks_) {
        if (task && task->isActive()) {
            task->update(deltaTime);
        }
    }

    // Remove inactive tasks
    tasks_.erase(
        std::remove_if(tasks_.begin(), tasks_.end(),
            [](const std::unique_ptr<Task>& task) {
                return !task || !task->isActive();
            }),
        tasks_.end()
    );
}

size_t TaskManager::getTaskCount() const {
    return std::count_if(tasks_.begin(), tasks_.end(),
        [](const std::unique_ptr<Task>& task) {
            return task && task->isActive();
        });
}

void TaskManager::clear() {
    tasks_.clear();
    nextTaskId_ = 0;
}

void TaskManager::sortTasks() {
    // Sort by priority (higher priority first)
    std::sort(tasks_.begin(), tasks_.end(),
        [](const std::unique_ptr<Task>& a, const std::unique_ptr<Task>& b) {
            return a->getPriority() > b->getPriority();
        });
}
